/*
 * rjosh002_lab7_part1.c
 *
 * Created: 8/13/2018 3:11:19 PM
 * Author : Rohit parking sensei Jonathan tha Swag 
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.c"
#define size 1

volatile unsigned char TimerFlag = 0; 
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;
unsigned long tasksPeriod = 1;

void ADC_init() {
    ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
    // ADEN: setting this bit enables analog-to-digital conversion.
    // ADSC: setting this bit starts the first conversion.
    // ADATE: setting this bit enables auto-triggering. Since we are
    //        in Free Running Mode, a new conversion will trigger whenever
    //        the previous conversion completes.
}


typedef struct Task {
	int state;
	unsigned long period;
	unsigned long elapsedTime;
	int (*TickFct)(int);
} task;

task tasks[size];

enum LED_states {LED_start, synch} LED_state;
int TickFct_LEDState(int state)
{	
	unsigned short x = ADC;  // Value of ADC 10-bit register now stored in variable x.
	//unsigned short max = 0xFF;
	
	switch(LED_state)
	{ //transitions
		case LED_start:
			LED_state= synch;
			break;
		case synch:
// 				if(x <= 0x1F)
// 				{
// 					PORTB = 0x00;
// 				}
				if(x <= 0x3F)
				{
					PORTB = 0x00;
				}
				if(x >= 0x3F)
				{
					PORTB = 0x0F;
				}
				if(x >= 0x7F)
				{
					PORTB = 0x3F;
				}
				if(x >= 0xFF)
				{
					PORTB = 0xFF;
				}
			LED_state = LED_start;
			break;
		default:	
			LED_state = LED_start;
			break;
	}
}

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
	//TimerFlag = 1;
	unsigned char i;
	for(i = 0; i < size; i++){
		if(tasks[i].elapsedTime >= tasks[i].period){
			tasks[i].state = tasks[i].TickFct(tasks[i].state);
			tasks[i].elapsedTime = 0;
		}
		tasks[i].elapsedTime += tasksPeriod;
	}
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

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; // Configure port A, B, and C's 8 pins as inputs
	DDRB = 0xFF; PORTB = 0x00;
	DDRD = 0xFF; PORTD = 0x00; 

	TimerSet(tasksPeriod);
	TimerOn();
	ADC_init();
	
	unsigned char i = 0;
	tasks[i].state = LED_start;
	tasks[i].period = 100;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &TickFct_LEDState;
		
	while(1)
	{
		
// 		while (!TimerFlag){}
// 		TimerFlag = 0;
	}
	return 0;
}