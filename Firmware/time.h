/*
 * time.h
 *
 * Created: October 31, ?2019, 12:33:01 PM
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

#ifndef TIME_H_
#define TIME_H_

#define DST

typedef struct
{
	uint8_t	ticks;
	uint8_t	sec;
	uint8_t min;
	uint8_t hour;
	uint8_t	day;	
	uint8_t	month;
	uint16_t year;
	uint8_t dayofweek;

	// flags
	uint8_t Tick:1;
	uint8_t LeapYear:1;
	uint8_t SecFlag:1;
	uint8_t HalfSec:1;
	uint8_t FullSec:1;
	
#ifdef DST	
	uint8_t DST_Enable:1;	
	uint8_t DST_Active:1;
	uint8_t DST_Start;
	uint8_t DST_Stop;
#endif

} rtc_t;

typedef struct
{
//	uint8_t al_hour;
//	uint8_t al_min;
//	uint16_t al_length1;
//	uint16_t al_length2;
	uint32_t DDS_Adj;
} nv_setting_t;

extern volatile rtc_t time;

//extern volatile uint16_t countdown;
//extern volatile nv_setting_t Setting;

// North America DST
// https://www.timetemperature.com/northamerica/north_america_daylight_saving_time.shtml

enum DST_
{
	DST_Start_Month = 3,
	DST_Start_Week = 2,
	DST_End_Month = 11,
	DST_End_Week = 1,
	DST_ChangeDay	= 0,			// Sunday (not used in code)
	DST_ChangeTime = 2			// 2am
};

void Time_Init(void);
void Timer_Reload(void);
void RTC(void);
uint8_t MonthDays(uint8_t month, uint16_t year);
uint8_t DayWeek(uint8_t Day, uint8_t Month, uint16_t Year);
void RTC_AnnualUpdate(void);
void DST_Check(void);
void RTC_SetTime(uint8_t Hour, uint8_t Min, uint8_t Sec);
void RTC_SetDate(uint8_t Day, uint8_t Month, uint16_t Year);

void DST_FixState(void);

// TIM: 12MHz/(20*60000) = 10Hz, 100ms

#define TIM1_PRESCALER		(CPU_CLOCK/TIM1_RELOAD/TIM1_TICKS)
#define TIM1_RELOAD				60000UL
#define TIM1_TICKS				20UL

// DDS target frequency
#define TICKS_PER_SEC			10UL

#define DDS_BITS					24UL
#define DDS_CARRY					(1UL << DDS_BITS)
#define DDS_MASK					(DDS_CARRY -1UL)

#define DDS_INC						(DDS_CARRY * TICKS_PER_SEC)/TIM1_TICKS

#define DDS_Adj						-2609UL  // 2602: very close.  slow: -2612, -2650, -2598 fast: -2575,-2593, -2606UL

#define TIM1_PSCR_H				((TIM1_PRESCALER-1) >> 8)
#define TIM1_PSCR_L				((TIM1_PRESCALER-1) & 0xff)

#define TIME_HR_MAX				23
#define TIME_HR_MIN				0

#endif
