#ifndef STM32F0X_H
#define STM32F0X_H

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

#define RCC_CFGR_SW_OFFSET 0
#define RCC_CFGR_SWS_OFFSET 2
#define RCC_CFGR_HPRE_OFFSET 4
#define RCC_CFGR_PPRE_OFFSET 8
#define RCC_CFGR_ADCPRE_OFFSET 14
#define RCC_CFGR_PLLSRC_OFFSET 15
#define RCC_CFGR_PLLXTPRE_OFFSET 17
#define RCC_CFGR_PLLMUL_OFFSET 18
#define RCC_CFGR_MCO_OFFSET 24
#define RCC_CFGR_MCOPRE_OFFSET 28
#define RCC_CFGR_PLLNODIV_OFFSET 31

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

#define RCC_APB2ENR_TIM1EN (1<<11)

#define RCC_CR2_HSI48ON (1<<16)
#define RCC_CR2_HSI48RDY (1<<17)

extern volatile struct gpio_regs GPIOA;
extern volatile struct gpio_regs GPIOB;
extern volatile struct gpio_regs GPIOC;
extern volatile struct gpio_regs GPIOD;
extern volatile struct gpio_regs GPIOE;
extern volatile struct gpio_regs GPIOF;

extern volatile struct advanced_control_timer_regs TIM1;

extern volatile struct rcc_regs RCC;

extern unsigned int NVIC_ISER;
extern unsigned int NVIC_ISPR;
extern unsigned int NVIC_ICPR;
extern unsigned int NVIC_IPR[8];

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
