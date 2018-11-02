#include "stm32f0x.h"

int main()
{
	RCC.AHBENR |= (1<<18);

	GPIOB.MODER = 1;
	GPIOB.OSPEEDR = 3;

	while (1) {
		int i;

		GPIOB.ODR = 1;

		for (i=0;i<100000;i++);

		GPIOB.ODR = 0;
		for (i=0;i<100000;i++);
	}
}
