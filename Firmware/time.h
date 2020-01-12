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

typedef struct
{
	uint8_t hour;
	uint8_t min;
	uint8_t	sec;
//	uint16_t al_length1;
//	uint16_t al_length2;
} time_hms_t;

typedef struct
{
//	uint8_t al_hour;
//	uint8_t al_min;
//	uint16_t al_length1;
//	uint16_t al_length2;
	uint8_t clock_trim;
} nv_setting_t;

extern volatile uint8_t time_flag,ticks;
extern volatile time_hms_t time;
extern volatile uint16_t countdown;
extern volatile nv_setting_t Setting;

void Time_Init(void);
void Timer_Reload(void);
void RTC(void);
void SetTime(uint8_t Hour, uint8_t Min, uint8_t Sec);

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
#define DDS_Adj						-2598UL  // 2602: very close.  2612, 2650: slow 2575,2593: fast 

#define TIM1_PSCR_H				((TIM1_PRESCALER-1) >> 8)
#define TIM1_PSCR_L				((TIM1_PRESCALER-1) & 0xff)

#define TIME_HR_MAX				23
#define TIME_HR_MIN				0

enum TimeFlagBits
{
	TIME_SEC_FLAG = 0x01,TIME_HALF_SEC = 0x02,TIME_FULL_SEC = 0x04,TIME_TICK = 0x08
};

#endif
