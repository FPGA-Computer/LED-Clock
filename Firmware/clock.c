/*
 * clock.c
 *
 * Created: November 1, 2019, 6:23:06 PM
 *  Author: K. C. Lee
 * Copyright (c) 2019 by K. C. Lee
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.

 If not, see http://www.gnu.org/licenses/gpl-3.0.en.html 
 */
 
#include "hardware.h"

uint8_t Clock_Mode, ModeRefresh;

void ClockApp(void)
{

	uint8_t key = Key_Get();
	
	ModeRefresh=0;
	
	switch(key)
	{
		case KEY_INC:
			if(Clock_Mode < Mode_Last-1)
			{ 
				Clock_Mode++;
				ModeRefresh=1;
			}
			else
				Clock_Mode = Mode_Clock;
			break;

		case KEY_DEC:
			if(Clock_Mode)
			{
			  Clock_Mode--;
				ModeRefresh=1;
			}
			else
				Clock_Mode = Mode_Last-1;
			break;
	}

	switch(Clock_Mode)
	{
		case Mode_Clock:
			if(key==(KEY_MENU|KEY_LONG))
				Set_Time();
				
			DisplayClock();
			break;
			
		case Mode_Calendar:
			if(key==(KEY_MENU|KEY_LONG))
				Set_Date();
				
			DisplayDate();
			break;
	}
}

void DisplayClock(void)
{
	if(ModeRefresh||(time_flag & TIME_SEC_FLAG))
	{
		ModeRefresh=0;	
		time_flag &= ~TIME_SEC_FLAG;
		UI_Print_Time(&time,DISPLAY_SEC);
	}
	
	// blink decimal point
	if(time_flag & TIME_HALF_SEC)
	{
		time_flag &= ~TIME_HALF_SEC;
		
		Display[HR_COL+1]&=~SEP_SEG;
		
		if(DISPLAY_SEC)
			Display[MIN_COL+1]&=~SEP_SEG;
	}
}

void DisplayDate(void)
{
	if(ModeRefresh||(time_flag & TIME_SEC_FLAG))
	{
		ModeRefresh=0;
		time_flag &= ~TIME_SEC_FLAG;
		
		UI_Print_Date(&time);

		Display[DOW_COL+1]|=SEP_SEG;
		Display[DAY_COL+1]|=SEP_SEG;
	}
}

		