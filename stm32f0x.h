#ifndef STM32F0X_H
#define STM32F0X_H

#include <stdint.h>

struct flash_regs {
	unsigned int ACR;
	unsigned int KEYR;
	unsigned int OPTKEYR;
	unsigned int SR;
	unsigned int CR;
	unsigned int AR;
	unsigned int OBR;
	unsigned int WRPR;
};

struct gpio_regs {
	unsigned int MODER;
	unsigned int OTYPER;
	unsigned int OSPEEDR;
	unsigned int PUPDR;
	unsigned int IDR;
	unsigned int ODR;
	unsigned int BSRR;
	unsigned int LCKR;
	unsigned int AFRL;
	unsigned int AFRH;
	unsigned int BRR;
};

struct rcc_regs {
	unsigned int CR;
	unsigned int CFGR;
	unsigned int CIR;
	unsigned int APB2RSTR;
	unsigned int APB1RSTR;
	unsigned int AHBENR;
	unsigned int APB2ENR;
	unsigned int APB1ENR;
	unsigned int BDCR;
	unsigned int CSR;
	unsigned int AHBRSTR;
	unsigned int CFGR2;
	unsigned int CFGR3;
	unsigned int CR2;
};

struct advanced_control_timer_regs {
	unsigned int CR1;
	unsigned int CR2;
	unsigned int SMCR;
	unsigned int DIER;
	unsigned int SR;
	unsigned int EGR;
	unsigned int CCMR1;
	unsigned int CCMR2;
	unsigned int CCER;
	unsigned int CNT;
	unsigned int PSC;
	unsigned int ARR;
	unsigned int RCR;
	unsigned int CCR1;
	unsigned int CCR2;
	unsigned int CCR3;
	unsigned int CCR4;
	unsigned int BDTR;
	unsigned int DCR;
	unsigned int DMAR;
};

struct general_purpose_timer_regs {
	unsigned int CR1;
	unsigned int CR2;
	unsigned int SMCR;
	unsigned int DIER;
	unsigned int SR;
	unsigned int EGR;
	unsigned int CCMR1;
	unsigned int CCMR2;
	unsigned int CCER;
	unsigned int CNT;
	unsigned int PSC;
	unsigned int ARR;
	unsigned int _res1;
	unsigned int CCR1;
	unsigned int CCR2;
	unsigned int CCR3;
	unsigned int CCR4;
	unsigned int _res2;
	unsigned int DCR;
	unsigned int DMAR;
};

#define TIMx_CCMR1_CC1S(_x) ((_x)<<0)
#define TIMx_CCMR1_CC2S(_x) ((_x)<<8)

/* Input capture */
#define TIMx_CCMR1_IC1PSC(_x) ((_x)<<2)
#define TIMx_CCMR1_IC1F(_x) ((_x)<<4)

#define TIMx_CCMR1_IC2PSC(_x) ((_x)<<10)
#define TIMx_CCMR1_IC2F(_x) ((_x)<<12)

/* Output capture */
#define TIMx_CCMR1_OC1FE (1<<2)
#define TIMx_CCMR1_OC1PE (1<<3)
#define TIMx_CCMR1_OC1M(_x) ((_x)<<4)
#define TIMx_CCMR1_OC1CE (1<<7)

#define TIMx_CCMR1_OC2FE (1<<10)
#define TIMx_CCMR1_OC2PE (1<<11)
#define TIMx_CCMR1_OC2M(_x) ((_x)<<12)
#define TIMx_CCMR1_OC2CE (1<<15)

struct usb_regs {
	unsigned int EPR[8];
	unsigned int _reserved[8];
	unsigned int CNTR;
	unsigned int ISTR;
	unsigned int FNR;
	unsigned int DADDR;
	unsigned int BTABLE;
	unsigned int LPMCSR;
	unsigned int BCDR;
};

#define RCC_CFGR_SW(_x) ((_x) << 0)
#define RCC_CFGR_SWS(_x) ((_x) << 2)
#define RCC_CFGR_HPRE(_x) ((_x) << 4)
#define RCC_CFGR_PPRE(_x) ((_x) << 8)
#define RCC_CFGR_ADCPRE(_x) ((_x << 14)
#define RCC_CFGR_PLLSRC(_x) ((_x) << 15)
#define RCC_CFGR_PLLXTPRE (1<<17)
#define RCC_CFGR_PLLMUL(_x) ((_x) << 18)
#define RCC_CFGR_MCO(_x) ((_x) ((_x) <<  24)
#define RCC_CFGR_MCOPRE(_x) ((_x) << 28)
#define RCC_CFGR_PLLNODIV 31

