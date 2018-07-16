/*----------------------------------------------------------------------------
 * Name:    display.c
 * Purpose: Calculate and display ADC value based on user choice
 * Note(s): 17/02/18: Multiple ranges for Voltage measurement, ADC value recalibrated
 *			08/03/18: Auto range implemented
 *			15/03/18: Unit will change with user's selection, not hard coded.
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2011 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/
 
 /* Developed by Design & Construction Lab Group 9 */
 /* Software group, Tian XIA, Kevin XU and Zhou XU */
 
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "STM32F4xx.h"
#include "../header/LED.h"
#include "../header/SWT.h"
#include "../header/LCD.h"
#include "../header/BEEP.h"
#include "../header/menu.h"
#include "../header/board_ctrl.h"
#include "../header/multimeter.h"
#include "sys.h"
#include "usart.h"

volatile uint32_t msTicks;                      /* counts 1ms timeTicks       */
/*----------------------------------------------------------------------------
  SysTick_Handler
 *----------------------------------------------------------------------------*/
void SysTick_Handler(void) {
  msTicks++;
}


/*----------------------------------------------------------------------------
  delays number of tick Systicks (happens every 1 ms)
 *----------------------------------------------------------------------------*/
void Delay (uint32_t dlyTicks) {                                              
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks);
}


/*----------------------------------------------------------------------------
  Function that initializes Button pins
 *----------------------------------------------------------------------------*/
void BTN_Init(void) {

  RCC->AHB1ENR  |= ((1UL <<  0) );              /* Enable GPIOA clock         */

  GPIOA->MODER    &= ~((3UL << 2*0)  );         /* PA.0 is input              */
  GPIOA->OSPEEDR  &= ~((3UL << 2*0)  );         /* PA.0 is 50MHz Fast Speed   */
  GPIOA->OSPEEDR  |=  ((2UL << 2*0)  ); 
  GPIOA->PUPDR    &= ~((3UL << 2*0)  );         /* PA.0 is no Pull up         */
}

/*----------------------------------------------------------------------------
  Function that read Button pins
 *----------------------------------------------------------------------------*/
 uint32_t BTN_Get(void) {

 return (GPIOA->IDR & (1UL<<0));
}

void initializationSet (void)
{
	ADC1_init();
	BEEP_Init(); 
	LED_Init();
	BTN_Init();   
	SWT_Init();	
	LCD_Initpins();	
	LCD_DriverOn();
	Delay(10);
	LCD_Init();

	LCD_DriverOn();
	LCD_On(1);
	Delay(20);
	LCD_Clear();
	autoRangeSignalInit();
	dataStorageInitializer();
  uart_init(84,115200);
}

/*----------------------------------------------------------------------------
  MAIN function
 *---------------
-------------------------------------------------------------*/
int main (void) {
  SystemCoreClockUpdate();                          /* Get Core Clock Frequency   */
  if (SysTick_Config(SystemCoreClock / 1000)) {     /* SysTick 1 msec interrupts  */
    while (1);                                      /* Capture error              */
  }

  initializationSet();

  printf("test"); //Serial port test
  mainMenu();
}

