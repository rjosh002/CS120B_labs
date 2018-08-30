/*
 * rjosh002_lab9_part1.c
 *
 * Created: 8/16/2018 12:11:30 PM
 * Author : Rohit
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.c"
#define size 1
#define A0 (~PINA & 0x01)
#define A1 (~PINA & 0x02)
#define A2 (~PINA & 0x04)
unsigned char B3 = 0x08;


volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;
unsigned long tasksPeriod = 1;

typedef struct Task {
	int state;
	unsigned long period;
	unsigned long elapsedTime;
	int (*TickFct)(int);
} task;

task tasks[size];

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





// 0.954 hz is lowest frequency possible with this function,
// based on settings in PWM_on()
// Passing in 0 as the frequency will stop the speaker from generating sound
void set_PWM(double frequency) {
	static double current_frequency; // Keeps track of the currently set frequency
	// Will only update the registers when the frequency changes, otherwise allows
	// music to play uninterrupted.
	if (frequency != current_frequency) {
		if (!frequency) { TCCR3B &= 0x08; } //stops timer/counter
		else { TCCR3B |= 0x03; } // resumes/continues timer/counter
		
		// prevents OCR3A from overflowing, using prescaler 64
		// 0.954 is smallest frequency that will not result in overflow
		if (frequency < 0.954) { OCR3A = 0xFFFF; }
		
		// prevents OCR0A from underflowing, using prescaler 64					// 31250 is largest frequency that will not result in underflow
		else if (frequency > 31250) { OCR3A = 0x0000; }
		
		// set OCR3A based on desired frequency
		else { OCR3A = (short)(8000000 / (128 * frequency)) - 1; }

		TCNT0 = 0; // resets counter
		current_frequency = frequency; // Updates the current frequency
	}
}

void PWM_on() {
	TCCR3A = (1 << COM0A0);
	// COM3A0: Toggle PB3 on compare match between counter and OCR0A
	TCCR3B = (1 << WGM02) | (1 << CS01) | (1 << CS00);
	// WGM02: When counter (TCNT0) matches OCR0A, reset counter
	// CS01 & CS30: Set a prescaler of 64
	set_PWM(0);
}

void PWM_off() {
	TCCR0A = 0x00;
	TCCR0B = 0x00;
}



enum SPEAKER_states {SPEAKER_start, off, one,two, three} SPEAKER_state;
int TickFct_SPEAKERState(int state)
{
//	unsigned short x = ADC;  // Value of ADC 10-bit register now stored in variable x.
	//unsigned short max = 0xFF;
	
	switch(SPEAKER_state)
	{ //transitions
		case SPEAKER_start:
		SPEAKER_state= off;
		break;
		
		case off: 
		if (A0){
			SPEAKER_state=one;
		}
		else if (A1){
			SPEAKER_state=two;
		}
		else if (A2){
			SPEAKER_state=three;
		}
		else{
					set_PWM(0);
			SPEAKER_state=off;
		}
		
		break;
		
		
		
		case one:
		if (A0){
			
		set_PWM(261.63);
		SPEAKER_state=one;
		
		}
		
		else{
		SPEAKER_state = off;
		}
		break;
		
		
		
		case two:
			if (A1){
					
				set_PWM(293.66);
				SPEAKER_state=two;
				
			}
			else{
				SPEAKER_state = off;
			}
		break;
		
		
		case three:
		
		if (A2){
			PWM_on();
			set_PWM(329.63);
			SPEAKER_state=three;
					
		}
				
				else{
					SPEAKER_state = off;
				}
		break;
		default:
		SPEAKER_state = SPEAKER_start;
		break;
	}
}



int main(void)
{
	
	
	
		DDRA = 0x00; PORTA = 0xFF; // Configure port A, B, and C's 8 pins as inputs
		DDRB = 0xFF; PORTB = 0x00;
//		DDRD = 0xFF; PORTD = 0x00;
		
		
		
	TimerSet(tasksPeriod);
	TimerOn();


	
	unsigned char i = 0;
	tasks[i].state = SPEAKER_start;
	tasks[i].period = 100;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &TickFct_SPEAKERState;
	PWM_on();
	
	while(1)
	{
	//	set_PWM(261);
		
		// 		while (!TimerFlag){}
		// 		TimerFlag = 0;
	}
	return 0;
    /* Replace with your application code */
  
}

