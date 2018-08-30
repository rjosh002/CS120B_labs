/*
 * rjosh002_customproject.c
 *
 * Created: 8/22/2018 9:22:03 PM
 * Author : Rohit
 */ 

#include <avr/io.h>
#include <avr/eeprom.h>
//#include "matrix.h"
#include "timer.h"
#include "io.c"
#include "scheduler.h"
#include <avr/pgmspace.h>
#define C0 (~PINA & 0x01)
#define C1 (~PINA & 0x02)
#define C2 (~PINA & 0x04)
#define C3 (~PINA & 0x08)
#define columnsize 3
#define rowsize 3
#define enemiesrs 3
#define enemiescs 3
#define bulletsize 7

static unsigned char pos=0xFF;
unsigned char b=0;
unsigned char highscore=0;
unsigned char customChar[8] = {0x00, 0x04, 0x0E, 0x0E, 0x0E, 0x1F, 0x0E, 0x1B};
unsigned char buffer[15];
unsigned char gameover=0;
const unsigned char stockcolumns[3] = {0xEF, 0xF7, 0xFB} ;
const unsigned char stockrows[3] = {0x80, 0xC0, 0x80};
unsigned char columns[columnsize] = {0xEF, 0xF7, 0xFB} ;
unsigned char rows[rowsize] = {0x80, 0xC0, 0x80};
unsigned char bullet =0x00;
char bullets[bulletsize] = {0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
 unsigned char triggered=0;
  unsigned char u=0;

//char enemiesro[enemiesrs] = {0x01};
//char enemiesco[enemiescs] = {0xC3} ;
const char stockenemiesro[enemiesrs] = {0x01, 0x01, 0x01};
const char stockenemiesco[enemiescs] = {0xC3, 0x81, 0xC3} ;
char enemiesro[enemiesrs] = {0x01, 0x01, 0x01};
char enemiesco[enemiescs] = {0xC3, 0x81, 0xC3} ;
unsigned char enemyrows=1; 
unsigned char gameon=0;

void LCD_Custom(unsigned char loc, unsigned char *ptr)
{														
	unsigned char i;									//from online from www.microcontroller-project.com/making-custom-characters-on-16x2-lcd.html
	if(loc<8)
	{
		LCD_WriteCommand(0x40+(loc*8));
		for(i=0;i<8;i++)
		{
			LCD_WriteData(ptr[ i ]);
		}
	}
	
}
void transmit_data(unsigned char data) {
	int j;
	for (j = 0; j < 8 ; ++j) {
		// Sets SRCLR to 1 allowing data to be set
		// Also clears SRCLK in preparation of sending data
		PORTD = 0x08;
		// set SER = next bit of data to be sent.
		PORTD |= ((data >> j) & 0x01);
		// set SRCLK = 1. Rising edge shifts next bit of data into the shift register
		PORTD |= 0x02;   // change to 4!!!!!!!!!!!!!!!
	}
	// set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
	PORTD |= 0x04;            //change to 2!!!!!!!!!!
	// clears all lines in preparation of a new transmission
	PORTD = 0x00;
}
void transmit_a(unsigned char data) {
	
	int j;
	for (j = 7; j >= 0 ; j--) {
		// Sets SRCLR to 1 allowing data to be set
		// Also clears SRCLK in preparation of sending data
		PORTD = 0x80;
		// set SER = next bit of data to be sent.
		PORTD |= (((data >> j) & 0x01)<< 4);
		// set SRCLK = 1. Rising edge shifts next bit of data into the shift register
		PORTD |= 0x20;   // change to 4!!!!!!!!!!!!!!!
	}
	// set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
	PORTD |= 0x40;            //change to 2!!!!!!!!!!
	// clears all lines in preparation of a new transmission
	PORTD = 0x00;
}


void LCD_print (){
	
	LCD_DisplayString(1,buffer);
	
	if (gameover==1){
		
		
		LCD_DisplayString(1,buffer);
	}
	return;
}


 enum button_States {wait,left,right,reset} bstate;		//charachter movement machine
 int Button(int state) {
	 unsigned char i=0;
	unsigned char r=0;
	  switch (bstate) {
		 
		  
		case wait:
			
		 if (C0) {
			 bstate=left;
			 }
		else if(C1){
				 bstate=right;
			 }
		else if(C3){
			 	bstate=reset;
			 }
		else{
			bstate=wait;
			}
		break;
		
		
	  case left:
	  if(C0 && columns[0] != 0x7F ){

		  for (i=0; i<columnsize;i++){
			  
			 columns[i] = (columns[i]<< 1) | 0x01;
			  
		  }
	  }
	  i=0;
	  bstate=wait;
	  break;
		
		
		
	 case right:
	 if(C1 && columns[columnsize-1] != 0xFE )
		{
	 	 for (i=0; i<columnsize;i++)
			{
		 	 columns[i] = (columns[i]>> 1) | 0x80;
	 		 }
		 }
		 i=0;
		bstate=wait;
	 break;
	 
	 case reset:
	 if (C3){
		 for (r=0; r<3; r++){
		 columns[r]=stockcolumns[r];
		rows[r]=stockrows[r];
		enemiesro[r]=stockenemiesro[r];
		enemiesco[r]=stockenemiesco[r];
		 }
		 enemyrows=1; 
		 gameover=0;
		 b=0;
		 pos=0xFF;
		 gameon=1;
		 
	 }
	 bstate=wait;
	 
	 
	 
	 
	 
	 break;
	  default:
	  bstate=wait;
	  
	  break;
	 
	  }
 }

 enum SM1_States {sm1_display, sm3_display, sm4_display} state;  //display state machine
 int SMTick1(int state) {																						
	 
	
	
	 // === Local Variables ===

	static unsigned char i=0;
	static unsigned char which=0;
		static unsigned char wich=0;
	static unsigned char charit=0 ;
	static unsigned char blip ;
		static unsigned char rich ;
		static unsigned char mip;
		static unsigned char stip;
	static unsigned char grip;
	
	 
	 // === Actions ===
if (gameon==1){
	 switch (state) {
		  //unsigned char i=0;
		 case sm1_display:   //populates character
			transmit_a(0x00);
			transmit_data(0xFF);
			
		
			
			  
			
			
			
			

			if(i<rowsize){
			

				//PORTD=0xFF;
			transmit_a(rows[i]); // PORTA displays column pattern
			 PORTB = columns[i];  // PORTB selects column to display patterns
			 //	 PORTD=0xFE;
				 // PORTA = 0x01; // PORTA displays column pattern
			// PORTD=0x7F;
				 
			//	  transmit_data(column_sel);
				  i++;
				 	state=sm1_display;
				
			}
			
			else{
			i=0;
			state=sm3_display;

			}

		break;
		 
		  case sm3_display:   //populates enemies
				//static unsigned char enemypos = 0xFF;
			 transmit_a(0x00);
			 PORTB = 0xFF;
			 blip=enemiesco[i];
		
			if   ((((unsigned char)~blip) & ((unsigned char)~pos))   ){
				which=i;
				
		 
			if(enemiesro[which] == bullet){
					
						enemiesco[which] = enemiesco[which] | ((unsigned char)~pos);
					//
						//	 transmit_data(0x00);
					b=b+ 10;
					triggered=0;
					u=0;
					pos=0xFF;
					bullet=0x00;
					state=sm3_display;
					
			}
				}
		
				
// 		for (charit=0;charit<2;charit++){
// 			mip=columns[charit];
// 			if ( ((unsigned char)~blip) & ((unsigned char)~mip) ){
// 				rich=charit;
// 					grip=rows[i];
// 					stip=enemiesro[rich];
// 				if ((unsigned char)grip & (unsigned char)stip) {
// 				PORTB=0x00;
// 				PORTA= 0xFF;
// 				transmit_data(0x00);
// 				}
// 			
// 			}
// 				
// 			charit++;
// 			}
// 		

			if((enemiesco[i] != 0xFF) && (enemiesro[i]== 0x80) ){						//date
				
				gameover=1;
				PORTB=0x00;
								transmit_a(0xFF);
										transmit_data(0x00);
				
			}
// 		
			
		 
 			 else if(i<enemyrows)
			 {
			 transmit_a(enemiesro[i]); // PORTA displays column pattern
			//PORTD =  enemiesco[i];
			 transmit_data(enemiesco[i]);
			 i++;
			state=sm3_display;
			}
		
			else{
			i=0;
			state=sm4_display;
			}
		
		 break;
		 
		
		 case sm4_display:		//populates bullets
			 transmit_a(0x00);
			 PORTB = 0xFF;
		 
			 transmit_data(0xFF);
		
			 transmit_a(bullet);
			 PORTB= pos;
			
			highscore= eeprom_read_byte((uint8_t*)1);
			 if ( highscore < b ){
			 eeprom_write_byte ((uint8_t*)1, b);
			 highscore=b;
			 
			 }
			 
			 
			 
			 
			 
			 
			 
			 
			 state=sm1_display;
		
		 
		 
		 
		 break;
		 
		 
		 
		 default:
		 state=sm1_display;
		 
   	        break;
	 
	  
	 }
}
	 return state;
 };




 enum SM2_States {sm2_display} cstate;       //enemy populator machine
 int SMTick2(int state) {

	 // === Local Variables ===
	// volatile static unsigned char column_val; // sets the pattern displayed on columns
	// volatile static unsigned char column_sel; // grounds column to display pattern
	 //static unsigned char column_sidel = 0xB7;
	 static unsigned char c=0;
	 
	 static unsigned char first=0;
	 
	 // === Actions ===

	 switch (cstate) {
		 //unsigned char i=0;
		 case sm2_display:   // If illuminated LED in bottom right corner
		 
	
		 if (first==1){
			
			 	
		for (c=0; c<enemyrows; c++)
		{
			enemiesro[c] = (enemiesro[c]<< 1);
			
			
		}
			
		 	if(enemyrows<enemiesrs){
			 	
		 	enemyrows++;}
		 
			
			
		}
		
		
		 else{
			 first=1;
		 }
		 
		 
		 break;
		 
		 default:
		 cstate=sm2_display;
		 
		 break;
	 }
	 
	 
	 return state;
 };



enum ShootTick_States {shoot} shootstate;		//shoot state machine
 int ShootTick(int state) {

	 // === Local Variables ===
	// volatile static unsigned char column_val; // sets the pattern displayed on columns
	// volatile static unsigned char column_sel; // grounds column to display pattern
	 //static unsigned char column_sidel = 0xB7;
	
	 // === Actions ===

	 switch (shootstate) {
		 
		
		// unsigned char waiter=0;
		 case shoot:   // If illuminated LED in bottom right corner
		 
		if(C2 && pos== 0xFF){
			pos= columns[1];
			triggered=1;
		}
		if(triggered==1){
			
			if(u<bulletsize){
				
				bullet= bullets[u];
				u++;
				shootstate=shoot;
				
			}
			
			else{
				u=0;
				pos=0xFF;
				bullet=0x00;
				triggered=0;
				shootstate=shoot;
			}
				
		}
				
				
	
	
		
		 break;
		 
		 default:
		 shootstate=shoot;
		 
		 break;
	 }
	 
	 
	 return shootstate;
 };



enum LCDTick_States {out} lcdstate;		//LCD state machine
 LCDTick(int state) {



	//string store;
	 static unsigned char v=0;
//	store=b;
	
	switch (lcdstate) {
		
    char buffer[15];
		case out:   
		
		
		if (gameon==0){
			
			LCD_DisplayString(1,"Welcome! Press  Start to Begin!");
			
			
		}
		if (gameon==1){
		sprintf(buffer, "Score:%d", b);
 LCD_DisplayString(1,buffer);
	//	LCD_DisplayString(1,buffer);
	//	LCD_Cursor(7);
	//	LCD_DisplayString (7,b);
		state=out;
		}
		
		if (b==140){
			
			LCD_DisplayString(1,"YOU WIN!YOU HAVE HIGH SCORE");
			
		}
		
		
		if (gameover==1){
			
			sprintf(buffer, "High Score:%d ", highscore);
	
		
		LCD_DisplayString(1,"GAME OVER!!");
		LCD_Cursor(13);
		LCD_WriteData(1);
		LCD_noDisplayString(17,buffer);
		//LCD_Cursor(16);
		
//	LCD_WriteData(1);
			
		state=out;
		}
		
		
		
		break;
		
		default:
		lcdstate=out;
		
		break;
	}
	
	
	return lcdstate;
};






int main(void)
{
	
	
	DDRA = 0xF0; PORTA = 0x0F;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	

    /* Replace with your application code */
  unsigned long int SMTick1_period = 1;
    unsigned long int enemypop_period = 1010;
  unsigned long int button_period = 50;
     unsigned long int Shoot_period = 100;
	     unsigned long int lcd_period = 1000;

  //Greatest common divisor for all tasks or smallest time unit for tasks.
  unsigned long int GCD = 1;

  //Declare an array of tasks
  LCD_Custom(1, customChar);
  	  LCD_init();
  transmit_a(0x00);
  transmit_data(0xFF);
  PORTB= 0xFF;

TimerSet(GCD);
TimerOn();
  // transmit_data(shift);
  	
  // Task 1
	task1.state = 0;//Task initial state.
	task1.period = SMTick1_period;//Task Period.
	task1.elapsedTime = SMTick1_period;//Task current elapsed time.
	task1.TickFct = &SMTick1;//Function pointer for the tick.


	task2.state = 0;//Task initial state.
	task2.period = button_period;//Task Period.
	task2.elapsedTime = button_period;//Task current elapsed time.
	task2.TickFct = &Button;//Function pointer for the tick.


	task3.state = 0;//Task initial state.
	task3.period =  enemypop_period;//Task Period.
	task3.elapsedTime =  enemypop_period;//Task current elapsed time.
	task3.TickFct = &SMTick2;//Function pointer for the tick.


	task4.state = 0;//Task initial state.
	task4.period =  Shoot_period;//Task Period.
	task4.elapsedTime =  Shoot_period;//Task current elapsed time.
	task4.TickFct = &ShootTick;//Function pointer for the tick.

	task5.state = 0;//Task initial state.
	task5.period =  lcd_period;//Task Period.
	task5.elapsedTime =  lcd_period;//Task current elapsed time.
	task5.TickFct = &LCDTick;//Function pointer for the tick.
	

	
	while(1) {
		
	
		}
 
	
  
  return 0;
}

