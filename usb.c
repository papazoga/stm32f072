#include "stm32f0x.h"
#include <stdint.h>

#ifdef DEBUG

static void break_and_show_state(struct usb_endpoint *e);
#define BREAK() 		__asm__("bkpt 0");

#define BLOGLEN 128
unsigned char blog[BLOGLEN];
unsigned char logindex = 0;

#define LOG(_x)   if (logindex < BLOGLEN) { blog[logindex++] = (_x); } else BREAK();
#else

#define LOG(_x)

#endif /* DEBUG */

enum {
	MANUFACTURER_INDEX = 1,
	PRODUCT_INDEX = 2,
	SERIALNUMBER_INDEX = 3,
	CONFIGURATION_INDEX = 4,
};

struct {
	enum usb_device_state {
		INIT = 0,
		ADDRESS = 1,
		CONFIGURED = 2,
	} state;
	unsigned char address;
} usb_device = {
	.state = INIT,
	.address = 0,
};

struct usb_endpoint {
	int index;
	int tx_offset;
	int tx_max;
	int rx_offset;
	int rx_max;
};

enum descriptor_type {
	DEVICE=1,
	CONFIGURATION,
	STRING,
	INTERFACE,
	ENDPOINT,
	DEVICE_QUALIFIER,
	OTHER_SPEED_CONFIGURATION,
	INTERFACE_POWER,
};

enum standard_request_type {
	GET_STATUS = 0,
	CLEAR_FEATURE = 1,
	SET_FEATURE = 3,
	SET_ADDRESS = 5,
	GET_DESCRIPTOR,
	SET_DESCRIPTOR,
	GET_CONFIGURATION,
	SET_CONFIGURATION,
	GET_INTERFACE,
	SET_INTERFACE,
	SYNCH_FRAME,
};

#define DEVICE_CLASS         2
#define DEVICE_SUBCLASS      0
#define DEVICE_PROTOCOL      2

const char manufacturer_string[] = "ACME Gadgets Inc.";
const char product_string[] = "USB Gadget";
const char serialnumber_string[] = "1234";
const char configuration_string[] = "Config 1";

const unsigned char usb_device_descriptor[] = {
	18,                     /* bLength */
	1,                      /* bDescriptorType */
	0x00, 0x02,		/* bcdUSB */
	DEVICE_CLASS, 	      	/* bDeviceClass */
	DEVICE_SUBCLASS,	/* bDeviceSubClass */
	DEVICE_PROTOCOL,       	/* bDeviceProtocol */
	64,			/* bMaxPacketSize (valid: 8,16,32,64) */
	0xfe, 0xca,		/* idVendor */
	0x0d, 0xf0,		/* idProduct */
	0x00, 0x00,		/* bcdDevice */
	MANUFACTURER_INDEX,	/* iManufacturer */
	PRODUCT_INDEX,		/* iProduct */
	SERIALNUMBER_INDEX,	/* iSerialNumber */
	1 			/* bNumConfigurations */
};

const unsigned char usb_configuration_descriptor[] = {
	/* Configuration */
	9,			/* bLength */
	2,			/* bDescriptorType */
	9, 0,			/* wTotalLength */
	0,			/* bNumInterfaces */
	1,			/* bConfigurationValue */
	CONFIGURATION_INDEX,   	/* iConfiguration */
	0x80,			/* bmAttributes */
	50,			/* bMaxPower */
};

struct usb_endpoint usb_endpoint[] = {
	/* Control */
	{
		.index = 0,
		.tx_offset = 100,
		.tx_max = 64,
		.rx_offset = 8,
		.rx_max = 64,
	},
};

struct icount {
	int RESET;
	int L1REQ;
	int ESOF;
	int SOF;
	int SUSP;
	int WKUP;
	int ERR;
	int PMAOVR;
	int CTR;
} icount;

#define SRAM_ADDR_TO_PTR(_addr)   (&USB_SRAM[_addr/2])


/*
 * The interface to the SRAM used for buffers and descriptors
 * requires that all memory accesses be word-width and word-aligned.
 */
inline static uint16_t _read_sram(const volatile void *src)
{
	uint16_t val;
	__asm__("ldrh %0, [%1]" : "=r" (val) : "r" (src));
	return val;
}

