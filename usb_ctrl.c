#include <stdint.h>
#include "stm32f0x.h"
#include "usb.h"

static int usb_ctrl_recv(struct usb_endpoint *);

static void usb_transmit_string_descriptor(struct usb_endpoint *e, struct string_descriptor *sdesc)
{
	int ep = e->index;
	volatile unsigned int *EPR = &USB.EPR[ep];
	volatile uint16_t *desc = &USB_SRAM[4*ep];
	int len = sdesc->length;
	const char *p = sdesc->string;
	volatile uint16_t *datap = SRAM_ADDR_TO_PTR(e->tx_offset);

	/*
	 * String Descriptor 0 lists supported languages.
	 * We only support US English for now.
	 */
	if (sdesc->length == 0) {
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
	struct usb_ctrl_info *info = e->priv;
	const unsigned char *desc;

	switch (type) {
	case DEVICE:
		LOG('d');
		desc = info->device_desc;
		usb_transmit(e, desc, desc[0]);
		break;
	case CONFIGURATION:
		LOG('c');
		desc = info->configuration_desc;
		usb_transmit(e, desc, *(uint16_t *)(desc+2));
		break;
	case STRING:
		LOG('s');
		usb_transmit_string_descriptor(e, info->string_desc_table + index);
		break;
	default:
		usb_set_transmit_state(e, EP_STATE_STALL);
		break;
	}

	return 0;
}

static int usb_ctrl_init(struct usb_endpoint *e)
{
	volatile unsigned int *EPR = &USB.EPR[e->index];
	volatile uint16_t *desc = &USB_SRAM[4*e->index];
	struct usb_ctrl_info *info = e->priv;

	_write_sram(desc + 0, e->tx_offset);	 /* USB_ADDRn_TX */
	_write_sram(desc + 1, 0);		 /* USB_COUNTn_TX */
	_write_sram(desc + 2, e->rx_offset);	 /* USB_ADDRn_RX */
	_write_sram(desc + 3, 0x8000 | (1<<10)); /* USB_COUNTn_RX */

	EPR_SET_ADDR(EPR, 0);
	EPR_SET_TYPE(EPR, 1);
	EPR_SET_STAT_TX(EPR, EP_STATE_NAK);
	EPR_SET_STAT_RX(EPR, EP_STATE_NAK);

	info->dev_state = INIT;
	info->dev_address = 0;

	USB.DADDR = 0x80;

	return 0;
}

static int usb_ctrl_recv(struct usb_endpoint *e)
{
	int ep = e->index;
	volatile uint16_t *desc = &USB_SRAM[4*ep];
	volatile unsigned int *EPR = &USB.EPR[ep];
	struct usb_ctrl_info *info = e->priv;
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
			info->dev_state = PREADDRESSED;
			info->dev_address = buf[2];
			usb_transmit(e, 0, 0);
			break;
		case GET_DESCRIPTOR:
			LOG('D');
			usb_transmit_descriptor(e, buf[2], buf[3]);
			break;
		case SET_CONFIGURATION:
			LOG('C');
			info->dev_state = CONFIGURED;
			usb_transmit(e, 0, 0);
			break;
		default:
			usb_set_transmit_state(e, EP_STATE_STALL);
			break;
		}
	} else {
		usb_set_transmit_state(e, EP_STATE_STALL);
	}

	return 0;
}

int usb_ctrl_xmit(struct usb_endpoint *e)
{
	struct usb_ctrl_info *info = e->priv;

	if (info->dev_state == PREADDRESSED) {
		USB.DADDR = info->dev_address | 0x80;
		info->dev_state = ADDRESSED;
	}
	EPR_CLEAR_CTR_TX(&USB.EPR[e->index]);
	EPR_SET_TOGGLE(&USB.EPR[e->index], USB_EPR_DTOG_RX, 0);
}

struct usb_endpoint_ops usb_ctrl_ops = {
	.init = usb_ctrl_init,
	.recv = usb_ctrl_recv,
	.xmit = usb_ctrl_xmit,
};
