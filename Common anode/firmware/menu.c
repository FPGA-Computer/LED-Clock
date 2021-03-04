/*
 * menu.c
 *
 * Created: November 8, 2019, 7:55:52 PM
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

uint8_t Set0,Set1,Set2,SetModified;

extern const UI_Menu_t TimeSetMenu;

const UI_Item_t Time_Setting[]=
{
	{&Set0,&SetModified,HR_COL,2,TIME_HR_MIN,TIME_HR_MAX,D_HR},
	{&Set1,&SetModified,MIN_COL,2,0,59,D_U8Z},
	{&Set2,&SetModified,SEC_COL,2,0,59,D_U8Z},
	{&TimeSetMenu,NULL,HR_COL,6,0,0,D_SetExit}
};

const UI_Menu_t TimeSetMenu =
{
	NULL,0,Time_Setting,sizeof(Time_Setting)/sizeof(UI_Item_t)
};

// date: WW.DD.MM
const UI_Item_t Date_Setting[]=
{
	{&Set0,&SetModified,S_DAY_COL,2,1,31,D_U8},
	{&Set1,&SetModified,S_MONTH_COL,2,1,12,D_U8Z},
	{&Set2,&SetModified,S_YEAR_COL,2,0,99,D_U8Z}
};

const UI_Menu_t DateSetMenu =
{
	NULL,0,Date_Setting,sizeof(Date_Setting)/sizeof(UI_Item_t)
};

void Set_Time(void)
{
	Set0 = time.hour;
	Set1 = time.min;
	Set2 = 0;						// clear seconds
	SetModified = 0;

	UI_Menu(&TimeSetMenu);
	
	if(SetModified)
		RTC_SetTime(Set0,Set1,Set2);
}

void Set_Date(void)
{
	Set0 = time.day;
	Set1 = time.month;
	Set2 = time.year%100;	// stripped
	SetModified = 0;

	UI_Menu(&DateSetMenu);
	
	if(SetModified)
		RTC_SetDate(Set0,Set1,Set2+CENTURY);
}

#ifdef ALARM
const UI_Item_t Alarm_Setting[]=
{
	{&time.Alarm_hour,&SetModified,HR_COL,2,TIME_HR_MIN,TIME_HR_MAX,D_HR},
	{&time.Alarm_min,&SetModified,MIN_COL,2,0,59,D_U8Z}
};

const UI_Menu_t AlarmSetMenu =
{
	NULL,0,Alarm_Setting,sizeof(Alarm_Setting)/sizeof(UI_Item_t)
};

void Set_Alarm(void)
{
	UI_Menu(&AlarmSetMenu);
}
#endif