inline static void _write_sram(volatile void *dst, uint16_t val)
{
	__asm__("strh %0, [%1]" : : "r" (val), "r" (dst));
}

inline static void _copy_from_sram(void *dst, const volatile void *src, int len)
{
	__asm__("loop%=:	\n\t" \
		"ldrh r5, [%1]	\n\t" \
		"strh r5, [%2] 	\n\t" \
		"add %1, %1, #2	\n\t" \
		"add %2, %2, #2	\n\t" \
		"sub %0, %0, #2	\n\t" \
		"cmp %0, #1    	\n\t" \
		"bgt loop%=    	\n\t" \
		"blt done%=    	\n\t" \
		"ldrh r5, [%1] 	\n\t" \
		"strb r5, [%2]	\n\t" \
		"done%=:	\n\t" \
		:
		: "r" (len), "r" (src), "r" (dst)
		: "r5");
}

inline static void _copy_to_sram(volatile void *dst, const void *src, int len)
{
	__asm__("loop%=:	\n\t" \
		"ldrh r5, [%1]	\n\t" \
		"strh r5, [%2]	\n\t" \
		"add %1, %1, #2	\n\t" \
		"add %2, %2, #2	\n\t" \
		"sub %0, %0, #2	\n\t" \
		"cmp %0, #1	\n\t" \
		"bgt loop%=	\n\t" \
		"blt done%=	\n\t" \
		"ldrb r5, [%1]	\n\t" \
		"strh r5, [%2]	\n\t" \
		"done%=:	\n\t" \
		:
		: "r" (len), "r" (src), "r" (dst)
		: "r5");
}

static void usb_set_transmit_state(struct usb_endpoint *e, unsigned char state)
{
	int ep = e->index;
	volatile unsigned int *EPR = &USB.EPR[ep];

	EPR_CLEAR_CTR_TX(EPR);
	EPR_SET_STAT_TX(EPR, state);
}

#ifdef DEBUG

static void break_and_show_state(struct usb_endpoint *e)
{
	struct usb_regs usb;

	int i;
	unsigned int *p = (unsigned int *)&usb,
		*q = (unsigned int *)&USB;

	for (i=0;i<(sizeof(struct usb_regs)/4);i++)
		p[i] = q[i];


	struct {
		unsigned char CTR_RX;
		unsigned char DTOG_RX;
		unsigned char STAT_RX;
		unsigned char SETUP;
		unsigned char CTR_TX;
		unsigned char DTOG_TX;
 char STAT_TX;
	} state = {
		.CTR_RX = (usb.EPR[0] & USB_EPR_CTR_RX) >> 15,
		.DTOG_RX = (usb.EPR[0] & USB_EPR_DTOG_RX) >> 14,
		.STAT_RX = (usb.EPR[0] & (3<<12)) >> 12,
		.SETUP = (usb.EPR[0] & USB_EPR_SETUP) >> 11,
		.CTR_TX = (usb.EPR[0] & USB_EPR_CTR_TX) >> 7,
		.DTOG_TX = (usb.EPR[0] & USB_EPR_DTOG_TX) >> 6,
		.STAT_TX = (usb.EPR[0] & (3<<4)) >> 4,
	};

	state;
	usb;
	BREAK();
}

#endif	/* DEBUG */


static void usb_transmit(struct usb_endpoint *e, const unsigned char *data, int len)
{
	int ep = e->index;
	volatile unsigned int *EPR = &USB.EPR[ep];
	volatile uint16_t *desc = &USB_SRAM[4*ep];

	_copy_to_sram(SRAM_ADDR_TO_PTR(e->tx_offset), data, len);

	_write_sram(desc + 1, len);

	EPR_CLEAR_CTR_TX(EPR);
	EPR_SET_TOGGLE(EPR, USB_EPR_DTOG_RX, 0);
	EPR_SET_STAT_TX(EPR, EP_STATE_VALID);
}

static void usb_receive(struct usb_endpoint *e)
{
	int ep = e->index;
	volatile unsigned int *EPR = &USB.EPR[ep];
	volatile uint16_t *desc = &USB_SRAM[4*ep];

	_write_sram(desc + 2, e->rx_offset);		/* USB_ADDRn_RX */

	EPR_CLEAR_CTR_RX(EPR);
	EPR_SET_STAT_RX(EPR, EP_STATE_VALID);
	EPR_SET_TOGGLE(EPR, USB_EPR_DTOG_RX, 0);
}

