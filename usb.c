#include "stm32f0x.h"
#include <stdint.h>

unsigned char blog[64];
unsigned char logindex = 0;

#define LOG(_x)   if (logindex < 64) { blog[logindex++] = (_x); } else BREAK();

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
	unsigned char *tx_buf;
	int tx_count;
};

#define BREAK() 		__asm__("bkpt 0");

static enum usb_device_state usb_device_state;

#define DEVICE_CLASS         2
#define DEVICE_SUBCLASS      0
#define DEVICE_PROTOCOL      2

#define USB_MANUFACTURER     "ACME Gadgets Inc."
#define USB_PRODUCT          "USB Gadget"
#define USB_SERIAL_NUMBER    "00000000"
#define USB_CONF_1		"c1"

#define MANUFACTURER_INDEX 0
#define PRODUCT_INDEX (sizeof(USB_MANUFACTURER))
#define SERIAL_NUMBER_INDEX (PRODUCT_INDEX + sizeof(USB_PRODUCT)) 
#define USB_CONF_INDEX (SERIAL_NUMBER_INDEX + sizeof(USB_SERIAL_NUMBER))

#define usb_descriptor_not_present ((unsigned char *)1)

const char usb_string_descriptor[] =
	USB_MANUFACTURER			\
	USB_PRODUCT				\
	USB_SERIAL_NUMBER;

const unsigned char usb_device_descriptor[] = {
	17,                     /* bLength */
	1,                      /* bDescriptorType */
	0x00, 0x02,		/* bcdUSB */
	DEVICE_CLASS, 	      	/* bDeviceClass */
	DEVICE_SUBCLASS,	/* bDeviceSubClass */
	DEVICE_PROTOCOL,       	/* bDeviceProtocol */
	32,			/* bMaxPacketSize (valid: 8,16,32,64) */
	0xfe, 0xca,		/* idVendor */
	0x0d, 0xf0,		/* idProduct */
	0x00, 0x00,		/* bcdDevice */
	MANUFACTURER_INDEX,	/* iManufacturer */
	PRODUCT_INDEX,		/* iProduct */
	SERIAL_NUMBER_INDEX,    /* iSerialNumber */
	1 			/* bNumConfigurations */
};

const unsigned char usb_configuration_descriptor[] = {
	/* Configuration */
	9,			/* bLength */
	2,			/* bDescriptorType */
	18, 0,			/* wTotalLength */
	0,			/* bNumInterfaces */
	1,			/* bConfigurationValue */
	USB_CONF_INDEX,		/* iConfiguration */
	0x80,			/* bmAttributes */
	50,			/* bMaxPower */

	/* First Interface */
	9,			/* bLength */
	4,			/* bDescriptorType */
	0,			/* bInterfaceNumber */
	0,			/* bAlternateSetting */
	0,			/* bNumEndpoints */
	0,			/* bInterfaceClass */
	0,			/* bInterfaceSubClass */
	0,			/* bInterfaceProtocol */
	0,			/* iInterface */
};

