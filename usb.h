#ifndef _USB_H
#define _USB_H

#define BREAK() 		__asm__("bkpt 0")

struct usb_endpoint {
	int index;
	int tx_offset;
	int tx_max;
	int rx_offset;
	int rx_max;
	struct usb_endpoint_ops *ops;
	void *priv;
};

struct usb_endpoint_ops {
	int (*init)(struct usb_endpoint *);
	int (*recv)(struct usb_endpoint *);
	int (*xmit)(struct usb_endpoint *);
};

extern struct usb_endpoint_ops usb_ctrl_ops;
extern struct usb_endpoint_ops usb_intr_ops;

#define SRAM_ADDR_TO_PTR(_addr)   (&USB_SRAM[_addr/2])

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

struct string_descriptor {
	int length;
	const char *string;
};

#define STRING_DESCRIPTOR(_str) \
	{ sizeof(_str)-1, _str }

struct usb_ctrl_info {
	const unsigned char *device_desc;
	const unsigned char *configuration_desc;
	struct string_descriptor *string_desc_table;
	enum usb_device_state {
		INIT = 0,
		PREADDRESSED,
		ADDRESSED,
		CONFIGURED
	} dev_state;
	unsigned char dev_address;
};

struct usb_intr_info {
	int s;
};

void usb_init();
void usb_transmit(struct usb_endpoint *e, const unsigned char *data, int len);
void usb_receive(struct usb_endpoint *e);
void usb_set_transmit_state(struct usb_endpoint *e, unsigned char state);

#ifdef DEBUG

extern unsigned char binary_log[BLOGLEN];
extern unsigned char binary_log_index;

#define BLOGLEN 128
#define LOG(_x)   if (logindex < BLOGLEN) { blog[logindex++] = (_x); } else BREAK();

#else

#define LOG(_x)

#endif /* DEBUG */


/*
 * The interface to the SRAM used for buffers and descriptors
 * requires that all memory accesses be word-width and word-aligned.
 */
static inline uint16_t _read_sram(const volatile void *src)
{
	uint16_t val;
	__asm__("ldrh %0, [%1]" : "=r" (val) : "r" (src));
	return val;
}

static inline void _write_sram(volatile void *dst, uint16_t val)
{
	__asm__("strh %0, [%1]" : : "r" (val), "r" (dst));
}

static inline void _copy_from_sram(void *dst, const volatile void *src, int len)
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

static inline void _copy_to_sram(volatile void *dst, const void *src, int len)
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

#endif
