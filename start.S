	.globl _reset
	.globl _Reset_handler
	.code 16
	.thumb

#define THUMB_ADDR(_x)   (_x+1)
	.section ".vectors"
_reset:
	.word _stack_top
	.word THUMB_ADDR(_Reset_handler)
	.word THUMB_ADDR(_NMI_handler)
	.word THUMB_ADDR(_HardFault_handler)
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word THUMB_ADDR(_SVCall_handler)
	.word 0
	.word 0
	.word THUMB_ADDR(_PendSV_handler)
	.word THUMB_ADDR(_SysTick_handler)

	.word THUMB_ADDR(_WWDG_handler)

	.word THUMB_ADDR(_PVD_VDDIO2_handler)
	.word THUMB_ADDR(_RTC_handler)
	.word THUMB_ADDR(_FLASH_handler)
	.word THUMB_ADDR(_RCC_CRS_handler)

	.word THUMB_ADDR(_EXTI0_1_handler)
	.word THUMB_ADDR(_EXTI2_3_handler)
	.word THUMB_ADDR(_EXTI4_15_handler)
	.word THUMB_ADDR(_TSC_handler)

	.word THUMB_ADDR(_DMA_CH1_handler)
	.word THUMB_ADDR(_DMA_CH2_3_DMA2_CH1_2_handler)
	.word THUMB_ADDR(_DMA_CH4_5_6_7_DMA2_CH3_4_5_handler)

	.word THUMB_ADDR(_ADC_COMP_handler)
	.word THUMB_ADDR(_TIM1_BRK_UP_TRG_COM_handler)
	.word THUMB_ADDR(_TIM1_CC_handler)

	.word THUMB_ADDR(_TIM2_handler)
	.word THUMB_ADDR(_TIM3_handler)
	.word THUMB_ADDR(_TIM6_DAC_handler)
	.word THUMB_ADDR(_TIM7_handler)

	.word THUMB_ADDR(_TIM14_handler)
	.word THUMB_ADDR(_TIM15_handler)
	.word THUMB_ADDR(_TIM16_handler)
	.word THUMB_ADDR(_TIM17_handler)

	.word THUMB_ADDR(_I2C1_handler)
	.word THUMB_ADDR(_I2C2_handler)
	.word THUMB_ADDR(_SPI1_handler)
	.word THUMB_ADDR(_SPI2_handler)

	.word THUMB_ADDR(_USART1_handler)
	.word THUMB_ADDR(_USART2_handler)
	.word THUMB_ADDR(_USART3_4_5_6_7_8_handler)
	.word THUMB_ADDR(_CEC_CAN_handler)

	.word THUMB_ADDR(_USB_handler)

	/* Override the reset handler */
_Reset_handler:
	/* Initialize the bss */
	ldr r1, =_bss_size
	cmp r1, #0
	beq bss_done

	ldr r0, =_bss_start
	sub r2, r2, r2
bss_loop:
	str r2, [r0]
	add r0, r0, #4
	sub r1, r1, #4
	bne bss_loop

	/* Copy the data section */
bss_done:
	ldr r0, =_data_src
	ldr r1, =_data_dst
	ldr r2, =_data_size

	cmp r2, #0
	beq data_done

data_loop:
	ldr r3, [r0]
	str r3, [r1]
	add r0, r0, #4
	add r1, r1, #4
	sub r2, r2, #4
	bne data_loop

data_done:
	bl main

