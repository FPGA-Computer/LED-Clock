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
volatile uint8_t Display[COLUMN_MAX+1];

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
	
	CLK->CKDIVR = CPU_DIV;

	Display_Col =0;
	ColDrv = ColumnDrv;

	// column drivers = Push-pull, set to '1'
	GPIOB->DDR = GPIOB->CR1 = GPIOB->ODR = PB_COLS;
	GPIOC->DDR = GPIOC->CR1 = PC_COLS|SPI_PINS;
	GPIOC->ODR = PC_COLS|SPI_LE|SPI_MOSI;
	GPIOC->CR2 = SPI_PINS;												// SPI pins: Push-Pull/Fast I/O
	GPIOD->DDR = GPIOD->ODR = PD_COLS | PD_SWIM;
	// disable SWIM	push-pull
	GPIOD->CR1 = PD_COLS;

	// TIM4 - display update
	TIM4->PSCR = TIM4_PSCR;	
	TIM4->ARR = TIM4_ARR;	
	// TIM4 enable, ARR Preload
	TIM4->CR1 = TIM4_CR1_CEN|TIM4_CR1_ARPE;	
	TIM4->IER = TIM4_IER_UIE;	
	
	Time_Init();

	// SPI MSB first, Enable, Master, data rate = CPU/2
	SPI->CR1 = SPI_CR1_SPE|SPI_CR1_MSTR;
	// bidirection mode,output,software slave select,master
	SPI->CR2 = SPI_CR2_BDM|SPI_CR2_BDOE|SPI_CR2_SSM|SPI_CR2_SSI;

	power = 1;
	// Turn on interrupts
	rim();
}

// return raw key(s) pressed
	uint16_t ADC_Result;
	
uint8_t Key_Scan(void)
{
	uint8_t key, i, PortC, PortD;

	// Disable SPI 
	SPI->CR1 &= ~SPI_CR1_SPE;
	
	// set ports to input
	GPIOC->DDR &= ~KEY_PC;
	PortC = GPIOC->IDR;
	GPIOC->DDR |= KEY_PC;
	
	// Enable SPI
	SPI->CR1 |= SPI_CR1_SPE;
	
	GPIOD->DDR &= ~KEY_PD;
	PortD = GPIOD->IDR;
	GPIOD->DDR |= KEY_PD;

	key = ~(((PortC & KEY_PC)>>KEY_PC_SHIFT)|(PortD & KEY_PD))& KEY_MASK;
	return(key);
}

void PowerDetect(void)
{
	uint8_t detect;
	
	detect=(GPIOB->IDR & PB_COLS)!=0;

	if(power!=detect)		// detect a power change
	{
		if(detect)				// power returns
		{
			// enable timer 4
			TIM4->CR1 = TIM4_CR1_CEN;	
			CLK->CKDIVR = CPU_DIV;	
		}
		else							// power outage
		{
			// disable timer 4 - display refresh, key scan disabled
			TIM4->CR1 = 0;
			CLK->CKDIVR = CPU_SLOW;
			GPIOB->ODR = PB_COLS;
		}
		
		power = detect;
	}
}

// display update + scan keys
@far @interrupt void TIM4_Update_IRQ(void)
{
	TIM4->SR1 &= ~TIM4_SR1_UIF;
	
	// Turn off prev column	
	GPIO(ColDrv->Port)->ODR |= ColDrv++->Bit;
	
	if(_7SEG_ACTIVE_LOW)
		SPI->DR = ~Display[Display_Col];
	else
		SPI->DR = Display[Display_Col];

	// Move here as needed a delay to flush pipeline
	SPI_PORT->ODR &= ~SPI_LE;
	
	while(SPI->SR & SPI_SR_BSY)
		/* wait for transmit */;

	// latch SPI data
	SPI_PORT->ODR |= SPI_LE;
	
	// Turn on column
	GPIO(ColDrv->Port)->ODR &= ColDrv++->Bit;
	
	PowerDetect();
	
	switch(Display_Col++)
	{	
		case LED_NOM_COL:
			TIM4->ARR = TIM4_ARR;
			break;
	
		case LED_DIM_COL:
			TIM4->ARR = TIM4_ARR_DIM;
			break;
			
		case COLUMN_MAX:
			Key_Task();								// scan for keys
			Display_Col=0;
			ColDrv = ColumnDrv;
			break;
	}
}
