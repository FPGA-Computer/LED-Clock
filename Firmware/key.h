/*
 * key.h
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
 
#ifndef KEY_H_
#define KEY_H_

typedef struct
{
	volatile uint8_t code;
	volatile uint8_t count;
} Key_t;

extern Key_t Key;

void Key_Task(void);
uint8_t Key_Get(void);
uint8_t Key_Purge(void);

#define KEY_REPEAT_RATE		4
#define KEY_THRESHOLD			3
#define KEY_RELOAD				30
#define KEY_CNT_MAX				(KEY_RELOAD+SCAN_PER_SEC/2/KEY_REPEAT_RATE)


#endif
