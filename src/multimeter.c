#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "STM32F4xx.h"
#include "../header/LED.h"
#include "../header/SWT.h"
#include "../header/LCD.h"
#include "../header/menu.h"
#include "../header/board_ctrl.h"
#include "../header/multimeter.h"
#include "../header/BEEP.h"

#define STORECAPACITY		20
#define CALIBRATIONCONST    1.224594107
char*	units[5] = { " V"," mV"," A"," mA"," KOhm" };	//Unit array
int		unitIndex;										//Index to select unit from the array above
int		autoRangeIndex = 4;								//Preventing range being overwritten
char**	storedData;										//Data store array
int		memAddr;										//Data array index

/*Function to initialize data slot array*/
/*Using malloc to allocate memory as a prevention of memory leak*/
void dataStorageInitializer(void)
{
	int i;
	memAddr = 0;
	storedData = (char**)malloc(STORECAPACITY * sizeof(char*));
	for (i = 0; i < STORECAPACITY; i++)
	{
		storedData[i] = NULL;
	}
}

/*Function to read data storage to other code file*/
char **dataArrayRetrieve(void)
{
	return storedData;
}

/*Function to obtain current data slot selected*/
int returnMemAddr(void)
{
	return memAddr;
}

/*Function to set slot for next data storage*/
void setMemAddr(int newAddr)
{
	memAddr = newAddr;
}

/* Function to select unit from the unit arrar automatically 
   This function takes mode (1=voltage, 2=current, 3=resistance) 
   and range (index same with manual menu design) as parameters*/
void unitSelect(int mode, int range)
{
	switch (mode)
	{
		case 1: 
			if (range == 1 || range == 2 || range == 3) 
				unitIndex = 1;	//1,10,100mV range
			else if (range == 4) 
				unitIndex = 0;	//1V range
			break;

		case 2:
			if (range == 1 || range == 2 || range ==3)
				unitIndex = 3;	//1,10,100mA range
			else if (range == 4)
				unitIndex = 2;	//1A range
			break;
		case 3: 
			unitIndex = 4;		//kOhm range
	}
}

/* Function to activate visual and audio feedback when input out of ADC capacity */
void ERROR_MSG(void)
{
	LCD_Clear();
	LCD_GotoXY(0, 0);
	LCD_PutS("Out of range.");						// Visual feedback
	Delay(50);
	BEEP_Interval(80,200);							// Audio feedback
}

/*Function to return the coefficient as part of the calculation of voltage at the circuit input*/
double expMulti(int range)
{
	double tmpExp;
	if (range == 1 || range == 2 || range == 3)
		tmpExp = 1000;
	else
		tmpExp = 1;

	return tmpExp;
}

/* Function to return specific value based on user selection */
char *switchableRangeParser(int mode, int range)
{
	double Vadc	= 0;

	Vadc  =	read_ADC1()/CALIBRATIONCONST;		// Raw calibrated ADC value
	Vadc /= 1000;								// Adjust unit to volts, default it millivolts

	double	gradientConst = 1.5;
	double	resistanceConst[4] = {10,50,100,1000};

	double	rangeLimit[4] = {0.01,0.1,1,10};

	double	voltage = (Vadc/gradientConst - 1)*rangeLimit[range - 1] * expMulti(range);
	double	current = voltage;

	double	resistance = Vadc*(resistanceConst[range-1]/3);

	double	measuredValue[3] 	= { voltage, current, resistance };
	char*	parsingAccuracy[3] 	= { "%.0f","%.1f","%.2f" };
	int		accuMode = 0;

	char	*displayString = (char*)malloc(20 * sizeof(char));

	if ((mode == 1 || mode == 2) && range == 4)
		accuMode = 2;
	else if (mode == 3)
		accuMode = 1;

	sprintf(displayString, parsingAccuracy[accuMode], measuredValue[mode - 1]);

	return displayString;
}


