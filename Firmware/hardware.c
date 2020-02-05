/*
 * hardware.c
 *
 * Created: August 30, 2019, 6:35:08 PM
 *  Author: K. C. Lee
 * Copyright (c) 2016 by K. C. Lee
 
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

volatile uint8_t Display_Col,tick,update,power,debugger;
volatile uint8_t Display[COLUMN_MAX];

const uint8_t CharMap[] =
{	
	Ch_0,Ch_1,Ch_2,Ch_3,Ch_4,Ch_5,Ch_6,Ch_7,
	Ch_8,Ch_9
	
#ifdef HEX_CHAR	
	,Ch_A,Ch_B,Ch_C,Ch_D,Ch_E,Ch_F
#endif	
};

const Column_Drv_t ColumnDrv[] = 
{	
	{	CC6,CC6_PORT },{ ~CC1,CC1_PORT }, 
	{	CC1,CC1_PORT },{ ~CC2,CC2_PORT },
	{	CC2,CC2_PORT },{ ~CC3,CC3_PORT },
	{ CC3,CC3_PORT },{ ~CC4,CC4_PORT },
	{	CC4,CC4_PORT },{ ~CC5,CC5_PORT },
	{	CC5,CC5_PORT },{ ~CC6,CC6_PORT },
};

Column_Drv_t *ColDrv;

void Init_Hardware(void)
{
	debugger = RST->SR & RST_SR_SWIMF;
	
	// Switch clock enable
	CLK->SWCR |= CLK_SWCR_SWEN;
	// Select HSE
	CLK->SWR = 0xb4;
	// wait for switching
	while(CLK->SWCR & CLK_SWCR_SWBSY)
	  /* */ ;
	
	// Clk divider, CPU divider = 1
	CLK->CKDIVR = 0;

	Display_Col =0;
	ColDrv = ColumnDrv;

	// column drivers = open drain, set to '1'
	GPIOA->DDR = GPIOA->ODR = PA_COLS;	
	GPIOC->DDR = PC_COLS|SPI_PINS;
	GPIOC->ODR = PC_COLS;
	
	GPIOC->CR2 = SPI_PINS;	// SPI pins: Fast I/O
	GPIOD->DDR = GPIOD->ODR = PD_COLS;
	// disable push-pull from SWIM
	GPIOD->CR1 = 0;
	
	// TIM4 - display update
	TIM4->PSCR = TIM4_PSCR;	
	TIM4->ARR = TIM4_ARR;	
	// TIM4 enable
	TIM4->CR1 = TIM4_CR1_CEN;	
	TIM4->IER = TIM4_IER_UIE;	
	
	Time_Init();

	// SPI MSB first, Enable, Master, data rate = CPU/2
	SPI->CR1 = SPI_CR1_SPE|SPI_CR1_MSTR;
	// bidirection mode,output,software slave select,master
	SPI->CR2 = SPI_CR2_BDM|SPI_CR2_BDOE|SPI_CR2_SSM|SPI_CR2_SSI;
	
	// 	Init_Serial();
	
	// Turn on interrupts
	rim();
}

// return raw key(s) pressed

uint8_t Key_Scan(void)
{
	uint8_t key, i;
	
	// set ports to input 
	GPIOA->DDR &= ~PA_KEYS;
	GPIOD->DDR &= ~PD_KEYS;
	// enable pull-up
	GPIOA->CR1 |= PA_KEYS;
	GPIOD->CR1 |= PD_KEYS;

	for(i=KEY_DELAY;i;i--)
		;

	key = ~(((GPIOA->IDR & PA_KEYS)>>PA_SHIFT)|((GPIOD->IDR & PD_KEYS)>>PD_SHIFT))& KEY_MASK;
	
	// disable pull-up	
	GPIOA->CR1 &= ~PA_KEYS;
	GPIOD->CR1 &= ~PD_KEYS;
	// restore I/O setting
	GPIOA->DDR |= PA_KEYS;
	GPIOD->DDR |= PD_KEYS;

	return(key);
}

void PowerDetect(void)
{
	GPIOC->DDR &= ~CC1;
	power = (GPIOC->IDR & CC1);
	GPIOC->DDR |= CC1;
}

// display update + scan keys
@far @interrupt void TIM4_Update_IRQ(void)
{
	TIM4->SR1 &= ~TIM4_SR1_UIF;
	
	// Turn off prev column	
	GPIO(ColDrv->Port)->ODR |= ColDrv++->Bit;
	
	// skip to reduce power and leakages
	if(power)
	{
		SPI->DR = Display[Display_Col];
		
		// Move here as pool needed a delay to flush pipeline
		SPI_PORT->ODR &= ~SPI_LE;		
	
		while(SPI->SR & SPI_SR_BSY)
			/* wait for transmit */;
		
		// latch SPI data
		SPI_PORT->ODR |= SPI_LE;
		
		// Turn on column
		GPIO(ColDrv->Port)->ODR &= ColDrv++->Bit;

		// scan for keys
		if(Display_Col==KEY_COL)
			Key_Task();
	}
	
	// scan input power
	if(Display_Col==SENSE_COL)
		PowerDetect();

	if(Display_Col>=COLUMN_MAX)
	{
	  Display_Col=0;
		ColDrv = ColumnDrv;
	}
	else
		Display_Col++;
}
