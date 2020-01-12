/*
 * UI.h
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
 
#ifndef UI_H_
#define UI_H_

typedef struct
{	
	void	*Value;
	uint8_t	*Modified;
	uint8_t X;
	uint8_t Width;
	uint16_t Min;
	uint16_t Max;
	uint8_t Flags;
} UI_Item_t;

typedef struct
{	
	uint8_t		*Text;
	uint8_t		X;
	UI_Item_t *Items;
	uint8_t		Size;
} UI_Menu_t;

#define BLINK_EDIT_ITEM

typedef void (*FuncPtr)(void);
typedef void (*FuncPtr_arg)(void *);

#define UI_LARGE_STEP		10

enum DataFlags_ { D_HR, D_U8, D_U8Z, D_SetExit
/*, D_Chars, D_Menu, D_Function, D_CustomData */ };

#define DISPLAY_MASK	0x1f
#define DISPLAY_ADJ		0x80

void sec2ms(uint16_t time,uint8_t *min,uint8_t *sec);
void Print2d(int16_t Number,uint8_t Pos);
void Print_Hr(uint8_t hour,uint8_t display_opt);
void Print_Time(time_hms_t *time,uint8_t display_sec);
void UI_PrintItems(UI_Menu_t *Menu);
uint8_t UI_Menu(UI_Menu_t *Menu);

#define ZERO_SUP				0x80
#define COL_MASK				0x0f

enum _Display_Options
{ 
	DISPLAY_SEC=0x01, DISPLAY_24H = 0x02
};


#define HR_COL					0
#define MIN_COL					2
#define SEC_COL					4

//#define _AM_PM

#ifdef _AM_PM
	#define AM_SEG						SEG_F
	#define PM_SEG						SEG_E
#else
	#define AM_SEG						0
	#define PM_SEG						SEG_F
#endif

#define SEP_SEG						SEG_DP
#define CURSOR_SEG				SEG_D

/* 7 segment display

     --- A ---
    |         |
    F         B
    |         |
     --- G ---
    |         |
    E         C
    |         |
     --- D ---  DP
*/

enum _7Seg_chars
{
	Ch_0 =	SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F,
	Ch_1 =	      SEG_B|SEG_C,
	Ch_2 =	SEG_A|SEG_B|      SEG_D|SEG_E|      SEG_G,
	Ch_3 =	SEG_A|SEG_B|SEG_C|SEG_D|            SEG_G,
	Ch_4 =	      SEG_B|SEG_C|            SEG_F|SEG_G,
	Ch_5 =	SEG_A|      SEG_C|SEG_D|      SEG_F|SEG_G,
	Ch_6 =	SEG_A|      SEG_C|SEG_D|SEG_E|SEG_F|SEG_G,
	Ch_7 =	SEG_A|SEG_B|SEG_C,
	Ch_8 = SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F|SEG_G,
	Ch_9 =	SEG_A|SEG_B|SEG_C|SEG_D|      SEG_F|SEG_G,

	Ch_A = 	SEG_A|SEG_B|SEG_C|      SEG_E|SEG_F      ,
	Ch_B = 	            SEG_C|SEG_D|SEG_E|SEG_F|SEG_G,
	Ch_C = 	SEG_A|            SEG_D|SEG_E|SEG_F      ,
	Ch_D= 	      SEG_B|SEG_C|SEG_D|SEG_E|      SEG_G,
	Ch_E= 	SEG_A            |SEG_D|SEG_E|SEG_F|SEG_G,
	Ch_F = 	SEG_A                  |SEG_E|SEG_F|SEG_G,
	Ch_G = 	SEG_A|SEG_B|SEG_C|SEG_D      |SEG_F|SEG_G,
	Ch_H = 	      SEG_B|SEG_C      |SEG_E|SEG_F|SEG_G,
	Ch_I = 	                        SEG_E|SEG_F      ,
	Ch_J = 	      SEG_B|SEG_C|SEG_D|SEG_E            ,
	Ch_L = 	                  SEG_D|SEG_E|SEG_F      ,
	Ch_M1=	SEG_A|SEG_B            |SEG_E|SEG_F      ,
	Ch_M2= 	SEG_A|SEG_B|SEG_C            |SEG_F      ,
	Ch_N = 	SEG_A|SEG_B|SEG_C      |SEG_E|SEG_F      ,
	Ch_O = 	            SEG_C|SEG_D|SEG_E      |SEG_G,
	Ch_P = 	SEG_A|SEG_B            |SEG_E|SEG_F|SEG_G,	
	Ch_Q = 	SEG_A|SEG_B|SEG_C            |SEG_F|SEG_G,	
	Ch_R = 	                        SEG_E      |SEG_G,
	Ch_S = 	SEG_A      |SEG_C|SEG_D      |SEG_F|SEG_G,	
	Ch_T = 	                  SEG_D|SEG_E|SEG_F|SEG_G,
	Ch_U = 	      SEG_B|SEG_C|SEG_D|SEG_E|SEG_F      ,
	Ch_V = 	            SEG_C|SEG_D|SEG_E|SEG_F      ,
	Ch_W1= 	            SEG_C|SEG_D|SEG_E|SEG_F      ,
	Ch_W2= 	      SEG_B|SEG_C|SEG_D|SEG_E            ,
	Ch_Y = 	      SEG_B|SEG_C|SEG_D      |SEG_F|SEG_G,
	Ch_Neg = SEG_G
};

#ifdef HEX_PRINT
#define  HEX_CHAR
void PrintHex24(uint32_t N);
#endif

#endif
