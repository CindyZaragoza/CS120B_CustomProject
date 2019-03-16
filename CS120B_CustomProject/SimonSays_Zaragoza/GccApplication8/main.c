/*
 * GccApplication8.c
 *
 * Created: 3/14/2019 12:53:13 PM
 * Author : CindyZaragoza
 */ 

#include <avr/io.h>
#include "io.c"
#include "io.h"
#include "timer.h"
#include "bit.h"
#include <avr/interrupt.h>
#include <stdio.h>

#include <avr/eeprom.h>
unsigned char flash = 0;
unsigned char highscore_total = 0; //HIGHSCORE COUNTER
uint8_t final_highscore; // EEPROM uint8_t

//Variables
unsigned char start_l1 = 0;
unsigned char start_l2 = 0;

unsigned char end_l1 = 0;
unsigned char choose_level_gl = 0;
unsigned char level = 1;
unsigned char highscore_gl = 0;

//Display Strings on LCD
unsigned char * menu_string = "1.Start 2.Choose3.Scores  Level";
unsigned char * start_string = "Begin Game!";
unsigned char * win_string = "You Win!        Congrats!";
unsigned char * wrong_string = "Sorry, Incorrect";

unsigned char * key_press1 = "Enter Key Press ";
unsigned char * key_press2 = "Enter Key Press ";
unsigned char * key_press3 = "Enter Key Press ";


unsigned char customChar[8] = {0b01110,0b01110,0b01110,0b00100,0b10101,0b00100,0b01010,0b01010};

//8X8 matrix display pattern description
unsigned char bc_top = 0xFF;
unsigned char b_bot = 0x0F;


unsigned char b_top_left = 0x0F;
unsigned char c_top_left = 0x0F;

unsigned char b_top_right = 0x0F;
unsigned char c_top_right = 0xF0;

unsigned char b_bot_left = 0xF0;
unsigned char c_bot_left = 0x0F;

unsigned char b_bot_right = 0xF0;
unsigned char c_bot_right = 0xF0;

//Global variables
unsigned char button1 = 0; 
unsigned char button2 = 0; 
unsigned char button3 = 0; 
unsigned char button4 = 0; 
unsigned char button5 = 0; 

