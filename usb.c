#include "stm32f0x.h"
#include <stdint.h>

const char usb_manufacturer[] = "ACME Gadgets Inc.";
const char usb_product[] = "USB Gadget";
const char usb_serialnumber[] = "00000000";

#define MANUFACTURER_INDEX   0
#define PRODUCT_INDEX        (sizeof(usb_manufacturer) + 1)
#define SERIALNUMBER_INDEX   (sizeof(usb_manufacturer) + sizeof(usb_serialnumber) + 2)

#define DEVICE_CLASS         2
#define DEVICE_SUBCLASS      0
#define DEVICE_PROTOCOL      2

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
	MANUFACTURER_INDEX,	/* iManufacturer */
	PRODUCT_INDEX,		/* iProduct */
	SERIALNUMBER_INDEX,     /* iSerialNumber */
	1 			/* bNumConfigurations */
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


void usb_setup_endpoint0()
{
	int i;

	for (i=4;i<0x100;i+=2)
		USB_SRAM[i] = 0;

	USB_SRAM[0] = 100;	/* ADDR_TX */
	USB_SRAM[1] = 30;	/* COUNT_TX */
	USB_SRAM[2] = 8;	/* ADDR_RX */
	USB_SRAM[3] = 0x8000 | (1<<10);	/* COUNT_RX: BL_SIZE=1, COUNT=1 */
	WRITE_EPR(USB.EP0R, USB_EPR_EP_TYPE(1) | (USB_EPR_STAT_RX(3) | USB_EPR_STAT_TX(2)));
}

void __attribute__((interrupt("IRQ"))) _USB_handler()
{
	unsigned int istr = USB.ISTR;

	if (USB.ISTR & USB_ISTR_RESET) {
		usb_setup_endpoint0();
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
		USB.CNTR &= ~USB_CNTR_LPMODE;

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

		__asm__("bkpt 0");
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
