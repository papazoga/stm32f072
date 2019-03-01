#include <stdint.h>
#include "stm32f0x.h"
#include "usb.h"

static int usb_intr_recv(struct usb_endpoint *);

static int usb_intr_init(struct usb_endpoint *e)
{
	volatile unsigned int *EPR = &USB.EPR[e->index];
	volatile uint16_t *desc = &USB_SRAM[4*e->index];
	struct usb_intr_info *info = e->priv;

	_write_sram(desc + 0, e->tx_offset);	 /* USB_ADDRn_TX */
	_write_sram(desc + 1, 0);		 /* USB_COUNTn_TX */
	_write_sram(desc + 2, e->rx_offset);	 /* USB_ADDRn_RX */
	_write_sram(desc + 3, 0x8000 | (1<<10)); /* USB_COUNTn_RX */

	EPR_SET_ADDR(EPR, 0);
	EPR_SET_TYPE(EPR, 3);

	EPR_SET_STAT_TX(EPR, EP_STATE_NAK);
	EPR_SET_STAT_RX(EPR, EP_STATE_VALID);

	return 0;
}

static int usb_intr_recv(struct usb_endpoint *e)
{
	int ep = e->index;
	volatile uint16_t *desc = &USB_SRAM[4*ep];
	volatile unsigned int *EPR = &USB.EPR[ep];
	struct usb_intr_info *info = e->priv;
	uint16_t addr_rx;
	uint16_t count_rx;

	addr_rx = _read_sram(desc + 2);
	count_rx = _read_sram(desc + 3) & 0x3ff;

	BREAK();

	return 0;
}

int usb_intr_xmit(struct usb_endpoint *e)
{
	struct usb_intr_info *info = e->priv;

	EPR_CLEAR_CTR_TX(&USB.EPR[e->index]);
	EPR_SET_TOGGLE(&USB.EPR[e->index], USB_EPR_DTOG_RX, 0);
}

struct usb_endpoint_ops usb_intr_ops = {
	.init = usb_intr_init,
	.recv = usb_intr_recv,
	.xmit = usb_intr_xmit,
};

