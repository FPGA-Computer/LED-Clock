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

static time_hms_t TimeSetting;
static uint8_t TimeModified;

extern const UI_Menu_t SettingMenu;

const UI_Item_t RTC_Setting[]=
{
	{&TimeSetting.hour,&TimeModified,HR_COL,2,TIME_HR_MIN,TIME_HR_MAX,D_HR},
	{&TimeSetting.min,&TimeModified,MIN_COL,2,0,59,D_U8Z},
	{&TimeSetting.sec,&TimeModified,SEC_COL,2,0,59,D_U8Z},
	{&SettingMenu,NULL,HR_COL,6,0,0,D_SetExit}
};

const UI_Menu_t SettingMenu =
{
	NULL,0,RTC_Setting,sizeof(RTC_Setting)/sizeof(UI_Item_t)
};

void Set_Time(void)
{
	uint16_t countdown;
	int8_t new_trim;
	
	TimeSetting.hour = time.hour;
	TimeSetting.min = time.min;
	TimeSetting.sec = 0;						// clear seconds
	TimeModified = 0;

	UI_Menu(&SettingMenu);
	
	if(TimeModified)
		SetTime(TimeSetting.hour,TimeSetting.min,TimeSetting.sec);
}
