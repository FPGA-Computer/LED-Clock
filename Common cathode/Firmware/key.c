/*
 * key.c
 * November 1, 2019, 6:23:06 PM
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

Key_t Key;

void Key_Task(void)
{
	uint8_t new_code;
	
	new_code = Key_Scan();

  if(new_code==(Key.code & KEY_MASK))
	{
		if(new_code)
		{
			if(Key.count == KEY_CNT_MAX)
			{	
				if(!(new_code&KEY_MENU))						// no auto-repeat for menu key
					Key.count = KEY_RELOAD;
				
				Key.code |= KEY_LONG|KEY_FLAG;
			}
			else
				Key.count++;
		}
	}
	else
	{
		if(!new_code&&Key.code&&(Key.count>KEY_THRESHOLD)&&!(Key.code&(KEY_REPORTED|KEY_LONG)))
			Key.code|=KEY_FLAG;
		else
		{
			Key.code = new_code;
			Key.count = 0;
		}
	}
}

uint8_t Key_Get(void)
{ 
  if(Key.code & KEY_FLAG)
	{
		Key.code = (Key.code & ~KEY_FLAG)|KEY_REPORTED;
		return(Key.code & (KEY_MASK|KEY_LONG));
	}
	return(0);
}

uint8_t Key_Purge(void)
{
	Key.count = 0;
}
