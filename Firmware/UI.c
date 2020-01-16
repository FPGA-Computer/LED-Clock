/*
 * ui.c
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

/*
void sec2ms(uint16_t time,uint8_t *min,uint8_t *sec)
{
	*min = time/60;	
	*sec = time-*min*60;
}
*/

#ifdef HEX_PRINT

void PrintHex24(uint32_t N)
{
	uint8_t pos=5, digit;
	
	do
	{
		digit = N & 0x0f;
		Display[Pos--] = CharMap[Digit];
		N >>=4;
		
	}while(pos)
}
#endif

void UI_Print2d(int16_t Number,uint8_t Pos)
{
	uint8_t ZeroSup = Pos & ZERO_SUP;
	uint16_t Digit;
	
	Pos &= COL_MASK;
	
	if(Number < 0)
	  Number = 0;
	
	Digit = Number/10;
	
	if(!Digit && ZeroSup)			// Supress 0
		Display[Pos++] = 0;
	else
		Display[Pos++] = CharMap[Digit];

	Digit = Number%10;
	Display[Pos] = CharMap[Digit];	
}

void UI_Print_Str(uint8_t *str,uint8_t x)
{
	for(;x<=COLUMN_MAX;)
		Display[x++] = *str++;
}

void UI_Fill(uint8_t ch, uint8_t x,uint8_t width)
{
	for(;width && (x<=COLUMN_MAX);width--)
		Display[x++] = ch;
}

void UI_Print_Hr(uint8_t hour,uint8_t display_opt)
{
	uint8_t am=0;
	
	if(display_opt & DISPLAY_24H)
		UI_Print2d(hour,HR_COL);
	else
	{
		if(hour<12)
			am =1;
		else
			hour-=12;
		
		if(!hour)
			hour=12;

		UI_Print2d(hour,HR_COL|ZERO_SUP);
		Display[HR_COL]|=am?AM_SEG:PM_SEG;
	}
}

void UI_Print_Time(rtc_t *time,uint8_t display_opt)
{
	UI_Print_Hr(time->hour,display_opt);
		
	Display[HR_COL+1]|=SEP_SEG;
	UI_Print2d(time->min,MIN_COL);

	if(display_opt & DISPLAY_SEC)
	{	
		Display[MIN_COL+1]|=SEP_SEG;
		UI_Print2d(time->sec,SEC_COL);
	}
}
const uint8_t DayofWeek[] = 
{ Ch_S,Ch_u,0, Ch_M1,Ch_M2,0, Ch_T,Ch_u,0, Ch_W1|SEG_DP,Ch_W2,0, 
	Ch_T,Ch_h,0, Ch_F,Ch_r,0, Ch_S,Ch_A,0
};
																
// print date: WW.DD.MM
void UI_Print_Date(rtc_t *time)
{
	UI_Print_Str(&DayofWeek[time->dayofweek*3],DOW_COL);
	UI_Print2d(time->day,DAY_COL|ZERO_SUP);
	UI_Print2d(time->month,MONTH_COL);
}

void UI_PrintItem(UI_Item_t *Item, uint8_t Disp)
{
	uint8_t *ptr;
	uint8_t x = Item->X, width=Item->Width;
	
	if(Disp)
	{
		switch(Item->Flags &  DISPLAY_MASK)
		{
			case D_HR:
				UI_Print_Hr(*(uint8_t*)Item->Value,0);
				break;
			case D_U8:
				UI_Print2d(*(uint8_t*)Item->Value,x|ZERO_SUP);
				break;				
			case D_U8Z:
				UI_Print2d(*(uint8_t*)Item->Value,x);
				break;
/*			case D_Str:
				UI_Print_Str((uint8_t*)Item->Value,x);		
				break;
*/

/*				
			case D_CustomData:
				((FuncPtr_arg)Item->Modified)(Item);
				break;
*/
			case D_SetExit:
				UI_PrintItems((UI_Menu_t *)Item->Value);
				break;
		}
	}
	else
		UI_Fill(0,x,width);
}

void UI_PrintItems(UI_Menu_t *Menu)
{
	uint8_t i,*ptr;

	if(Menu->Text)
		for(i=Menu->X,ptr=Menu->Text;*ptr;)
			Display[i++] = *ptr++;

	for(i=0;i<Menu->Size;i++)
	{
		// prevent recursive calls
		if((Menu->Items[i].Flags & DISPLAY_MASK)!=D_SetExit)
			UI_PrintItem(&Menu->Items[i],1);
	}
}