#define RCC_CR_PLLRDY (1<<25)
#define RCC_CR_PLLON (1<<24)
#define RCC_CR_CSSON (1<<19)
#define RCC_CR_HSEBYP (1<<18)
#define RCC_CR_HSERDY (1<<17)
#define RCC_CR_HSEON (1<<16)
#define RCC_CR_HSICAL(_x) ((_x) << 24)
#define RCC_CR_HSITRIM(_x) ((_x) <<3)
#define RCC_CR_HSIRDY (1<<1)
#define RCC_CR_HSION (1<<0)

#define RCC_CFGR2_PREDIV_OFFSET 0

#define RCC_AHBENR_DMAEN (1<<0)
#define RCC_AHBENR_DMA2EN_OFFSET 1
#define RCC_AHBENR_SRAMEN (1<<2)
#define RCC_AHBENR_FLITFEN (1<<4)
#define RCC_AHBENR_CRCEN (1<<6)
#define RCC_AHBENR_IOPAEN (1<<17)
#define RCC_AHBENR_IOPBEN (1<<18)
#define RCC_AHBENR_IOPCEN (1<<19)
#define RCC_AHBENR_IOPDEN (1<<20)
#define RCC_AHBENR_IOPEEN (1<<21)
#define RCC_AHBENR_IOPFEN (1<<22)
#define RCC_AHBENR_IOPGEN (1<<23)

#define RCC_APB2ENR_SYSCFGEN (1<<0)
#define RCC_APB2ENR_TIM1EN (1<<11)

#define RCC_APB1ENR_TIM2EN (1<<0)
#define RCC_APB1ENR_TIM3EN (1<<1)
#define RCC_APB1ENR_TIM6EN (1<<4)
#define RCC_APB1ENR_TIM7EN (1<<5)
#define RCC_APB1ENR_TIM14EN (1<<8)
#define RCC_APB1ENR_WWDGEN (1<<11)
#define RCC_APB1ENR_SPI2EN (1<<14)
#define RCC_APB1ENR_USART2EN (1<<17)
#define RCC_APB1ENR_USART3EN (1<<18)
#define RCC_APB1ENR_USART4EN (1<<19)
#define RCC_APB1ENR_USART5EN (1<<20)
#define RCC_APB1ENR_I2C1EN (1<<21)
#define RCC_APB1ENR_I2C2EN (1<<22)
#define RCC_APB1ENR_USBEN (1<<23)
#define RCC_APB1ENR_CANEN (1<<25)
#define RCC_APB1ENR_CRSEN (1<<27)
#define RCC_APB1ENR_PWREN (1<<28)
#define RCC_APB1ENR_DACEN (1<<29)
#define RCC_APB1ENR_CECEN (1<<30)

#define RCC_CR2_HSI48ON (1<<16)
#define RCC_CR2_HSI48RDY (1<<17)

#define USB_DADDR_EF (1<<7)

#define USB_EPR_EA(_x) ((_x) << 0)
#define USB_EPR_STAT_TX(_x) ((_x) << 4)
#define USB_EPR_DTOG_TX (1<<6)
#define USB_EPR_CTR_TX (1<<7)
#define USB_EPR_EP_KIND (1<<8)
#define USB_EPR_EP_TYPE(_x) ((_x)<<9)
#define USB_EPR_SETUP (1<<11)
#define USB_EPR_STAT_RX(_x) ((_x) << 12)
#define USB_EPR_DTOG_RX (1<<14)
#define USB_EPR_CTR_RX (1<<15)

#define USB_CNTR_FRES (1<<0)
#define USB_CNTR_PDWN (1<<1)
#define USB_CNTR_LPMODE (1<<2)
#define USB_CNTR_FSUSP (1<<3)
#define USB_CNTR_RESUME (1<<4)
#define USB_CNTR_L1RESUME (1<<5)
#define USB_CNTR_L1REQM (1<<7)
#define USB_CNTR_ESOFM (1<<8)
#define USB_CNTR_SOFM (1<<9)
#define USB_CNTR_RESETM (1<<10)
#define USB_CNTR_SUSPM (1<<11)
#define USB_CNTR_WKUPM (1<<12)
#define USB_CNTR_ERRM (1<<13)
#define USB_CNTR_PMAOVRM (1<<14)
#define USB_CNTR_CTRM (1<<15)

#define USB_ISTR_L1REQ (1<<7)
#define USB_ISTR_ESOF (1<<8)
#define USB_ISTR_SOF (1<<9)
#define USB_ISTR_RESET (1<<10)
#define USB_ISTR_SUSP (1<<11)
#define USB_ISTR_WKUP (1<<12)
#define USB_ISTR_ERR (1<<13)
#define USB_ISTR_PMAOVR (1<<14)
#define USB_ISTR_CTR (1<<15)
#define USB_ISTR_DIR (1<<4)
#define EP_ID(_x) ((_x) & 0xf)

