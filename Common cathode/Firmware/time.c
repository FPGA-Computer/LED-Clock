/*
 * time.c
 *
 * Created: October 31, 2019, 12:33:01 PM
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

volatile rtc_t time;
//volatile nv_setting_t Setting;
uint32_t DDS_PhaseInc;
volatile uint32_t DDS_Accum;

void Time_Init(void)
{	// prescaler
	TIM1->PSCRH = TIM1_PSCR_H;
	TIM1->PSCRL = TIM1_PSCR_L;
	
	// Preload, Counter enable
	
	// use DDS for clock fine adjustments
	DDS_PhaseInc = DDS_INC + DDS_Adj;
	
	// Autoreload
	TIM1->ARRH = (TIM1_RELOAD-1) >> 8;
	TIM1->ARRL = (TIM1_RELOAD-1) & 0xff;

	TIM1->CR1 = TIM1_CR1_ARPE|TIM1_CR1_CEN;
	// Update Interrupt
	TIM1->IER = TIM1_IER_UIE;
	
	RTC_SetDate(1,1,YEAR_START);
}

// RTC

@far @interrupt void TIM1_IRQ(void)
{
	uint8_t carry;
	
	// clear UIF
	TIM1->SR1 &= ~TIM1_SR1_UIF;
	
	// tick comes from Direct Digital Synthesis
	DDS_Accum += DDS_PhaseInc;
	
	if (DDS_Accum & DDS_CARRY)
	{
		if(time.ticks)
		{	
			// blink decimal point
			if(time.ticks==(TICKS_PER_SEC/2))
				time.HalfSec = 1;
	
			time.ticks--;
		}
		else
		{
			time.ticks = TICKS_PER_SEC-1;
	
			time.sec++;
			
			if(time.sec > 59)
			{
				time.sec = 0;
				time.min++;
			
				if(time.min > 59)
				{
					time.min = 0;			
					time.hour++;
					
					#ifdef DST
						if(time.DST_Enable)	// check every hour on the hour
							DST_Check();
					#endif	
				}
				if(time.hour > TIME_HR_MAX)
				{
					time.hour = TIME_HR_MIN;
					
					if(time.dayofweek < 6)
						time.dayofweek++;
					else
						time.dayofweek = 0;
					
					if(time.day<MonthDays(time.month,time.year))
						time.day++;
					else
					{
						time.day = 1;
						
						if(time.month <12)
							time.month++;
						else
						{
							time.month = 1;
							time.year++;
							RTC_AnnualUpdate();
						}
					}
				}
			}
			
			time.SecFlag = 1;
			time.FullSec = 1;
		}	
	}
	
	DDS_Accum &= DDS_MASK;
	time.Tick = 1;
}

/* https://www.wikihow.com/Calculate-Leap-Years */
uint8_t LeapYear(uint8_t year)
{
	if(year%4)
		return(0);
	else if(year%100)
		return(1);
	else if(year%400)
		return(0);
	else			
		return(1);
}

void RTC_AnnualUpdate(void)
{
	time.LeapYear=LeapYear(time.year);

	#ifdef DST
		if(time.DST_Enable)
		{
			uint8_t dayofweek;

			dayofweek = DayWeek(1,DST_Start_Month,time.year);
			time.DST_Start = 1+ 7*(DST_Start_Week-1) + dayofweek;
			dayofweek = DayWeek(1,DST_End_Month,time.year);
			time.DST_Stop = 1+ 7*(DST_End_Week-1) + dayofweek;		
		}
	#endif		
}

// months starts at 1
const uint8_t MonthDays_Tbl[]= { 0,31,28,31,30,31,30,31,31,30,31,30,31 };

uint8_t MonthDays(uint8_t month, uint16_t year)
{
	if((month !=2)||!time.LeapYear)
	  return(MonthDays_Tbl[month]);
	else
		return(29);
}

/*
	https://en.wikipedia.org/wiki/Determination_of_the_day_of_the_week
	Sakamoto's methods
*/
uint8_t DayWeek(uint8_t Day, uint8_t Month, uint16_t Year)
{
	const uint8_t t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
	uint16_t y;

	y = Year - (Month < 3);
	return ((y + y/4 - y/100 + y/400 + t[Month-1] + Day) % 7);
}

#ifdef DST

void DST_Check(void)
{
	if(time.DST_Active)
	{
		if(	(time.month == DST_End_Month)&&
				(time.day == time.DST_Stop)&&
				(time.hour == DST_ChangeTime) )
		{
			time.hour --;
			time.DST_Active = 0;
		}
	}
	else
	{
		if(	(time.month == DST_Start_Month)&&
				(time.day == time.DST_Start)&&
				(time.hour == DST_ChangeTime) )
		{
			time.hour ++;
			time.DST_Active = 1;
		}
	}
}

// Quick & dirty date compare - ignore differences in # of days in the month
int16_t DST_CmpDates(uint8_t M,uint8_t D)
{
	return((time.month-M)*31+(time.day-D));
}

int32_t DST_CmpTime(void)
{
	return((time.hour-DST_ChangeTime)*3600+time.min*60+time.sec);
}

// fix up DST state after time/data change
void DST_FixState(void)
{	int16_t cmp;

	cmp=DST_CmpDates(DST_Start_Month,time.DST_Start);

	if(cmp<0)								// < DST start date
		time.DST_Active = 0;
	else if(cmp==0)					// == DST start date
		time.DST_Active = (DST_CmpTime()>=0);
	else										// > DST start date
	{
		cmp=DST_CmpDates(DST_End_Month,time.DST_Stop);	
		
		if(cmp<0)							// < DST end date
			time.DST_Active = 1;
		else if(cmp==0)				// == DST end date
			time.DST_Active = (DST_CmpTime()< 0L);
		else									// > DST end date
			time.DST_Active = 0;
	}
}

#endif		

void RTC_SetTime(uint8_t Hour, uint8_t Min, uint8_t Sec)
{
	sim();
	time.hour =	Hour;
	time.min =	Min;	
	time.sec =	Sec;
	time.ticks = TICKS_PER_SEC-1;

#ifdef DST	
	DST_FixState();
#endif

	rim();
}

void RTC_SetDate(uint8_t Day, uint8_t Month, uint16_t Year)
{	
	uint8_t dayofweek;

	dayofweek=DayWeek(Day,Month,Year);
	sim();
	time.dayofweek = dayofweek;
	time.day = Day;
	time.month = Month;	
	time.year =	Year;
	RTC_AnnualUpdate();
	DST_FixState();
	rim();
}