uint8_t UI_EditItem(UI_Item_t *Item)
{
	uint8_t Changed=0,Quit=0,Update=1;
	uint8_t Value=*(uint8_t*)Item->Value;
	
	Key_Purge();
	
	do
	{
		switch(Key_Get())
		{
			case KEY_DEC:
				if(Value > Item->Min)
					Value--;
				else
					Value = Item->Max;										// wrap around
					
				Update=1;					
				Changed |= 1;

				break;
				
			case KEY_DEC|KEY_LONG:
				if(Value >= (Item->Min+UI_LARGE_STEP))
					Value -= UI_LARGE_STEP;
				else																		// wrap around
					Value = Item->Max /* + (Value-UI_LARGE_STEP-Item->Min) +1*/;
				
				Changed |= 1;
				Update=1;				
				break;
				
			case KEY_INC:
				if(Value < Item->Max)
					Value++;
				else
					Value = Item->Min;										// wrap around
					
				Update=1;							
				Changed |= 1;
		
				break;
				
			case KEY_INC|KEY_LONG:
				if(Value <= (Item->Max-UI_LARGE_STEP))
					Value += UI_LARGE_STEP;
				else																		// wrap around
					Value = Item->Min /*+ (Value+UI_LARGE_STEP-Item->Max) -1*/;
					
				Update=1;				
				Changed |= 1;

				break;
				
			case KEY_MENU:
			case KEY_MENU|KEY_LONG:
				Quit = 1;
				break;
		}

		if(time.HalfSec||time.FullSec)
		{		
				if(time.FullSec)
					UI_PrintItem(Item,1);
				else
					UI_Fill(CURSOR_SEG,Item->X,Item->Width);

			time.HalfSec = 0;
			time.FullSec = 0;
		}		

		if(Update||Quit)
		{	
			*(uint8_t*)Item->Value=Value;
			UI_PrintItem(Item,Update||Quit);
			Update = 0;
		}
			
	} while (!Quit);

	Key_Purge();
	return(Changed);
}

uint8_t UI_Menu(UI_Menu_t *Menu)
{
	uint8_t key, Modified=0,ItemModified=0,Quit=0,Item=0,prev_item,MenuSize_1=Menu->Size-1;
		
	Key_Purge();	

	UI_PrintItems(Menu);
			
	do
	{	
		prev_item = Item;
		key = Key_Get();
		
		switch(key & ~KEY_LONG)
		{
			case KEY_MENU:
				if(key & KEY_LONG)
					Quit = 1;
				else 
					switch(Menu->Items[Item].Flags)
					{
						case D_SetExit:					// quick exit setting
							Quit = 1;
							break;

/*
						case D_Menu:
							// Item is a menu
								UI_Menu((UI_Menu_t *)Menu->Items[Item].Modified);
								UI_PrintItems(Menu);					
							break;
						
						case D_Function:
							// Item is a function call
							((FuncPtr)Menu->Items[Item].Modified)();
							UI_PrintItems(Menu);						
							break;
*/						
						default:
							ItemModified=UI_EditItem(&Menu->Items[Item]);
							*Menu->Items[Item].Modified|=ItemModified;
							
							UI_PrintItems(Menu);
							Modified|=ItemModified;
					}
					
			// fall through to next item		
				
			case KEY_INC:
				if(Item < MenuSize_1)
					Item++;				
				else
					Item = 0;									// wrap around
				break;
				
			case KEY_DEC:
				if(Item > 0)
					Item--;
				else
					Item = MenuSize_1;				// wrap around
				break;
		}
		
		if(prev_item != Item)
			UI_PrintItem(&Menu->Items[prev_item],1);	// restore prev item
		else
		{
			// blink
			if(time.HalfSec||time.FullSec)
			{
				UI_PrintItem(&Menu->Items[Item],time.FullSec);
				time.HalfSec = 0;
				time.FullSec = 0;
			}	
		}
	
	} while (!Quit);
	
	Key_Purge();
	UI_Fill(0,0,COLUMN_MAX);
	return(Modified);
}
