/*
 * rjosh002_lab1_part3.c
 *
 * Created: 7/30/2018 9:44:34 PM
 * Author : ucrcse
 */ 

#include <avr/io.h>



int main(void)
{
    DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
    DDRC = 0xFF; PORTC = 0x00; // Configure port B's 8 pins as outputs,
    // initialize to 0s
    unsigned char tmpB = 0x00; 	// Temporary variable to hold the value of B
    unsigned char tmpA = 0x00; // Temporary variable to hold the value of A
    unsigned char cntavail = 0x00;
    unsigned char spac = 0x04;
    unsigned char occ = 0x00;

    while(1)
    {

        // (1) Read input
        tmpA = PINA & 0x0F;
        // (2) Perform computation
        // if PA0 is 1, set PB1PB0=01, else=10
        if (tmpA >= 0x01) { // True if PA0 is 1
            occ= (PINA & 0x01) + ((PINA >> 0x01) & 0x01) + ((PINA >> 0x02) & 0x01) + ((PINA >> 0x03) & 0x01);
            tmpB = (0x04 - occ); // Sets tmpB to bbbbbb01
            // (clear rightmost 2 bits, then set to 01)
        }
        else {









            tmpB = (tmpB & 0xFC) | 0x04; // Sets tmpB to 00
            // (clear rightmost 2 bits, then set to 10)
        }



if (occ== 4){
    tmpB =  0x80;
}
        // (3) Write output
        PORTC = tmpB;
    }
    return 0;
}