enum Menu{init, begin, choose_level, highscore, wait}state; 
void Menu_TickFct(){//transitions
	
	start_l1=0;
	start_l2=0;
	
	button1 = ~PINA & 0X04;
	button2 = ~PINA & 0x08;
	button3 = ~PINA & 0X10;
	button4 = ~PINA & 0X20;
	button5 = ~PINA & 0X40;
	switch (state){
		case init:
		if(button5){
			state = init;
		}
		else if (button1 && !end_l1){
			state = begin;
		}
		else if (button2 && !end_l1){
			state  = choose_level;
			LCD_ClearScreen();
			LCD_Cursor(1);
			LCD_DisplayString(1, start_string);
		}
		else if (button3 && !end_l1){
			state = highscore;
		}
		else {
			state = init;
		}
		break;
		
		
		case begin:
		state = wait;
		break;
		
		
		case choose_level:
		if (button5){
			state = init;
		}
		else if(button1 && (level <2)){
			level++;
		}
		else if(button2 && (level > 1)){
			level--;
		}
		else if (button3){
			state = init;
		}
		break;
		
		
		case highscore:
		if (button5){
			state = init;
		}
		else{
			state = highscore;
		}
		break;
		
		
		default:
		state = init;
		break;
	}
	switch (state){ //actions
		case init:
		TimerSet(300); //300
		LCD_ClearScreen();
		LCD_Cursor(1);
		LCD_DisplayString(1, menu_string);
		LCD_WriteData(0);
		break;
		
		
		case begin:
		if (level == 1){
			start_l1 = 1;
		}
		else if (level == 2){
			start_l2 = 1;
		}
		break;
		
		case choose_level:
		LCD_ClearScreen();
		LCD_Cursor(1);
		LCD_WriteData(level + '0');
		break;
		
		-
		
		case highscore:
		LCD_ClearScreen();
		LCD_Cursor(1);
		uint8_t byteRead = eeprom_read_byte((uint8_t*)46);
		LCD_WriteData(final_highscore +'0');
		break;
		
		
		
		default:
		break;
		
	}
}
enum level1_SM {init1, play, display, wrong, win} state1;
void level1_TickFct(){//transitions
		button1 = ~PINA & 0x04;
		button2 = ~PINA & 0x08;
		button3 = ~PINA & 0x10;
		button4 = ~PINA & 0x20;
		button5 = ~PINA & 0x40;
		
		switch (state1){
			case init1:
			end_l1 = 0;
			if (!start_l1){
				state1 = init1;
			}
			else if (start_l1){
				highscore_total = 0;
				state1 = play;
				end_l1 = 1;
			}
			break;
			
			case play:
			state1 = display;
			break;
			
			case display:
			if (button1){
				highscore_total++;
				if (highscore_total >= final_highscore){
					final_highscore = highscore_total;
				}
				state1 = win;
			}
			else if (button2 || button3 || button4){
				if (highscore_total >= final_highscore){
					final_highscore = highscore_total;
				}
				state1 = wrong;
			}
			else if (button5){
				state1 = init1;
			}
			break;
			
			case wrong:
			state1 = init1;
			break;
			
			case win:
			state1 = init1;
			break;
			
		}
		
		switch(state1){//actions
			case init1:
			TimerSet(1000);
			start_l1 = 0;
			end_l1 = 0;
			break;
			
			case play:
			TimerSet(2000);
			
			DDRB = 0xFF; PORTB = 0x0F;
			DDRC = 0xFF; PORTC = 0x0F;
			LCD_ClearScreen();
			LCD_Cursor(1);
			LCD_DisplayString(1, start_string);
			break;
			
			case display:
			TimerSet(1000);
			
			DDRB = 0xFF; PORTB = 0xFF;
			DDRC = 0xFF; PORTC = 0x00;
			LCD_ClearScreen();
			LCD_Cursor(1);
			LCD_DisplayString(1, key_press1);
			break;
			
			case wrong:
			eeprom_write_byte((uint8_t*)46, final_highscore);
			TimerSet(1000);
			LCD_ClearScreen();
			LCD_Cursor(1);
			LCD_DisplayString(1, wrong_string);
			LCD_WriteData(0);
			break;
			
			case win:
		eeprom_write_byte((uint8_t*)46, final_highscore);
			TimerSet(1000);
			LCD_ClearScreen();
			LCD_Cursor(1);
			LCD_DisplayString(1, win_string);
			LCD_WriteData(0);
			break;
			
		}
	}
	
	enum level2_SM {init2, play1, play2, display1, display2, wrong2, win2} state2;
	void level2_TickFct(){//transitions
		button1 = ~PINA & 0x04;
		button2 = ~PINA & 0x08;
		button3 = ~PINA & 0x10;
		button4 = ~PINA & 0x20;
		button5 = ~PINA & 0x40;
		switch (state2){
			case init2:
			end_l1 = 1;
			if (!start_l2){
				state2 = init2;
			}
			else if (start_l2){
				highscore_total = 0;
				state2 = play1;
				end_l1 = 1;
			}
			break;
	
			case play1:
			state2 = play2;
			break;
			
			case play2:
			state2 = display1;
			break;
			
			case display1:
			if (button1){
				highscore_total++;
				if (highscore_total >= final_highscore){
					final_highscore = highscore_total;
				}
				state2 = display2;
			}
			else if (button2 || button3 || button4){
				if (highscore_total >= final_highscore){
					final_highscore = highscore_total;
				}
				state2 = wrong2;
			}
			else if (button5){
				state2 = init2;
			}
			break;
			
			case display2:
			if (button4){
				highscore_total++;
				if (highscore_total >= final_highscore){
					final_highscore = highscore_total;
				}
				state2 = win2;
			}
			else if (button2 || button3 || button1){
				if (highscore_total >= final_highscore){
					final_highscore = highscore_total;
				}
				state2 = wrong2;
			}
			else if (button5){
				state2 = init2;
			}
			break;
			
			case wrong2:
			state2 = init2;
			break;
			
			case win2:
			state2 = init2;
			break;
		}
		
		switch(state2){//actions
			case init2:
			start_l2 = 0;
			end_l1 = 0;
			break;
			
			case play1:
			TimerSet(1500);
			
			DDRB = 0xFF; PORTB = 0x0F;
			DDRC = 0xFF; PORTC = 0x0F;
			LCD_ClearScreen();
			LCD_Cursor(1);
			LCD_DisplayString(1, start_string);
			break;
			
			case play2:
			
			DDRB = 0x0F; PORTB = 0xF0;
			DDRC = 0xFF; PORTC = 0xF0;//0xf0
			LCD_ClearScreen();
			LCD_Cursor(1);
			LCD_DisplayString(1, start_string);
			break;
			
			case display1:
			TimerSet(1000);
			
			DDRB = 0xFF; PORTB = 0xFF;
			DDRC = 0xFF; PORTC = 0x00;
			LCD_ClearScreen();
			LCD_Cursor(1);
			LCD_DisplayString(1, key_press1);
			break;
			
			case display2:
			LCD_ClearScreen();
			LCD_Cursor(1);
			LCD_DisplayString(1, key_press2);
			break;
		
			case wrong2:
			TimerSet(1000);
			eeprom_write_byte((uint8_t*)46, final_highscore);
			LCD_ClearScreen();
			LCD_Cursor(1);
			LCD_DisplayString(1, wrong_string);
			LCD_WriteData(0);
			break;
			
			case win2:
			TimerSet(1000);
		eeprom_write_byte((uint8_t*)46, final_highscore);
			LCD_ClearScreen();
			LCD_Cursor(1);
			LCD_DisplayString(1, win_string);
			LCD_WriteData(0);
			break;
		}
	}
	

int main(void){ 
	
	DDRA = 0x03; PORTA = 0xFC;
	DDRD = 0xFF; PORTD = 0x00;
	DDRB = 0xFF; PORTB = 0xFF; 
	DDRC = 0xFF; PORTC = 0x00;
	
	//CUSTOM CHARACTER
	LCD_WriteCommand(0x40);
	for(unsigned char i=0; i<8; i++){
		LCD_WriteData(customChar[i]);
	}
	//LCD_DisplayString(1,"Hello");
	LCD_WriteCommand(0x80); //put back to normal
	
	//EE PROM
	 eeprom_write_byte((uint8_t*)46,0);
	 
	 eeprom_read_byte((uint8_t*)46);
	
	TimerSet(500);
	TimerOn();
	state = begin;
	state1 = init1;
	
	LCD_init();
	LCD_ClearScreen();
	LCD_Cursor(1);
	while(1){
		Menu_TickFct();
		level1_TickFct();
		level2_TickFct();
		while(!TimerFlag){}
		TimerFlag = 0;
	}
}

