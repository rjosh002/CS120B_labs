/*
 * rjosh002_lab3_part1.c
 *
 * Created: 8/2/2018 12:50:11 PM
 * Author : Rohit
 */ 
#include <avr/io.h>

enum states {start, off, on, waitoff, waiton} state;
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

        unsigned char A0 = PINA & 0x01; //bitmask for the flag bit
        switch (state){

            case start:
            state=off;
            break;


            case off:
            PORTB = 0x01;
            state = A0? off : waitoff;
            break;
            case on:
            PORTB = 0x02;
            state = A0? on : waiton;
            break;
            case waitoff:
            PORTB = 0x01;
            state = A0? on : waitoff;
            break;
            case waiton:
            PORTB = 0x02;
            state = A0? off : waiton;
            break;
            default:
            state = off;
            break;


        }

    }





