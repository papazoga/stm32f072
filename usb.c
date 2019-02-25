#include <stdint.h>
#include "stm32f0x.h"
#include "usb.h"

#ifdef DEBUG

static void break_and_show_state(struct usb_endpoint *e);
#define BREAK() 		__asm__("bkpt 0");

unsigned char binary_log[BLOGLEN];
unsigned char binary_log_index = 0;

#endif /* DEBUG */

enum {
	MANUFACTURER_INDEX = 1,
	PRODUCT_INDEX = 2,
	SERIALNUMBER_INDEX = 3,
	CONFIGURATION_INDEX = 4,
};

#define DEVICE_CLASS         2
#define DEVICE_SUBCLASS      0
#define DEVICE_PROTOCOL      2

const char manufacturer_string[] = "ACME Gadgets Inc.";
const char product_string[] = "USB Gadget";
const char serialnumber_string[] = "1234";
const char configuration_string[] = "Config 1";

struct string_descriptor string_descriptor[] = {
	{ 0, 0 },
	STRING_DESCRIPTOR(manufacturer_string),
	STRING_DESCRIPTOR(product_string),
	STRING_DESCRIPTOR(serialnumber_string),
	STRING_DESCRIPTOR(configuration_string),
};


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

struct usb_ctrl_info usb_ctrl_endpoint_info = {
	.device_desc = usb_device_descriptor,
	.configuration_desc = usb_configuration_descriptor,
	.string_desc_table = string_descriptor,
};

struct usb_endpoint usb_endpoint[] = {
	/* Control */
	{
		.index = 0,
		.tx_offset = 100,
		.tx_max = 64,
		.rx_offset = 8,
		.rx_max = 64,
		.ops = &usb_ctrl_ops,
		.priv = &usb_ctrl_endpoint_info,
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


void usb_set_transmit_state(struct usb_endpoint *e, unsigned char state)
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


void usb_transmit(struct usb_endpoint *e, const unsigned char *data, int len)
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

void usb_receive(struct usb_endpoint *e)
{
	int ep = e->index;
	volatile unsigned int *EPR = &USB.EPR[ep];
	volatile uint16_t *desc = &USB_SRAM[4*ep];

	_write_sram(desc + 2, e->rx_offset);		/* USB_ADDRn_RX */

	EPR_CLEAR_CTR_RX(EPR);
	EPR_SET_STAT_RX(EPR, EP_STATE_VALID);
	EPR_SET_TOGGLE(EPR, USB_EPR_DTOG_RX, 0);
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


static void usb_transfer_handler(struct usb_endpoint *e)
{
	if (USB.ISTR & USB_ISTR_DIR) {
		/* OUT or SETUP direction */
		LOG('r');
		e->ops->recv(e);
	} else {
		LOG('t');
		e->ops->xmit(e);
	}
}

void __attribute__((interrupt("IRQ"))) _USB_handler()
{
	if (USB.ISTR & USB_ISTR_RESET) {
		usb_endpoint[0].ops->init(&usb_endpoint[0]);
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
