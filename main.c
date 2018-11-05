#include "stm32f0x.h"

int led_state = 0;
int count = 0;

int main()
{
	RCC.AHBENR |= RCC_AHBENR_IOPBEN;
	RCC.APB2ENR |= RCC_APB2ENR_TIM1EN;

	RCC.CR2 |= RCC_CR2_HSI48ON;

	/* Wait for oscillator to become ready */
	while (!(RCC.CR2 & RCC_CR2_HSI48RDY));

	/* Switch to HSI48 oscillator */
	RCC.CFGR |= 3;

	GPIOB.MODER = 1;
	GPIOB.OSPEEDR = 3;

	TIM1.PSC = 1000;
	TIM1.ARR = 10000;
	TIM1.DIER |= 1;
	TIM1.CR1 |= 1;

	/* Enable the timer interrupt */
	NVIC_ICPR = (1<< _TIM1_BRK_UP_TRG_COM_IRQ);
	NVIC_ISER = (1<< _TIM1_BRK_UP_TRG_COM_IRQ);

	while (1);
}

void __attribute__((interrupt("IRQ"))) _TIM1_BRK_UP_TRG_COM_handler()
{
	TIM1.SR &= ~1;

	if (led_state) {
		GPIOB.ODR = 0;
		led_state = 0;
	} else {
		led_state = 1;
		GPIOB.ODR = 1;
	}

	count++;
}

