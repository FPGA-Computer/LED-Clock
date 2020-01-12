/*
 * hardware.h
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

#ifndef HARDWARE_H_
#define HARDWARE_H_

#ifndef __CSMC__
#define __CSMC__
#endif
#define STM8S003

#include "stm8s.h"
#include <stdio.h>

// STM8S003F3P6
enum _PA { PA1=0x02, PA2=0x04, PA3=0x08 };
enum _PB { PB4=0x10, PB5=0x20 };
enum _PC { PC3=0x08, PC4=0x10, PC5=0x20, PC6=0x40, PC7=0x80 };
enum _PD { PD1=0x02, PD2=0x04, PD3=0x08, PD4=0x10, PD5=0x20, PD6=0x40 };

enum _7SEG_ENCODE
{
	SEG_A=0x01,SEG_F=0x02,SEG_B=0x04,SEG_E=0x08,
	SEG_D=0x10,SEG_DP=0x20,SEG_C=0x40,SEG_G=0x80
};

enum _CC_DRV
{ 
	CC1=PC3,CC1_PORT=GPIOC_BaseAddress,
	CC2=PC7,CC2_PORT=GPIOC_BaseAddress,
	CC3=PD1,CC3_PORT=GPIOD_BaseAddress,
	CC4=PA3,CC4_PORT=GPIOA_BaseAddress,
	CC5=PD2,CC6_PORT=GPIOD_BaseAddress,
	CC6=PD3,CC5_PORT=GPIOD_BaseAddress
};

#define GPIO(X)					((GPIO_TypeDef *)(X))

#define PA_COLS					(CC4)
#define PC_COLS					(CC1|CC2)
#define PD_COLS					(CC3|CC5|CC6)

#define KEY_COL					0
#define SENSE_COL				1

#define PA_KEYS					(CC4)
#define PD_KEYS					(CC5|CC6)
#define PA_SHIFT				1
#define PD_SHIFT				2

#define SW_MENU					CC4
#define SW_INC					CC5
#define SW_DEC					CC6

// charge up parasitic
#define KEY_DELAY				0x08

enum keys_encoding 	{ KEY_MENU=0x04, KEY_DEC=0x02, KEY_INC=0x01, KEY_MASK=0x07,
											KEY_LONG=0x20, KEY_REPORTED=0x40, KEY_FLAG=0x80 };

#define SPI_SCK					PC5
#define SPI_MOSI				PC6
#define SPI_LE					PC4

#define SPI_PINS				(SPI_SCK|SPI_MOSI|SPI_LE)
#define SPI_PORT				GPIOC

#define UART_TX					PD5
#define UART_RX					PD6
#define UART_PORT				GPIOD

#define TIM2_CH1				PD4
#define TIM2_CH1_PORT		GPIOD

#define COLUMN_MAX			5

#define CPU_CLOCK				12000000UL
#define EE_Addr					0x4000

// 12MHz/128 -> 93750Hz
#define TIM4_PSCR				0x07
// 93750Hz/256 = 366Hz.  Full refresh = 366Hz/6 columns -> 61Hz
#define TIM4_ARR				0xff
#define SCAN_PER_SEC		(CPU_CLOCK/(1<<TIM4_PSCR)/(1+TIM4_ARR)/(COLUMN_MAX+1))

#define Clock_Trim			18  

typedef struct 
{
	uint8_t		Bit;
	uint16_t	Port;
} Column_Drv_t;

#include "irq.h"
#include "time.h"
#include "key.h"
#include "ui.h"
#include "clock.h"
#include "menu.h"
//#include "eeprom.h"
//#include "serial.h"

extern const Column_Drv_t ColumnDrv[];

extern volatile uint8_t Display_Col,tick,update,power,debugger;
extern volatile uint8_t Display[];
extern const uint8_t CharMap[];

void Init_Hardware(void);
void Update(void);
uint8_t Key_Scan(void);
#endif
