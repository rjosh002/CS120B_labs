/*
 * rjosh002_lab3_part1.c
 *
 * Created: 8/2/2018 12:50:11 PM
 * Author : Rohit
 */ 
#include <avr/io.h>
unsigned char counter = 7;
enum states {init, start, wait, increment, decrement, reset} state;
void tick();

int main(void)
{
  DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
  DDRC = 0xFF; PORTC = 0x00; // Configure port B's 8 pins as outputs,
    while (1){
        tick();
    }
}
    void tick(){

        unsigned char A0 = PINA & 0x01; //bitmask for the flag bit
		unsigned char A1 = PINA & 0x02; //bitmask for the flag bit
	   unsigned char waiter = PINA & 0x03; //bitmask for the flag bit
        switch (state){

            case init:
            counter=7;
			PORTC = counter;
			state = start;
            break;


            case start:
			if (A0 && !A1) {
			state=increment;
}
		
            else if (A1 && !A0){
			state=decrement;
}
			else if (A1 && A0){
			state=reset;
}
			else{
			state=start;
}
            break;
			
			case wait:
			state= waiter? wait: start;
			break;





            case increment:
			if (counter <=8){
			counter = counter + 1;}
			PORTC = counter;
            state = wait;
            break;


            case decrement:
			if (counter >=1){
           	counter = counter - 1;}
           	PORTC = counter;
           	state = wait;
            break;


            case reset:
            counter = 0;
            PORTC = counter;
            state = wait;
            break;


            default:
            state = init;
            break;


        }

    }





