#include <stdio.h>
#include <math.h>
#include <string.h>
#include "../header/BEEP.h"
#include "stm32f4xx.h"
#include "../header/LED.h"
#include "../header/SWT.h"
#include "../header/LCD.h"



/*----------------------------------------------------------------------------
  initialize BEEP Pin
 *----------------------------------------------------------------------------*/
void BEEP_Init (void)
{
	RCC->AHB1ENR |= ((1UL << 2) );                  /* Enable GPIOC clock                */
	
	GPIOC->MODER &= ~((3UL << 2*9 ));
	GPIOC->MODER |= ((1UL << 2*9 ));                /* PC9 is output                     */
	
	GPIOC->OSPEEDR &= ~((3UL << 2*9 ));
	GPIOC->OSPEEDR |= ((2UL << 2*9 ));              /* PC9 is 50MHz Fast Speed           */
	
	GPIOC->OTYPER &= ~((1UL << 9 ));
	GPIOC->OTYPER |= ((0UL << 2*9 ));               /* PC9 is output Push-Pull           */
	
	GPIOC->PUPDR &= ~((3UL << 2*9 ));
	GPIOC->PUPDR |= ((2UL << 2*9 ));                /* PC9 is Pull down                  */
	
}

void BEEP_On (void)
{
	GPIOC->ODR |= ((1UL << 9 ));
}


void BEEP_Off (void)
{
	GPIOC->ODR = ((0UL << 9));
}

void BEEP_Interval (int duration, int interval)
{
	BEEP_On();
	Delay(duration);
	BEEP_Off();
	Delay(interval);
}

