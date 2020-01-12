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

void Print2d(int16_t Number,uint8_t Pos)
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

void Print_Hr(uint8_t hour,uint8_t display_opt)
{
	uint8_t am=0;
	
	if(display_opt & DISPLAY_24H)
		Print2d(hour,HR_COL);
	else
	{
		if(hour<12)
			am =1;
		else
			hour-=12;
		
		if(!hour)
			hour=12;

		Print2d(hour,HR_COL|ZERO_SUP);
		Display[HR_COL]|=am?AM_SEG:PM_SEG;
	}
}

void Print_Time(time_hms_t *time,uint8_t display_opt)
{
	Print_Hr(time->hour,display_opt);
		
	Display[HR_COL+1]|=SEP_SEG;
	Print2d(time->min,MIN_COL);

	if(display_opt & DISPLAY_SEC)
	{	
		Display[MIN_COL+1]|=SEP_SEG;
		Print2d(time->sec,SEC_COL);
	}
}

void Display_Cursor(uint8_t x,uint8_t width)
{
	for(;width && (x<=COLUMN_MAX);width--)
		Display[x++] = CURSOR_SEG;
}

void ItemClear(uint8_t x, uint8_t width)
{
	for(;width && (x<=COLUMN_MAX);width--)
		Display[x++] = 0;
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
				Print_Hr(*(uint8_t*)Item->Value,0);
				break;
			case D_U8:
				Print2d(*(uint8_t*)Item->Value,x|ZERO_SUP);
				break;				
			case D_U8Z:
				Print2d(*(uint8_t*)Item->Value,x);
				break;
			case D_SetExit:
				UI_PrintItems((UI_Menu_t *)Item->Value);
				break;
/*				
			case D_CustomData:
				((FuncPtr_arg)Item->Modified)(Item);
				break;
*/
		}
	}
	else
		ItemClear(x,width);
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

		if(time_flag & (TIME_HALF_SEC|TIME_FULL_SEC))
		{		
				if(time_flag & TIME_FULL_SEC)
					UI_PrintItem(Item,1);
				else
					Display_Cursor(Item->X,Item->Width);

			time_flag &= ~(TIME_HALF_SEC|TIME_FULL_SEC);
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
			if(time_flag & (TIME_HALF_SEC|TIME_FULL_SEC))
			{
				UI_PrintItem(&Menu->Items[Item],time_flag & TIME_FULL_SEC);
				time_flag &= ~(TIME_HALF_SEC|TIME_FULL_SEC);
			}	
		}
	
	} while (!Quit);
	
	Key_Purge();
	ItemClear(0,COLUMN_MAX);
	return(Modified);
}
