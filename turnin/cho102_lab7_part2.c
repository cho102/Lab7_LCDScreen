/*	Author: Cindy Ho
 *  Partner(s) Name: 
 *	Lab Section: 23
 *	Assignment: Lab #7  Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include "io.h"
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum States{Start, B0_On, B1_On, B2_On, Button_Press, Button_Wait} state;

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

unsigned char temp;
unsigned char tmpD;
void Tick() {
	switch(state) { //transition
		case Start:
			tmpD = 0x05;
			state = B0_On;
			LCD_ClearScreen();
			LCD_Cursor(1);
			LCD_WriteData(tmpD +'0');
			break;
		case B0_On:
			if ((~PINA & 0x01) == 0x01) {
				state = Button_Press;
			}
			else {
				state = B1_On;
			}
			break;
		case B1_On:
			if ((~PINA & 0x01) == 0x01) {
				state = Button_Press;
			}
			else {
				if((temp & 0x04) == 0x04) {
					state=B0_On;
				}
				else {
					state = B2_On;
				}
			}
			break;
		case B2_On:
			if ((~PINA & 0x01) == 0x01) {
				state = Button_Press;
			}
			else {
				state = B1_On;
			}
			break;
		case Button_Press:
			if ((~PINA & 0x01) == 0x01) {
				state = Button_Press;
			}
			else {
				state = Button_Wait;
			}
			break;
		case Button_Wait:
			if ((~PINA & 0x01) == 0x01) {
				if (tmpD == 0x09) {
					state = Start;
				}
				else {
					state = B0_On;
				}
			}
			else {
				state = Button_Wait;
			}
			break;
		default:
			state = Start;
			break;
	}
	switch(state){ //state
		case Start:
			temp = 0;
			break;
		case B0_On:
			PORTB = 1;
			temp = 1;
			break;
		case B1_On:
			PORTB = 2;
			break;
		case B2_On:
			PORTB = 4;
			temp = 4;
			break;
		case Button_Press:
			if (((PINB & 0x07) == 0x02) && (tmpD < 0x09)) {
				tmpD++;
				if (tmpD == 0x09) {
					LCD_DisplayString(1, "VICTORY!");
				}
				else {
					LCD_ClearScreen();
					LCD_Cursor(1);
					LCD_WriteData(tmpD +'0');
				}
			}
			else if (((PINB & 0x07) != 0x02) && (tmpD > 0)) {
				--tmpD;
				LCD_ClearScreen();
				LCD_Cursor(1);
				LCD_WriteData(tmpD +'0');
			}
			break;
		case Button_Wait:
			PORTB = PINB;
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
	temp = 0;
    /* Insert your solution below */
    TimerSet(300);
    TimerOn();
    LCD_init();
    while (1) {
	Tick();
	while(!TimerFlag) {}
	TimerFlag =0;
    }
    return 1;
}