void usb_setup_control_endpoint()
{
	volatile unsigned int *EPR = &USB.EPR[0];
	volatile uint16_t *desc = &USB_SRAM[0];

	_write_sram(desc + 0, usb_endpoint[0].tx_offset);		/* USB_ADDRn_TX */
	_write_sram(desc + 1, 0);					/* USB_COUNTn_TX */
	_write_sram(desc + 2, usb_endpoint[0].rx_offset);		/* USB_ADDRn_RX */
	_write_sram(desc + 3, 0x8000 | (1<<10));			/* USB_COUNTn_RX */

	EPR_SET_ADDR(EPR, 0);
	EPR_SET_TYPE(EPR, 1);
	EPR_SET_STAT_TX(EPR, EP_STATE_NAK);
	EPR_SET_STAT_RX(EPR, EP_STATE_NAK);

	USB.DADDR = 0x80;
}

void usb_init()
{
	/* Bring up the USB macrocell */
	USB.BCDR |= 0x8000;
	USB.CNTR = 1;		/* /PWRDN, enter RESET */
	USB.CNTR = 0;

	/* Set interrupts */
	USB.CNTR = USB_CNTR_RESETM |
		USB_CNTR_L1REQM |
		USB_CNTR_ESOFM |
		USB_CNTR_SOFM |
		USB_CNTR_RESETM |
		USB_CNTR_SUSPM |
		USB_CNTR_WKUPM |
		USB_CNTR_ERRM |
		USB_CNTR_PMAOVRM |
		USB_CNTR_CTRM;
}

#define STRING_DESCRIPTOR(_str) \
	{ sizeof(_str)-1, _str }

struct {
	int length;
	const char *string;
} string_descriptor[] = {
	{ 0, 0 },
	STRING_DESCRIPTOR(manufacturer_string),
	STRING_DESCRIPTOR(product_string),
	STRING_DESCRIPTOR(serialnumber_string),
	STRING_DESCRIPTOR(configuration_string),
};

static void usb_transmit_string_descriptor(struct usb_endpoint *e, int index)
{
	int ep = e->index;
	volatile unsigned int *EPR = &USB.EPR[ep];
	volatile uint16_t *desc = &USB_SRAM[4*ep];
	int len = string_descriptor[index].length;
	const char *p = string_descriptor[index].string;
	volatile uint16_t *datap = SRAM_ADDR_TO_PTR(e->tx_offset);

	/*
	 * String Descriptor 0 lists supported languages.
	 * We only support US English for now.
	 */
	if (index == 0) {
		_write_sram(datap++, 0x0304);
		_write_sram(datap++, 0x0409);
		len = 1;
		goto send;
	}

	_write_sram(datap++, ((2*len+2) & 0xff) | (3 << 8));


	/* Convert to 16-bit UNICODE */
	while (*p)
		_write_sram(datap++, *p++);

send:
	_write_sram(desc + 1, 2*len + 2);

	EPR_CLEAR_CTR_TX(EPR);
	EPR_SET_TOGGLE(EPR, USB_EPR_DTOG_RX, 0);
	EPR_SET_TOGGLE(EPR, USB_EPR_DTOG_TX, USB_EPR_DTOG_TX);
	EPR_SET_STAT_TX(EPR, EP_STATE_VALID);
}

static int usb_transmit_descriptor(struct usb_endpoint *e, int index, enum descriptor_type type)
{
	switch (type) {
	case DEVICE:
		LOG('d');
		usb_transmit(e, usb_device_descriptor, sizeof(usb_device_descriptor));
		break;
	case CONFIGURATION:
		LOG('c');
		usb_transmit(e, usb_configuration_descriptor, sizeof(usb_configuration_descriptor));
		break;
	case STRING:
		LOG('s');
		usb_transmit_string_descriptor(e, index);
		break;
	default:
		usb_set_transmit_state(e, EP_STATE_STALL);
		break;
	}

	return 0;
}