char *autoRangingParser(int mode)
{
	rangeReturn(4); // For circuit and board safety

	double	Vadc = 0;
	char*	parsingAccuracy[2] = {"%.0f","%.2f"};
	int		accuMode = 0;

	Vadc = read_ADC1() / CALIBRATIONCONST;		// Raw calibrated ADC value
	Vadc /= 1000;

	double	gradientConst = 1.5;
	double	resistanceConst[4] = { 10,50,100,1000 };

	double	rangeLimit[4] = { 0.01,0.1,1,10 };

	double	voltage = (Vadc/gradientConst - 1)*rangeLimit[autoRangeIndex - 1]*1000;
	double	current = voltage;

	double	resistance = Vadc*(resistanceConst[autoRangeIndex - 1] / 3);	// Resistance value

	char	*displayString = (char*)malloc(20 * sizeof(char));

	switch (mode) {
		case 1:
			if (Vadc <= 0.003)
				autoRangeIndex = 1;
			else if (Vadc > 0.003 && Vadc <= 0.03)
				autoRangeIndex = 2;
			else if (Vadc > 0.03 && Vadc <= 0.3)
				autoRangeIndex = 3;
			else if (Vadc > 0.3)
			{
				autoRangeIndex = 4;
				voltage /= 1000;
				accuMode = 1;
			}
			break;
		case 2:
			if (Vadc <= 0.003)
				autoRangeIndex = 1;
			else if (Vadc > 0.003 && Vadc <= 0.03)
				autoRangeIndex = 2;
			else if (Vadc > 0.03 && Vadc <= 0.3)
				autoRangeIndex = 3;
			else if (Vadc > 0.3)
			{
				autoRangeIndex = 4;
				voltage /= 1000;
				accuMode = 1;
			}
			break;
		case 3:
			if (resistance <= 50)
				autoRangeIndex = 2;
			else if (resistance > 50 && resistance <= 100)
				autoRangeIndex = 3;
			else if (resistance > 100 && resistance < 1000)
				autoRangeIndex = 4;
			break;
	}

	double measuredValue[3] = { voltage, current, resistance };

	unitSelect(mode, autoRangeIndex);
	rangeReturn(autoRangeIndex);

	sprintf(displayString, parsingAccuracy[accuMode], measuredValue[mode-1]);

	return displayString;
}


void rangeReturn(int range)
{

	if ( range == 1 )
		rangeSignal(0, 0);
	else if (range == 2)
		rangeSignal(0, 1);
	else if (range == 3)
		rangeSignal(1, 0);
	else if (range == 4)
		rangeSignal(1, 1);
}

/* Function to identify whether user is attempting to return sub menu */
int returnSubMenu(void)
{
	int i;
	if (readButtonIndex() == 4)
	{
		Delay(25);
		i = 1;
	}
	else
		i = 0;
	
	return i;
}

/* Function to display parsed ADC value */
void ADC_display(int mode, int range, int measureMode)
{
	char *displayString = (char*)malloc(200 * sizeof(char));
	char *tmpString;

	do
	{
		free(displayString); //Preventing memory leak
		LCD_Clear();

		if (measureMode == 1) //Manual mode
		{
			displayString = switchableRangeParser(mode, range);
			unitSelect(mode,range);
			if (mode == 1 || mode == 3)
				rangeReturn(range);
			else if (mode == 2)
				rangeReturn(range+1);
		}
		else if (measureMode == 0) //Auto
			displayString = autoRangingParser(mode);

		LCD_GotoXY(0, 0);
		LCD_PutS(displayString);
		LCD_GotoXY(strlen(displayString), 0);
		LCD_PutS(units[unitIndex]);

		if (readButtonIndex() == 5 && memAddr <= 19)
		{
			tmpString = malloc(strlen(displayString) + 1); 
			strcpy(tmpString, displayString);		//Copy LCD content (numerical) to tmpString
			strcat(tmpString, units[unitIndex]);	//Copy unit to tmpString
			storedData[memAddr] = tmpString;		//Put tmpString to preallocated data slot
			LCD_GotoXY(0, 1);
			LCD_PutS("Data Stored");				//Warning info
			Delay(500);
			memAddr++;								//Ready to handle next data
		}

		/* Range is determined by ADC maximum capacity */
		while (abs(read_ADC1()/CALIBRATIONCONST) > 2450)
		{
			ERROR_MSG();
		}
	
		Delay(500);
		
	} while (returnSubMenu() != 1);

	LCD_Clear();
	mainMenu();
}
