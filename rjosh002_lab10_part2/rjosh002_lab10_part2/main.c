
#include <avr/io.h>
#include <avr/interrupt.h>
#include "bit.h"
#include <stdio.h>
#include "io.c"
#include "io.h"

#define size 1

unsigned char doggyStyle = 1;
const unsigned char barney[67]={' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','C','S','1','2','0','B',' ','i','s',' ','L','e','g','e','n','d','.','.','.','w','a','i','t',' ','f','o','r',' ','i','t',' ','D','A','R','Y','!',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '};

//const unsigned char lexi[27]={' ', ' ', ' ', 'L', 'E', 'X', 'I', ' ', 'I', 'S', ' ', 'A', ' ', 'G', 'O', 'O', 'D', ' ', 'P', 'U', 'P', 'P', 'E', 'R', '!', ' ', ' '};

//--------End find GCD function ----------------------------------------------

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;
unsigned long tasksPeriod = 300;
volatile unsigned long TimerFlag = 0;

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
	TimerFlag = 1;
	// unsigned char i;
	// for(i = 0; i < size; i++){
	// 	if(tasks[i].elapsedTime >= tasks[i].period){
	// 		tasks[i].state = tasks[i].TickFct(tasks[i].state);
	// 		tasks[i].elapsedTime = 0;
	// 	}
	// 	tasks[i].elapsedTime += tasksPeriod;
	// }
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

//--------User defined FSMs---------------------------------------------------
enum LCD_states {start} LCD_state;
int SMTick1(int state){
	unsigned char i = 0;
	switch(LCD_state){
		case start:
		for (i = 0; i < 16; i++)
		{
			PORTB = 0x1F; //removing when working
			LCD_Cursor(i);
			LCD_WriteData(barney[doggyStyle + i - 1]);
		}
		if(doggyStyle + i <= 67) //thrust on
		{
			doggyStyle++;
		}
		else //dinner for two instead ;)
		{
			doggyStyle = 1;
			PORTB = 0x00; //remove when working
		}
		LCD_state = start;
		break;
	}
}

int main(void)
{
DDRA = 0xFF; PORTA = 0x00;
DDRB = 0xFF; PORTB = 0x00;
DDRC = 0xF0; PORTC = 0x0F; // PC7..4 outputs init 0s, PC3..0 inputs init 1s
DDRD = 0xFF; PORTD = 0x00;
// Period for the tasks
unsigned long int SMTick1_calc = 400;


//Calculating GCD
unsigned long int tmpGCD = 1;

//Greatest common divisor for all tasks or smallest time unit for tasks.
unsigned long int GCD = tmpGCD;

//Recalculate GCD periods for scheduler
unsigned long int SMTick1_period = SMTick1_calc;

//Declare an array of tasks
static task task1;
task *tasks[] = { &task1};
const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

// Task 1
task1.state = 0;//Task initial state.
task1.period = SMTick1_period;//Task Period.
task1.elapsedTime = SMTick1_period;//Task current elapsed time.
task1.TickFct = &SMTick1;//Function pointer for the tick.


// Set the timer and turn it on
TimerSet(GCD);
TimerOn();
LCD_init();
LCD_ClearScreen();
unsigned short i; // Scheduler for-loop iterator
while(1) {
	// Scheduler code
	for ( i = 0; i < numTasks; i++ ) {
		// Task is ready to tick
		if ( tasks[i]->elapsedTime >= tasks[i]->period ) {
			// Setting next state for task
			tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
			// Reset the elapsed time for next tick.
			tasks[i]->elapsedTime = 0;
		}
		tasks[i]->elapsedTime += 1;
	}
	while(!TimerFlag);
	TimerFlag = 0;
}

// Error: Program should not exit!
return 0;
}

