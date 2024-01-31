//All settings and include files will be here.

#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include <stdlib.h>

// LCD Configuration
#define F_CPU 16000000UL
#define LCD_DPRT PORTD
#define LCD_DDDR DDRD
#define LCD_CPRT PORTC
#define LCD_CDDR DDRC
#define LCD_RS 0
#define LCD_EN 1

// Keypad Configuration
#define KEY_PRT PORTB
#define KEY_DDR DDRB
#define KEY_PIN PINB

#define MAX_EXPRESSION_LENGTH 16