static void usb_packet_recv(struct usb_endpoint *e)
{
	int ep = e->index;
	volatile uint16_t *desc = &USB_SRAM[4*ep];
	volatile unsigned int *EPR = &USB.EPR[ep];
	uint16_t addr_rx;
	uint16_t count_rx;
	enum standard_request_type type;

	addr_rx = _read_sram(desc + 2);
	count_rx = _read_sram(desc + 3) & 0x3ff;

	if (((*EPR) & USB_EPR_SETUP) && (count_rx < 32)) {
		unsigned char buf[count_rx];
		LOG('S');

		/* Copy the request onto the stack (up to 32 bytes) */
		_copy_from_sram(buf, SRAM_ADDR_TO_PTR(addr_rx), count_rx);

		/* Unlock the SETUP bit */
		EPR_CLEAR_CTR_RX(EPR);
		type = buf[1];

		switch (type) {
		case SET_ADDRESS:
			LOG('A');
			usb_device.address = buf[2];
			usb_transmit(e, 0, 0);
			break;
		case GET_DESCRIPTOR:
			LOG('D');
			usb_transmit_descriptor(e, buf[2], buf[3]);
			break;
		case SET_CONFIGURATION:
			LOG('C');
			usb_transmit(e, 0, 0);
			break;
		default:
			usb_set_transmit_state(e, EP_STATE_STALL);
			break;
		}
	} else {
		usb_set_transmit_state(e, EP_STATE_STALL);
	}
}

static void usb_transfer_handler(struct usb_endpoint *e)
{
	if (USB.ISTR & USB_ISTR_DIR) {
		/* OUT or SETUP direction */
		LOG('r');
		usb_packet_recv(e);
	} else {
		/* IN */
		if (usb_device.address) {
			USB.DADDR = usb_device.address | 0x80;
			usb_device.state = ADDRESS;
		}
		EPR_CLEAR_CTR_TX(&USB.EPR[0]);
		EPR_SET_TOGGLE(&USB.EPR[0], USB_EPR_DTOG_RX, 0);
		LOG('t');
	}
}

void __attribute__((interrupt("IRQ"))) _USB_handler()
{
	if (USB.ISTR & USB_ISTR_RESET) {
		USB.DADDR = 0;
		usb_device.state = INIT;
		usb_device.address = 0;
		usb_setup_control_endpoint();
		USB.ISTR = ~USB_ISTR_RESET;
		LOG('0');
		icount.RESET++;
	}

	if (USB.ISTR & USB_ISTR_ESOF) {
		USB.ISTR = ~USB_ISTR_ESOF;
		icount.ESOF++;
	}

	if (USB.ISTR & USB_ISTR_SOF) {
		USB.ISTR = ~USB_ISTR_SOF;
		icount.SOF++;
	}

	if (USB.ISTR & USB_ISTR_SUSP) {
		USB.ISTR = ~USB_ISTR_SUSP;
		icount.SUSP++;
		USB.CNTR |= USB_CNTR_FSUSP | USB_CNTR_LPMODE;
		LOG('-');
	}

	if (USB.ISTR & USB_ISTR_WKUP) {
		icount.WKUP++;
		USB.CNTR = ~USB_CNTR_LPMODE;

		/* Set interrupts */
		USB.CNTR = USB_CNTR_RESETM |
			USB_CNTR_L1REQM |
			USB_CNTR_ESOFM |
			USB_CNTR_SOFM |
			USB_CNTR_RESETM |
			USB_CNTR_SUSPM |
			USB_CNTR_WKUPM |
			USB_CNTR_ERRM |
			USB_CNTR_PMAOVRM |
			USB_CNTR_CTRM;

		LOG('+');
		USB.ISTR = ~USB_ISTR_WKUP;
	}


	if (USB.ISTR & USB_ISTR_CTR) {
		unsigned int ep = EP_ID(USB.ISTR);

		USB.ISTR = ~USB_ISTR_CTR;
		icount.CTR++;

		LOG('I');
		usb_transfer_handler(&usb_endpoint[ep]);
	}

	if (USB.ISTR & USB_ISTR_L1REQ) {
		USB.ISTR = ~USB_ISTR_L1REQ;
		icount.L1REQ++;
	}

	if (USB.ISTR & USB_ISTR_ERR) {
		USB.ISTR = ~USB_ISTR_ERR;
		LOG('!');
		icount.ERR++;
	}

	if (USB.ISTR & USB_ISTR_PMAOVR) {
		USB.ISTR = ~USB_ISTR_PMAOVR;
		icount.PMAOVR++;
	}

}
