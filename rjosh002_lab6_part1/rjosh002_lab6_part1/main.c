/*
 * rjosh002_lab3_part1.c
 *
 * Created: 8/2/2018 12:50:11 PM
 * Author : Rohit
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.c"

volatile unsigned char TimerFlag = 0;
unsigned char var = 0; 
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
enum states {start, reset, increment, decrement, wait} state;
void tick();

int main(void)

{

	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
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
		LCD_WriteData(var + '0');
		while (!TimerFlag){}
		TimerFlag = 0;
	}
}



void tick()
{
	unsigned char underByte = ~PINA & 0x03; //bitmask for the flag bit
	static unsigned char count;
	
	switch(state)
	{ //transitions
		case start:
			count = 0;
			if(underByte == 0x00)
			{
				state = start;
			}
			else if(underByte == 0x01)
			{
				state = increment;
			}
			else if(underByte == 0x02)
			{
				state = decrement;
			}
			else if(underByte == 0x03)
			{
				state = reset;
			}
			break;
		case wait:
			count = 0;
			if(underByte == 0x03) { state = reset;}
			else { state = underByte? wait : start;}
			break;
		case increment:
			if(var < 9 && (count == 0 || count == 10))
			{
				var++;
				count = 0;
			}
			if(underByte == 0x01 && var < 9) { 
				count++;
				state = increment;
			}
			else { state = wait;}
			break;
		case decrement:
			if(var > 0 && (count == 0 || count == 10))
			{
				var--;
				count = 0;
			}
			if(underByte == 0x02 && var > 0) {
				count++;
				state = decrement;
			}
			else { state = wait;}
			break;
		case reset:
			var = 0;
			state = underByte? reset : start;
			break;
			default:
			state = start;
			break;
	}
}