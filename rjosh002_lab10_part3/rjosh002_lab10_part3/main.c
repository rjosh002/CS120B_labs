#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.c"
#include "bit.h"
#include "keypad.h"

volatile unsigned char TimerFlag = 0;
unsigned char x = 0;
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

// int main(void)
// 
// {
// 
// 	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
// 	DDRC = 0xFF; PORTC = 0x00; // LCD data lines
// 	DDRD = 0xFF; PORTD = 0x00; // LCD control lines
// 
// 	TimerSet(100);
// 	TimerOn();
// 	
// 	state = start;
// 	LCD_init();
// 	LCD_WriteData(0 + '0');
// 	// Starting at position 1 on the LCD screen, writes Hello World
// 	//LCD_DisplayString(1, "Hello World");
// 
// 	while(1)
// 	{
// 		tick();
// 		LCD_Cursor(1);
// 		LCD_WriteData(var + '0');
// 		while (!TimerFlag){}
// 		TimerFlag = 0;
// 	}
// }


//--------User defined FSMs---------------------------------------------------
enum states {start} state;
void TickFct_Keys()
{
	x = GetKeypadKey();
	
	switch(state){
		case start:
			switch (x) {
				case '\0': PORTB = 0x1F; break; // All 5 LEDs on
				case '1': PORTB = 0x01;  break; // hex equivalent
				case '2': PORTB = 0x02; break; //LCD_WriteData(2 + '0'); break; // hex equivalent
				case '3': PORTB = 0x03; break; // All 5 LEDs on
				case '4': PORTB = 0x04; break; // hex equivalent
				case '5': PORTB = 0x05; break;
				case '6': PORTB = 0x06; break;
				case '7': PORTB = 0x07; break;
				case '8': PORTB = 0x08; break;
				case '9': PORTB = 0x09; break;
				case 'A': PORTB = 0x0A; break;
				case 'B': PORTB = 0x0B; break;
				case 'C': PORTB = 0x0C; break;
				case 'D': PORTB = 0x0D; break;
				case '*': PORTB = 0x0E; break;
				case '0': PORTB = 0x00; break;
				case '#': PORTB = 0x0F; break;
				default: PORTB = 0x1B; break; // Should never occur. Middle LED off.
			}
		state = start;
		break;
	}
}

int main(void)
{
		DDRA = 0xFF; PORTA = 0x00; // PORTB set to output, outputs init 0s
	DDRB = 0xFF; PORTB = 0x00; // PORTB set to output, outputs init 0s
	DDRC = 0xF0; PORTC = 0x0F; // PC7..4 outputs init 0s, PC3..0 inputs init
	DDRD = 0xFF; PORTD = 0x00; // PORTB set to output, outputs init 0s
	
	LCD_init();
	LCD_WriteData(0 + '0');
	TimerSet(100);
	TimerOn();
	
	state = start;
	
	while(1){
		TickFct_Keys();
		LCD_Cursor(1);
		if(PORTB == 0x1F){ LCD_WriteData(0);}
		else{ LCD_WriteData(x);}
		while (!TimerFlag){}
		TimerFlag = 0;
	}
	return 0;
}
