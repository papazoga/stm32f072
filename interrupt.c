
void __attribute__((weak, alias("_default_handler"))) _Reset_handler();
void __attribute__((weak, alias("_default_handler"))) _NMI_handler();
void __attribute__((weak, alias("_default_handler"))) _HardFault_handler();
void __attribute__((weak, alias("_default_handler"))) _SVCall_handler();
void __attribute__((weak, alias("_default_handler"))) _PendSV_handler();
void __attribute__((weak, alias("_default_handler"))) _SysTick_handler();
void __attribute__((weak, alias("_default_handler"))) _WWDG_handler();
void __attribute__((weak, alias("_default_handler"))) _PVD_VDDIO2_handler();
void __attribute__((weak, alias("_default_handler"))) _RTC_handler();
void __attribute__((weak, alias("_default_handler"))) _FLASH_handler();
void __attribute__((weak, alias("_default_handler"))) _RCC_CRS_handler();
void __attribute__((weak, alias("_default_handler"))) _EXTI0_1_handler();
void __attribute__((weak, alias("_default_handler"))) _EXTI2_3_handler();
void __attribute__((weak, alias("_default_handler"))) _EXTI4_15_handler();
void __attribute__((weak, alias("_default_handler"))) _TSC_handler();
void __attribute__((weak, alias("_default_handler"))) _DMA_CH1_handler();
void __attribute__((weak, alias("_default_handler"))) _DMA_CH2_3_DMA2_CH1_2_handler();
void __attribute__((weak, alias("_default_handler"))) _DMA_CH4_5_6_7_DMA2_CH3_4_5_handler();
void __attribute__((weak, alias("_default_handler"))) _ADC_COMP_handler();
void __attribute__((weak, alias("_default_handler"))) _TIM1_BRK_UP_TRG_COM_handler();
void __attribute__((weak, alias("_default_handler"))) _TIM1_CC_handler();
void __attribute__((weak, alias("_default_handler"))) _TIM2_handler();
void __attribute__((weak, alias("_default_handler"))) _TIM3_handler();
void __attribute__((weak, alias("_default_handler"))) _TIM6_DAC_handler();
void __attribute__((weak, alias("_default_handler"))) _TIM7_handler();
void __attribute__((weak, alias("_default_handler"))) _TIM14_handler();
void __attribute__((weak, alias("_default_handler"))) _TIM15_handler();
void __attribute__((weak, alias("_default_handler"))) _TIM16_handler();
void __attribute__((weak, alias("_default_handler"))) _TIM17_handler();
void __attribute__((weak, alias("_default_handler"))) _I2C1_handler();
void __attribute__((weak, alias("_default_handler"))) _I2C2_handler();
void __attribute__((weak, alias("_default_handler"))) _SPI1_handler();
void __attribute__((weak, alias("_default_handler"))) _SPI2_handler();
void __attribute__((weak, alias("_default_handler"))) _USART1_handler();
void __attribute__((weak, alias("_default_handler"))) _USART2_handler();
void __attribute__((weak, alias("_default_handler"))) _USART3_4_5_6_7_8_handler();
void __attribute__((weak, alias("_default_handler"))) _CEC_CAN_handler();
void __attribute__((weak, alias("_default_handler"))) _USB_handler();

void __attribute__((interrupt("IRQ"))) _default_handler()
{
	for (;;);
}
