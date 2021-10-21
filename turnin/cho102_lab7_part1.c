/*	Author: Cindy Ho
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #7  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum States {Start, Init, A0_Press, A0_Wait, A1_Press, A1_Wait, Both_Press, Both_Wait} state;

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn() {
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff() {
	TCCR1B = 0x00;
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
} 

unsigned char counter;
void Tick() {
	switch(state) {
		case Start:
			PORTB = 0x07;
			state = Init;
			break;
		case Init:
			counter = 0;
			if ((~PINA & 0x03) == 0x01) { 
				state = A0_Press; 
			}
			else if ((~PINA & 0x03) == 0x02) { 
				state = A1_Press; 
			}
			else if ((~PINA & 0x03) == 0x03) { 
				state = Both_Press; 
			}
			else { 
				state = Init; 
			}
			break;
		case A0_Press:
			counter = 0;
			state = A0_Wait;
			break;
		case A0_Wait:
			++counter;
			if ((~PINA & 0x03) == 0x00) { 
				state = Init; 
			}
			else if ((~PINA & 0x03) == 0x03) { 
				state = Both_Press; 
			}
			else if (((~PINA & 0x03) == 0x01) && counter > 10) { 
				state = A0_Press; 
			}
			else { 
				state = A0_Wait; 
			}
			break;
		case A1_Press:
			counter = 0;
			state = A1_Wait;
			break;
		case A1_Wait:
			++counter;
			if ((~PINA & 0x03) == 0x00) { 
				state = Init; 
			}
			else if ((~PINA & 0x03) == 0x03) { 
				state = Both_Press; 
			}
			else if (((~PINA & 0x03) == 0x02) && counter > 10) { 
				state = A1_Press; 
			}
			else { 
				state = A1_Wait; 
			}
			break;
		case Both_Press:
			state = Both_Wait;
			break;
		case Both_Wait:
			if ((~PINA & 0x03) == 0x00) { 
				state = Init; 
			}
			else { 
				state = Both_Wait; 
			}
			break;
		default:
			break;
	}
	switch(state) {
		case Start:			
			break;
		case Init:
			break;
		case A0_Press:
			
			if (PINB < 9) { 
				PORTB = PINB + 1; 
			}
			LCD_ClearScreen();
			LCD_Cursor(1);
			LCD_WriteData(PINB+'0');
			break;
		case A0_Wait:
			break;
		case A1_Press:
	
			if (PINB > 0) { 
				PORTB = PINB - 1; 
			}
			LCD_ClearScreen();
			LCD_Cursor(1);
			LCD_WriteData(PINB+'0');
			break;
		case A1_Wait:	
			break;
		case Both_Press:
			PORTB = 0x00;
			LCD_ClearScreen();
			LCD_Cursor(1);
			LCD_WriteData(PINB+'0');
			break;
		case Both_Wait:
			break;
		default:
			break;
	}
}


int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	state = Start;
	LCD_init();
    /* Insert your solution below */
    TimerSet(100);
    TimerOn();
    while (1) {
	Tick();
	while(!TimerFlag) {}
	TimerFlag =0;
    }
    return 1;
}

