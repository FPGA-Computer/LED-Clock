/*
 * clock.h
 *
 * Created: November 1, 2019, 6:22:57 PM
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

#ifndef CLOCK_H_
#define CLOCK_H_

enum _ClockMode
{
	Mode_Clock,
	Mode_Last
};

typedef struct
{
	FuncPtr Mode;
	FuncPtr Mode_Setting;
	FuncPtr *AltModes;
	uint8_t	Alt;
	uint8_t flag;
} Clock_Modes_t;

void ClockApp(void);
void DisplayClock(void);

#endif
