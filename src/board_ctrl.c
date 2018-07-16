	#include <stdio.h>
#include <math.h>
#include <string.h>
#include "STM32F4xx.h"
#include "../header/LED.h"
#include "../header/SWT.h"
#include "../header/LCD.h"
#include "../header/BEEP.h"
#include "../header/board_ctrl.h"

/* function to decode SWT_Get value to button index */
int readButtonIndex(void)
{
	return log2(SWT_Get()) - 7;
}


void ADC1_init(void) {
	/* Enable clocks */
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

	/* ADC12_IN14 is the channel we shall use. It is connected to
	* PC4 which is connected to the board edge connectors */
	GPIOC->MODER = 0x3 << (2 * 4);
	GPIOC->PUPDR = 0;

	/* Set ADC to discontinuous conversion mode. */
	ADC1->CR1 = 0x00;
	ADC1->CR1 |= ADC_CR1_DISCEN;


	/* Ensure CR2 is set to zero. This means data will be right aligned,
	* DMA is disabled and there are no extrnal triggers/injected channels */
	ADC1->CR2 = 0x00;

	/* Set to one conversion at a time, and set that first conversion
	* to come from channel 14 (connected to PC4) */
	ADC1->SQR1 &= ~ADC_SQR1_L;
	ADC1->SQR3 = 14 & ADC_SQR3_SQ1;

	/* Enable the ADC */
	ADC1->CR2 |= ADC_CR2_ADON;
	ADC1->CR2 |= (1 << 2);
}	

/* function to read ADC and retun value */
int read_ADC1(void) {
	/* set SWSTART to 1 to start conversion */
	ADC1->CR2 |= ADC_CR2_SWSTART;

	/* Wait until End Of Conversion bit goes high */
	while (!(ADC1->SR & ADC_SR_EOC));

	/* Return data value */
	return (ADC1->DR);
}

void autoRangeSignalInit(void)
{
	RCC->AHB1ENR |= ((1UL << 2));					/* Enable GPIOC clock                */

	GPIOC->MODER &= ~((3UL << 2 * 6) | (3UL << 2 * 7));
	GPIOC->MODER |= ((1UL << 2 * 6) | (1UL << 2 * 7));                /* PC6 & 7 is output                     */ 

	GPIOC->OSPEEDR &= ~((3UL << 2 * 6) | (3UL << 2 * 7));
	GPIOC->OSPEEDR |= ((2UL << 2 * 6) | (2UL << 2 * 7));              /* PC6 & 7 is 50MHz Fast Speed           */
    

	GPIOC->OTYPER &= ~((1UL << 6) | (1UL << 7));
	GPIOC->OTYPER |= ((0UL <<  6) | (0UL << 7));               /* PC6 & 7 is output Push-Pull           */

	GPIOC->PUPDR &= ~((3UL << 2 * 6) | (3UL << 2 * 7));
	GPIOC->PUPDR |= ((2UL << 2 * 6) |   (2UL << 2 * 7));                /* PC6 & 7 is Pull down                */

}


/* Output range signal for hardware circuit */
/* Part of autoranging system */
void rangeSignal(int a, int b)
{
	if (a == 0 && b == 0)
	{
		GPIOC->ODR &= 0;
		GPIOC->ODR = ((0UL << 6) | (0UL << 7));

		LED_Off(0);
		LED_Off(1);

	}
	else if (a == 0 && b == 1)
	{
		GPIOC->ODR &= 0;
		GPIOC->ODR = ((0UL << 6) | (1UL << 7));

		LED_Off(0);
		LED_On(1);
	}
	else if (a == 1 && b == 0)
	{
		GPIOC->ODR &= 0;
		GPIOC->ODR = ((1UL << 6) | (0UL << 7));

		LED_On(0);
		LED_Off(1);
	}
	else if (a == 1 && b == 1)
	{
		GPIOC->ODR &= 0;
		GPIOC->ODR = ((1UL << 6) | (1UL << 7));

		LED_On(0);
		LED_On(1);
	}
}

