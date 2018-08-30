 
 // ====================
 // SM1: DEMO LED matrix
 // ====================
 enum SM1_States {sm1_display};
 int SM1_Tick(int state) {

	 // === Local Variables ===
	 static unsigned char column_val = 0x01; // sets the pattern displayed on columns
	 static unsigned char column_sel = 0x7F; // grounds column to display pattern
	 
	 // === Transitions ===
	 switch (state) {
		 case sm1_display:    break;
		 default:   	        state = sm1_display;
		 break;
	 }
	 
	 // === Actions ===
	 switch (state) {
		 case sm1_display:   // If illuminated LED in bottom right corner
		 if (column_sel == 0xFE && column_val == 0x80) {
			 column_sel = 0x7F; // display far left column
			 column_val = 0x01; // pattern illuminates top row
		 }
		 // else if far right column was last to display (grounded)
		 else if (column_sel == 0xFE) {
			 column_sel = 0x7F; // resets display column to far left column
			 column_val = column_val << 1; // shift down illuminated LED one row
		 }
		 // else Shift displayed column one to the right
		 else {
			 column_sel = (column_sel >> 1) | 0x80;
		 }
		 break;
		 default:   	        break;
	 }
	 
	 PORTA = column_val; // PORTA displays column pattern
	 PORTB = column_sel; // PORTB selects column to display pattern

	 return state;
 };