struct usb_endpoint usb_endpoint[] = {
	/* Control */
	{
		.index = 0,
		.tx_offset = 100,
		.tx_max = 64,
		.rx_offset = 8,
		.rx_max = 64,
		.tx_buf = 0,
		.tx_count = 0,
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

static void _copy_from_sram(void *dst, const volatile void *src, int len)
{
	const volatile uint16_t *p = src;
	unsigned char *q = dst;

	while (len) {
		unsigned int val;

		__asm__("ldrh %0, [%1]" : "=r" (val) : "r" (p));

		len--;
		*q++ = val & 0xff;
		if (len--)
			*q++ = (val >> 8) & 0xff;

		p++;
	}

}

static void _copy_to_sram(volatile void *dst, const void *src, int len)
{
	const unsigned char *p = src;
	volatile uint16_t *q = dst;
	uint16_t data;

	while (len) {
		data = *p++;
		len--;

		if (len) {
			data |= (*p++) << 8;
			len--;
		}

		__asm__("strh %0, [%1]" : : "r" (data), "r" (q));

		q++;
	}
}

static void usb_transmit(struct usb_endpoint *e, const unsigned char *data, int len)
{
	int ep = e->index;
	volatile unsigned int *EPR = &USB.EPR[ep];
	volatile uint16_t *desc = &USB_SRAM[4*ep];

	_copy_to_sram(SRAM_ADDR_TO_PTR(e->tx_offset), data, len);

	_write_sram(desc + 1, len);

	EPR_CLEAR_CTR_TX(EPR);
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
}

void usb_setup_control_endpoint()
{
	volatile unsigned int *EPR = &USB.EPR[0];
	volatile uint16_t *desc = &USB_SRAM[0];

	_write_sram(desc + 0, usb_endpoint[0].tx_offset);		/* USB_ADDRn_TX */
	_write_sram(desc + 2, usb_endpoint[0].rx_offset);		/* USB_ADDRn_RX */
	_write_sram(desc + 1, 0);					/* USB_COUNTn_TX */
	_write_sram(desc + 3, 0x8000 | (1<<10));			/* USB_COUNTn_RX */

	EPR_SET_ADDR(EPR, 0);
	EPR_SET_TYPE(EPR, 1);
	EPR_SET_STAT_TX(EPR, EP_STATE_NAK);
	EPR_SET_STAT_RX(EPR, EP_STATE_VALID);


}

void usb_init()
{
	/* Bring up the USB macrocell */
	USB.BCDR |= 0x8000;
	USB.CNTR = 1;		/* /PWRDN, enter RESET */

	{ int i; for (i=0;i<25000;i++); }

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

unsigned char buf[16];


int descriptor_count = 0;

static int usb_get_descriptor(int index, int type, unsigned char **data, int *len)
{
	switch (type) {
	case 1:			/* DEVICE */
		LOG('d');
		*data = (unsigned char *)&usb_device_descriptor;
		*len = sizeof(usb_device_descriptor);
		return 0;
		break;
	case 2:			/* CONFIGURATION */
		LOG('c');
		*data = (unsigned char *)&usb_configuration_descriptor;
		*len = sizeof(usb_configuration_descriptor);
		return 0;
		break;
	case 6:			/* QUALIFIER */
		*data = usb_descriptor_not_present;
		*len = 0;
		return 0;
		break;
	default:
		*data = 0;
		BREAK();
		break;
	}
}

int address_count = 0;

static void usb_packet_recv(int ep)
{
	volatile uint16_t *desc = &USB_SRAM[4*ep];
	uint16_t addr_rx;
	uint16_t count_rx;
	volatile unsigned int *EPR = &USB.EPR[ep];
	volatile void *src;
	unsigned char *data;

	addr_rx = _read_sram(desc + 2);
	count_rx = _read_sram(desc + 3);
	src = SRAM_ADDR_TO_PTR(addr_rx);
	_copy_from_sram(buf, src, count_rx & 0x3ff);

	if ((*EPR) & USB_EPR_SETUP) {
		LOG('S');
		/* Unlock the SETUP bit */
		EPR_CLEAR_CTR_RX(EPR);
		switch (buf[1]) {
		case 0x05:	/* SET_ADDRESS */
			LOG('A');
			usb_device.address = buf[2];
			usb_endpoint[0].tx_buf = (unsigned char *)1;
			usb_endpoint[0].tx_count = 0;
			address_count++;
			break;
		case 0x06:	/* GET_DESCRIPTOR */
			LOG('D');
			usb_get_descriptor(buf[2], buf[3], &usb_endpoint[0].tx_buf, &usb_endpoint[0].tx_count);
			break;
		default:
			BREAK();
			break;
		}
	} else {
		if (count_rx & 0x3ff)
			BREAK();
	}
}

void __attribute__((interrupt("IRQ"))) _USB_handler()
{
	unsigned int istr = USB.ISTR;

	if (USB.ISTR & USB_ISTR_RESET) {
		usb_device.state = INIT;
		usb_setup_control_endpoint();
		USB.ISTR = ~USB_ISTR_RESET;
		USB.DADDR |= 0x80;
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

		USB.ISTR = ~USB_ISTR_WKUP;
	}


	if (USB.ISTR & USB_ISTR_CTR) {
		unsigned int ep = EP_ID(USB.ISTR);

		USB.ISTR = ~USB_ISTR_CTR;
		icount.CTR++;

		LOG('I');

		if (USB.ISTR & USB_ISTR_DIR) {
			LOG('r');
			usb_packet_recv(ep);
		} else {
			EPR_CLEAR_CTR_TX(&USB.EPR[0]);
			LOG('t');
			usb_endpoint[ep].tx_buf = 0;
			if (usb_device.address && !(USB.DADDR & 0x7f)) {
				USB.DADDR = 0x80 | usb_device.address;
				usb_device.state = ADDRESS;
			}
		}

		/* If we've got more to send, queue it up. Otherwise receive. */
		if (usb_endpoint[ep].tx_buf) {
			LOG('T');
			usb_transmit(&usb_endpoint[ep], usb_endpoint[ep].tx_buf, usb_endpoint[ep].tx_count);
			if (usb_endpoint[ep].tx_buf == usb_configuration_descriptor)
				BREAK();
		} else {
			LOG('R');
			usb_receive(&usb_endpoint[ep]);
		}
	}

	if (USB.ISTR & USB_ISTR_L1REQ) {
		USB.ISTR = ~USB_ISTR_L1REQ;
		icount.L1REQ++;
	}

	if (USB.ISTR & USB_ISTR_ERR) {
		USB.ISTR = ~USB_ISTR_ERR;
		icount.ERR++;
	}

	if (USB.ISTR & USB_ISTR_PMAOVR) {
		USB.ISTR = ~USB_ISTR_PMAOVR;
		icount.PMAOVR++;
	}

}
