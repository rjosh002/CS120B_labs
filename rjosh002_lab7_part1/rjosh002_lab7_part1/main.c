/*
 * rjosh002_lab7_part1.c
 *
 * Created: 8/13/2018 3:11:19 PM
 * Author : Rohit parking sensei Jonathan tha Swag master
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.c"
#define size 3

volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;
unsigned char flag = 0;
unsigned char b3 = 0x08;
unsigned char bottomB = 0x07;

typedef struct Task {
	int state;
	unsigned long period;
	unsigned long elapsedTime;
	int (*TickFct)(int);
} task;

task tasks[size];

enum TL_states {TL_start, right, middle, left, wait} TL_state;
int TickFct_ThreeLED(int state)
{
	static unsigned char count;
	
	switch(TL_state)
	{ //transitions
		case TL_start:
		count = 0;
		TL_state = right;
		break;
		case right:
		bottomB = 0x01;
		if(count<10){
			count++;
			TL_state=right;
		}
		else
		{
			count=0;
			TL_state=middle;
		}
		break;
		case middle:
		bottomB = 0x02;
		if(count<10)
		{
			count++;
			TL_state=middle;
		}
		else
		{
			count=0;
			TL_state = left;
		}
		break;
		case left:
		bottomB = 0x04;
		if(count<10)
		{
			count++;
			TL_state=left;
		}
		else
		{
			count=0;
			TL_state = right;
		}
			break;
		default:
			TL_state = TL_start;
			break;
	}
};
enum BL_states {BL_start, on, off} BL_state;
int TickFct_BlinkLED(int state)
{
	static unsigned char count;
	
	switch(BL_state)
	{ //transitions
		case BL_start:
		count = 0;
		BL_state = on;
		break;
		case on:
		b3 = 0x08;
		if(count<10){
			count++;
			BL_state=on;
		}
		else{
			count=0;
			BL_state=off;
		}
		break;
		case off:
		b3 = 0x00;
		if(count<10)
		{
			count++;
			BL_state=off;
		}
		else
		{
			count=0;
			BL_state = on;
		}
		break;
		default:
		BL_state = BL_start;
		break;
	}
};
enum m_states {m_start, synch} m_state;
int TickFct_mergedState(int state)
{
	static unsigned char count;
	
	switch(m_state)
	{ //transitions
		case m_start:
			count=0;
			m_state= synch;
			break;
		case synch:
			if(count < 10) 
			{
				count++;
				m_state = synch;
			}
			else // if(!(count < 10))
			{
				count=0;
				PORTB = b3 | bottomB;
				m_state = synch;
			}
			break;
		default:	
			count = 0;
			m_state = m_start;
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
		tasks[i].elapsedTime += tasks[i].period;
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

	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRB = 0xFF; PORTB = 0x00;

	TimerSet(100);
	TimerOn();
	
	unsigned char i = 0;
	tasks[i].state = BL_start;
	tasks[i].period = 100;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &TickFct_BlinkLED;
	i++;
	tasks[i].state = TL_start;
	tasks[i].period = 100;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &TickFct_ThreeLED;
	i++;
	tasks[i].state = m_start;
	tasks[i].period = 100;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &TickFct_mergedState;
	
	while(1)
	{
// 		while (!TimerFlag){}
// 		TimerFlag = 0;
	}
	return 0;
}