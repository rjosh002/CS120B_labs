/*
 * rjosh002_lab3_part1.c
 *
 * Created: 8/2/2018 12:50:11 PM
 * Author : Rohit
 */ 
#include <avr/io.h>
unsigned char counter = 7;

enum states {init, start, ywaiter,pcheck, wait, lockerst} state;
void tick();

int main(void)
{
  DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
  DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as outputs,
    while (1){
        tick();
    }
}
    void tick(){
	
        unsigned char x = PINA & 0x01; //bitmask for the flag bit
		unsigned char why = PINA & 0x02; //bitmask for the flag bit
	   unsigned char pound = PINA & 0x04; //bitmask for the flag bit
		unsigned char locker = PINA & 0x80; //bitmask for the flag bit
			   unsigned char waiter = PINA & 0x07; //bitmask for the flag bit


        switch (state){

            case init:
            
			PORTB = 0;
			state = start;
            break;


            case start:


			if (pound && !why && !x){
			state=pcheck;
			}
			
			if (locker){
			state=locker;
			}

            break;
			


		case wait:
		state= waiter? wait: start;
		break;





		case pcheck:
		if (pound && !why && !x){
	//	 PORTB=2;
		state= ywaiter;

		}

		case ywaiter:


	//	PORTB=3;
		if (pound && !why && !x){
	//		PORTB=3;
    		state=ywaiter;
		}

		else if (!pound && !why && !x && !locker){
	   state=ywaiter;
		}
		else if (!pound && why && !x){
    			PORTB=1;
			}

		else{
	//	PORTB= 9;
		state=start;
		}
		break;



		
			
         state= wait;
            break;




            case lockerst:
            
            PORTB = 0;
            state = wait;
            break;


            default:
            state = init;
            break;


        }

    }





