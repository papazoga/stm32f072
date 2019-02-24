#include <stdint.h>

#include "stm32f0x.h"
#include "usb.h"

int led_state = 0;
int count = 0;

int main()
{
	RCC.AHBENR |= RCC_AHBENR_IOPAEN | RCC_AHBENR_IOPBEN;
	RCC.APB1ENR |= RCC_APB1ENR_TIM2EN | RCC_APB1ENR_USBEN;
	RCC.APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	/* Wait-state required if SYSCLK >= 24MHz */
	FLASH.ACR |= 1;

	/* Turn on HSI48 */
	RCC.CR2 |= RCC_CR2_HSI48ON;

	/* Wait for HSI48 to become ready */
	while (!(RCC.CR2 & RCC_CR2_HSI48RDY));

	/* Set PLL parameters  for 24MHz */
	RCC.CFGR |= RCC_CFGR_PLLSRC(1) | RCC_CFGR_PLLMUL(1);
	RCC.CR |= RCC_CR_PLLON;

	while (!(RCC.CR & RCC_CR_PLLRDY));

	/* Switch to PLL */
	RCC.CFGR |= RCC_CFGR_SW(1);

	/* Set up PA15 for TIM2 OC1 output */
	GPIOA.MODER |= 2 << 30;
	GPIOA.OSPEEDR |= 3 << 30;
	GPIOA.AFRH &= ~(0xf << 28);
	GPIOA.AFRH |= 2 << 28;

	GPIOB.MODER = 1;
	GPIOB.OSPEEDR = 3;

	/* Set up Timer 2 for simple PWM */
	TIM2.PSC = 47;
	TIM2.ARR = 50;
	TIM2.CCR1 = 40;
	TIM2.CCMR1 |= TIMx_CCMR1_OC1M(6) | TIMx_CCMR1_OC1PE;
	TIM2.CCER |= 1;
	TIM2.DIER |= 1;
	TIM2.CR1 |= 1;

	/* Enable the timer interrupt */
	NVIC_ICPR = (1 << _TIM2_IRQ) | (1 << _USB_IRQ);
	NVIC_ISER = (1 << _TIM2_IRQ) | (1 << _USB_IRQ);

	usb_init();

	while (1) {
		int i;
		for (i=0;i<50000;i++);
		if (led_state == 0) {
			TIM2.CCR1--;
			if (TIM2.CCR1 == 1)
				led_state = 1;
		} else {
			TIM2.CCR1++;
			if (TIM2.CCR1 == 30)
				led_state = 0;
		}
	}
}

void usb_tick(void);

void __attribute__((interrupt("IRQ"))) _TIM2_handler()
{
	TIM2.SR &= ~1;
	NVIC_ICPR = (1 << _TIM2_IRQ);

	count++;
	if ((count & 0xfff) == 0)
	    GPIOB.ODR ^= 1;

	if ((count & 0xff) == 0)
		usb_tick();
}