/* The endpoint register has a complicated toggle scheme */
#define EP_W0_MASK 0x8080
#define EP_TOGGLE_MASK 0x7070

#define EP_STATE_DISABLED 0
#define EP_STATE_STALL 1
#define EP_STATE_NAK 2
#define EP_STATE_VALID 3

#define EPR_WMR_PART(_regp) \
	((*(_regp)) & ~(EP_W0_MASK | EP_TOGGLE_MASK))

#define EPR_CLEAR_CTR_RX(_regp) \
	*(_regp) = ( EPR_WMR_PART(_regp) | USB_EPR_CTR_TX )

#define EPR_CLEAR_CTR_TX(_regp) \
	*(_regp) = ( EPR_WMR_PART(_regp) | USB_EPR_CTR_RX )

#define _epr_extract_stat_rx(_regp) \
	( ((*(_regp)) >> 12) & 3 )

#define _epr_extract_stat_tx(_regp) \
	( ((*(_regp)) >> 4) & 3 )

#define EPR_SET_STAT_RX(_regp, _val)					\
	*(_regp) = ( EPR_WMR_PART(_regp) | \
		     USB_EPR_STAT_RX(_epr_extract_stat_rx(_regp) ^ _val) | \
		     EP_W0_MASK )

#define EPR_SET_STAT_TX(_regp, _val)					\
	*(_regp) = ( EPR_WMR_PART(_regp) | \
		     USB_EPR_STAT_TX(_epr_extract_stat_tx(_regp) ^ _val) | \
		     EP_W0_MASK )

#define EPR_SET_TYPE(_regp, _val) \
	*(_regp) |= ( USB_EPR_EP_TYPE(_val) | EP_W0_MASK )

#define EPR_SET_ADDR(_regp, _val) \
	*(_regp) |= ( _val | EP_W0_MASK )

#define EPR_SET_TOGGLE(_regp, _mask)				\
	*(_regp) = ( ((*_regp) & (_mask)) ^ (_mask) )

#define EPR_CLEAR_TOGGLE(_regp, _mask)				\
	*(_regp) = ( ((*_regp) & (_mask)) )

extern volatile struct flash_regs FLASH;

extern volatile struct gpio_regs GPIOA;
extern volatile struct gpio_regs GPIOB;
extern volatile struct gpio_regs GPIOC;
extern volatile struct gpio_regs GPIOD;
extern volatile struct gpio_regs GPIOE;
extern volatile struct gpio_regs GPIOF;

extern volatile struct advanced_control_timer_regs TIM1;
extern volatile struct general_purpose_timer_regs TIM2;

extern volatile struct rcc_regs RCC;

extern volatile unsigned int NVIC_ISER;
extern volatile unsigned int NVIC_ISPR;
extern volatile unsigned int NVIC_ICPR;
extern volatile unsigned int NVIC_IPR[8];

extern volatile struct usb_regs USB;
extern volatile uint16_t USB_SRAM[0x400];

/* Interrupts */
#define _WWDG_IRQ 0
#define _PVD_VDDIO2_IRQ 1
#define _RTC_IRQ 2
#define _FLASH_IRQ 3
#define _RCC_CRS_IRQ 4
#define _EXTI0_1_IRQ 5
#define _EXTI2_3_IRQ 6
#define _EXTI4_15_IRQ 7
#define _TSC_IRQ 8
#define _DMA_CH1_IRQ 9
#define _DMA_CH2_3_DMA2_CH_1_2_IRQ 10
#define _DMA_CH4_5_6_7_DMA2_CH3_4_5_IRQ 11
#define _ADC_COMP_IRQ 12
#define _TIM1_BRK_UP_TRG_COM_IRQ 13
#define _TIM1_CC_IRQ 14
#define _TIM2_IRQ 15
#define _TIM3_IRQ 16
#define _TIM6_DAC_IRQ 17
#define _TIM7_IRQ 18
#define _TIM14_IRQ 19
#define _TIM15_IRQ 20
#define _TIM16_IRQ 21
#define _TIM17_IRQ 22
#define _I2C1_IRQ 23
#define _I2C2_IRQ 24
#define _SPI1_IRQ 25
#define _SPI2_IRQ 26
#define _USART1_IRQ 27
#define _USART2_IRQ 28
#define _USART3_4_5_6_7_8_IRQ 29
#define _CEC_CAN_IRQ 30
#define _USB_IRQ 31

#endif
