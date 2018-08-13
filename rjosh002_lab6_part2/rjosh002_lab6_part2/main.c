/*
 * rjosh002_lab3_part1.c
 *
 * Created: 8/2/2018 12:50:11 PM
 * Author : Rohit and Jonathan
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.c"

volatile unsigned char TimerFlag = 0;
unsigned char var = 0; 
unsigned char flag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn() {
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff() {
	TCCR1B - 0x00;
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect){
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}
enum states {start, right, middle, left, wait} state;
void tick();

int main(void)

{

	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00; // LCD data lines
	DDRD = 0xFF; PORTD = 0x00; // LCD control lines

	TimerSet(100);
	TimerOn();
	
	state = start;
	LCD_init();
	LCD_WriteData(0 + '0');
	// Starting at position 1 on the LCD screen, writes Hello World
	//LCD_DisplayString(1, "Hello World");

	while(1)
	{
		tick();
		LCD_Cursor(1);
		if(var == 9)
		{
			LCD_DisplayString(1, "WEINER!!!!");
		}
		else if(var == 0)
		{
			LCD_DisplayString(1, "L7 Weenie :( ");
		}
		else
		{
			LCD_WriteData(var + '0');
		}
		while (!TimerFlag){}
		TimerFlag = 0;
	}
}



void tick()
{
	unsigned char A0 = ~PINA & 0x01; //bitmask for the flag bit

	static unsigned char count;
	
	switch(state)
	{ //transitions
		case start:
			count = 0;
			var = 5;
			state = right;
			break;
		case wait:
			count = 0;
			if(A0) {state = wait;}
			else {state = right;}
			break;
		case right:
			PORTB = 0x01;
			if(var < 9 && var > 0 && A0)
			{
				var--;
				count = 0;
				state = wait;
			}
			else if(!(count < 2) && var != 9 && var != 0 && !A0)
			{
				count = 0;
				state = middle;
			}
			else
			{
				count++;
				state = right;
			}
			break;
		case middle:
			PORTB = 0x02;
			if(var < 9 && var > 0 && A0)
			{
				var++;
				count = 0;
				state = wait;
			}
			else if(!(count < 2) && var != 9 && var != 0 && !A0)
			{
				count = 0;
				if(flag == 0) 
				{
					state = left;
				}
				else 
				{
					flag = 0;
					state = right;
				}
			}
			else
			{	count++;
				state = middle;
			}
			break;
		case left:
			PORTB = 0x04;
			if(var < 9 && var > 0 && A0)
			{
				var--;
				count = 0;
				state = wait;
			}
			else if(!(count < 2) && var != 9 && var != 0 && !A0)
			{
				flag = 1;
				count = 0;
				state = middle;
			}
			else
			{
				count++;
				state = left;
			}
			break;
		default:
			state = start;
			break;
	}
}