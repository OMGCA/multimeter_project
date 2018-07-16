#include <stdio.h>
#include <math.h>
#include <string.h>
#include "STM32F4xx.h"
#include "../header/LED.h"
#include "../header/SWT.h"
#include "../header/LCD.h"
#include "../header/menu.h"
#include "../header/board_ctrl.h"
#include "../header/multimeter.h"

int mainMenu(void)
{
	char* mainMenu[6];
	/* Clear LCD */
	LCD_Clear();
	LCD_GotoXY(0, 0);
	int optionIndex = 0;

	/* Initialize main menu options */
	mainMenu[0] = "     Measure  ->";
	mainMenu[1] = "      AUTO      ";
	mainMenu[2] = "<-   Measure  ->";
	mainMenu[3] = "     MANUAL     ";
	mainMenu[4] = "<-  Retrieve    ";
	mainMenu[5] = " ";
	do {
		LCD_GotoXY(0, 0);
		LCD_PutS(mainMenu[optionIndex]);
		LCD_GotoXY(0, 1);
		LCD_PutS(mainMenu[optionIndex + 1]);
		if (readButtonIndex() == 2 && optionIndex < 4)
		{
			Delay(25);
			LCD_Clear();
			optionIndex += 2;
		}
		else if (readButtonIndex() == 1 && optionIndex > 0)
		{
			Delay(25);
			LCD_Clear();
			optionIndex -= 2;
		}
	} while (readButtonIndex() != 3);

	if (optionIndex == 0)
		measurementSelection(optionIndex);
	else if (optionIndex == 2)
		manualMeasurement();
	else if (optionIndex == 4)
		accessStoredData();
	return optionIndex;
}


/* Manual measurement menu design */
void manualMeasurement(void)
{
	char* selectionMenu[3];

	LCD_Clear();
	LCD_GotoXY(0, 0);
	int optionIndex = 0;
	int rangeIndex = optionIndex * 4;
	selectionMenu[0] = "     Voltage  ->";
	selectionMenu[1] = "<--  Current  ->";
	selectionMenu[2] = "<-- Resistance  ";

	char* selectionRange[12];
	selectionRange[0] = "     10 mV    ->";
	selectionRange[1] = "<--  100 mV   ->";
	selectionRange[2] = "<--   1 V     ->";
	selectionRange[3] = "<--   10 V      ";

	selectionRange[4] = "     10 mA    ->";
	selectionRange[5] = "<--  100 mA   ->";
	selectionRange[6] = "<--   1 A       ";
	selectionRange[7] = "<--   3 A       ";

	selectionRange[8] = "    10 kOhm   ->";
	selectionRange[9] = "    50 kOhm   ->";
	selectionRange[10] = "<--  100 kOhm ->";
	selectionRange[11] = "<--  1 MOhm     ";

	do {
		LCD_GotoXY(0, 0);
		LCD_PutS(selectionMenu[optionIndex]);
		LCD_GotoXY(0, 1);
		LCD_PutS(selectionRange[rangeIndex]);
		if (readButtonIndex() == 2 && optionIndex < 2)
		{
			Delay(25);
			LCD_Clear();
			optionIndex++;
			
			if ( optionIndex == 2 )
				rangeIndex = optionIndex * 4 + 1;
			else
				rangeIndex = optionIndex * 4;
		}
		else if (readButtonIndex() == 1 && optionIndex > 0)
		{
			Delay(25);
			LCD_Clear();
			optionIndex--;
			rangeIndex = optionIndex * 4;
		}
		else if (readButtonIndex() == 8 && rangeIndex < optionIndex * 4 + 3 && rangeIndex != 6)
		{
			Delay(25);
			LCD_Clear();
			rangeIndex++;
		}
		else if (readButtonIndex() == 7 && rangeIndex > optionIndex * 4 && rangeIndex != 9)
		{
			Delay(25);
			LCD_Clear();
			rangeIndex--;
		}

	} while (readButtonIndex() != 3 && readButtonIndex() != 4);
	if (readButtonIndex() == 4)
		mainMenu();
	else if (readButtonIndex() == 3)
		ADC_display(optionIndex + 1, rangeIndex + 1 - optionIndex * 4,1);
}

/* Auto measurement menu design */
void measurementSelection(int rangeOption)
{
	char* selectionMenu[3];

	LCD_Clear();
	LCD_GotoXY(0, 0);
	int optionIndex = 0;
	selectionMenu[0] = "     Voltage  ->";
	selectionMenu[1] = "<--  Current  ->";
	selectionMenu[2] = "<-- Resistance  ";

	do {
		LCD_GotoXY(0, 0);
		LCD_PutS(selectionMenu[optionIndex]);
		if (readButtonIndex() == 2 && optionIndex < 2)
		{
			Delay(25);
			LCD_Clear();
			optionIndex++;
		}
		else if (readButtonIndex() == 1 && optionIndex > 0)
		{
			Delay(25);
			LCD_Clear();
			optionIndex--;
		}
	} while (readButtonIndex() != 3 && readButtonIndex() != 4);

	if (readButtonIndex() == 4)
		mainMenu();
	else if (readButtonIndex() == 3)
		ADC_display(optionIndex + 1,0,0);
}

void displayDataSlot(char** memData, int memCounter, char* memAddrStr, char* slotIndex)
{
	LCD_Clear();
	LCD_GotoXY(0, 0);
	LCD_PutS(memData[memCounter]);
	/* Display slot number */
	sprintf(memAddrStr, "%d", memCounter + 1);
	strcat(slotIndex, memAddrStr);
	LCD_GotoXY(0, 1);
	LCD_PutS(slotIndex);
}

void accessStoredData(void)
{
	char**	memData = dataArrayRetrieve();
	int		memAddrCounter = 0;
	char	memAddrStr[2];
	char	slotIndex[7];

	/* Clear LCD */
	LCD_Clear();
	LCD_GotoXY(0, 0);

	do {
		Delay(300);
		LCD_Clear();
		LCD_GotoXY(0, 0);
		strcpy(slotIndex, "Slot ");

		do
		{
			if (memData[memAddrCounter] == NULL)
				LCD_PutS("No Data Stored");
			else
				LCD_PutS(memData[memAddrCounter]);

			/* Display slot number */
			sprintf(memAddrStr, "%d", memAddrCounter + 1);
			strcat(slotIndex, memAddrStr);
			LCD_GotoXY(0, 1);
			LCD_PutS(slotIndex);

			if (readButtonIndex() == 2 && memAddrCounter <= 19) // Button 2: next data
			{
				displayDataSlot(memData, memAddrCounter, memAddrStr, slotIndex);
				memAddrCounter++;
			}
			else if (readButtonIndex() == 1 && memAddrCounter >= 1) // Button 1: previous data
			{
				displayDataSlot(memData, memAddrCounter, memAddrStr, slotIndex);
				memAddrCounter--;
			}
		} while (readButtonIndex() == 0);
			
		if (readButtonIndex() == 6 && memData[memAddrCounter] != NULL) //Delete data from current slot
		{
			LCD_Clear();
			LCD_GotoXY(0, 0);
			LCD_PutS("Delete data?");
			do {
				if (readButtonIndex() == 3) //Button 3: confirm
				{
					memData[memAddrCounter] = NULL;
					setMemAddr(memAddrCounter);
					if(memAddrCounter != 0)
						memAddrCounter--;
				}
				else if (readButtonIndex() == 5) //Button 5: discard
					LCD_Clear();
			} while (readButtonIndex() != 3 && readButtonIndex() != 5);
		}

	} while (readButtonIndex() != 4);
	if (readButtonIndex() == 4)
		mainMenu();
}


