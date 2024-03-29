#ifndef LiquidCrystal_h
#define LiquidCrystal_h

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "sys_core.h"
#include "rti.h"
#include "gio.h"
#include "adc.h"
#include "het.h"

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// OJO CON LA POSICION
enum {
	LOW = 0,
	HIGH = 1
};
typedef struct {
	gioPORT_t * PORT;
	uint8_t BIT;
} GIO;

GIO _rs_pin; // LOW: command.  HIGH: character.
GIO _rw_pin; // LOW: write to LCD.  HIGH: read from LCD.
GIO _enable_pin; // activated by a HIGH pulse.
GIO _data_pins[8];

uint8_t _displayfunction;
uint8_t _displaycontrol;
uint8_t _displaymode;

uint8_t _initialized;

uint8_t _numlines;
uint8_t _row_offsets[4];

void LCD_init(uint8_t fourbitmode, GIO rs, GIO rw, GIO enable,
		GIO d0, GIO d1, GIO d2, GIO d3, GIO d4, GIO d5,
		GIO d6, GIO d7);

void LCD_begin(uint8_t cols, uint8_t rows, uint8_t charsize);

void LCD_clear();
void LCD_home();

void LCD_send(uint8_t, uint8_t);
void LCD_write4bits(uint8_t);
void LCD_write8bits(uint8_t);
void LCD_pulseEnable();

void LCD_noDisplay();
void LCD_display();
void LCD_noBlink();
void LCD_blink();
void LCD_noCursor();
void LCD_cursor();
void LCD_scrollDisplayLeft();
void LCD_scrollDisplayRight();
void LCD_leftToRight();
void LCD_rightToLeft();
void LCD_autoscroll();
void LCD_LCD_noAutoscroll();

void LCD_setRowOffsets(int row1, int row2, int row3, int row4);
void LCD_createChar(uint8_t, uint8_t[]);
void LCD_setCursor(uint8_t, uint8_t);
uint8_t LCD_write(uint8_t);
void LCD_command(uint8_t);
void LCD_printLn(char* text);

uint32_t __pulseCount_1us;

// |---> [Prototipos de funciones para el TIMER]
void rtiNotification(uint32 notification);
// <---| [Prototipos de funciones para el TIMER]

void delayMicroseconds(uint32_t timeRef_us);


void LCD_init(uint8_t fourbitmode, GIO rs, GIO rw, GIO enable,
		GIO d0, GIO d1, GIO d2, GIO d3, GIO d4, GIO d5,
		GIO d6, GIO d7) {

	// |---> [Inicializaciones varias]
	gioInit();
	rtiInit();
	adcInit();
	// <---| [Inicializaciones varias]

	// |---> [Inicializacion del TIMER]
	rtiEnableNotification(rtiNOTIFICATION_COMPARE0);
	rtiEnableNotification(rtiNOTIFICATION_COMPARE1);
	rtiStartCounter(rtiCOUNTER_BLOCK0);
	_enable_interrupt_();
	// <---| [Inicializacion del TIMER]


	_rs_pin = rs;

	_rw_pin = rw;
	_enable_pin = enable;

	_data_pins[0] = d0;
	_data_pins[1] = d1;
	_data_pins[2] = d2;
	_data_pins[3] = d3;
	_data_pins[4] = d4;
	_data_pins[5] = d5;
	_data_pins[6] = d6;
	_data_pins[7] = d7;

	if (fourbitmode)
		_displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
	else
		_displayfunction = LCD_8BITMODE | LCD_1LINE | LCD_5x8DOTS;
}
void LCD_begin(uint8_t cols, uint8_t lines, uint8_t dotsize) {
	if (lines > 1) {
		_displayfunction |= LCD_2LINE;
	}
	_numlines = lines;

	LCD_setRowOffsets(0x00, 0x40, 0x00 + cols, 0x40 + cols);

	// for some 1 line displays you can select a 10 pixel high font
	if ((dotsize != LCD_5x8DOTS) && (lines == 1)) {
		_displayfunction |= LCD_5x10DOTS;
	}

	// ---------------------------------------------------------> SE CONFIGURAN PINES DESDE HALGOGEN.
	//	pinMode(_rs_pin, OUTPUT);
	// we can save 1 pin by not using RW. Indicate by passing 255 instead of pin#
	//	if (_rw_pin != 255) {
	//		pinMode(_rw_pin, OUTPUT);
	//	}
	//	pinMode(_enable_pin, OUTPUT);

	// Do these once, instead of every time a character is drawn for speed reasons.
	//	for (int i=0; i<((_displayfunction & LCD_8BITMODE) ? 8 : 4); ++i) {
	//		pinMode(_data_pins[i], OUTPUT);
	//	}
	// ---------------------------------------------------------> SE CONFIGURAN PINES DESDE HALGOGEN.

	// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	// according to datasheet, we need at least 40ms after power rises above 2.7V
	// before sending commands. Arduino can turn on way before 4.5V so we'll wait 50
	delayMicroseconds(50000);
	// Now we pull both RS and R/W low to begin commands
	gioSetBit(_rs_pin.PORT, _rs_pin.BIT, LOW); // digitalWrite(_rs_pin, LOW);
	gioSetBit(_enable_pin.PORT, _enable_pin.BIT, LOW); // digitalWrite(_enable_pin, LOW);

	if (_rw_pin.BIT != 255) {
		gioSetBit(_rw_pin.PORT, _rw_pin.BIT, LOW); // digitalWrite(_rw_pin, LOW);
	}

	//put the LCD into 4 bit or 8 bit mode
	if (! (_displayfunction & LCD_8BITMODE)) {
		// this is according to the hitachi HD44780 datasheet
		// figure 24, pg 46

		// we start in 8bit mode, try to set 4 bit mode
		LCD_write4bits(0x03);
		delayMicroseconds(4500);// wait min 4.1ms

		// second try
		LCD_write4bits(0x03);
		delayMicroseconds(4500);// wait min 4.1ms

		// third go!
		LCD_write4bits(0x03);
		delayMicroseconds(150);

		// finally, set to 4-bit interface
		LCD_write4bits(0x02);
	} else {
		// this is according to the hitachi HD44780 datasheet
		// page 45 figure 23

		// Send function set command sequence
		LCD_command(LCD_FUNCTIONSET | _displayfunction);
		delayMicroseconds(4500);// wait more than 4.1ms

		// second try
		LCD_command(LCD_FUNCTIONSET | _displayfunction);
		delayMicroseconds(150);

		// third go
		LCD_command(LCD_FUNCTIONSET | _displayfunction);
	}

	// finally, set # lines, font size, etc.
	LCD_command(LCD_FUNCTIONSET | _displayfunction);

	// turn the display on with no cursor or blinking default
	_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	LCD_display();

	// clear it off
	LCD_clear();

	// Initialize to default text direction (for romance languages)
	_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	// set the entry mode
	LCD_command(LCD_ENTRYMODESET | _displaymode);}
void LCD_setRowOffsets(int row0, int row1, int row2, int row3) {
	_row_offsets[0] = row0;
	_row_offsets[1] = row1;
	_row_offsets[2] = row2;
	_row_offsets[3] = row3;
}

/********** high level commands, for the user! */
void LCD_clear(void) {
	LCD_command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
	delayMicroseconds(2000);// this command takes a long time!
}
void LCD_home(void) {
	LCD_command(LCD_RETURNHOME);  // set cursor position to zero
	delayMicroseconds(2000);// this command takes a long time!
}
void LCD_setCursor(uint8_t col, uint8_t row) {
	const size_t max_lines = sizeof(_row_offsets) / sizeof(*_row_offsets);
	if ( row >= max_lines ) {
		row = max_lines - 1;    // we count rows starting w/0
	}
	if ( row >= _numlines ) {
		row = _numlines - 1;    // we count rows starting w/0
	}

	LCD_command(LCD_SETDDRAMADDR | (col + _row_offsets[row]));
}

// Turn the display on/off (quickly)
void LCD_noDisplay(void) {
	_displaycontrol &= ~LCD_DISPLAYON;
	LCD_command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LCD_display(void) {
	_displaycontrol |= LCD_DISPLAYON;
	LCD_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void LCD_noCursor(void) {
	_displaycontrol &= ~LCD_CURSORON;
	LCD_command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LCD_cursor(void) {
	_displaycontrol |= LCD_CURSORON;
	LCD_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void LCD_noBlink(void) {
	_displaycontrol &= ~LCD_BLINKON;
	LCD_command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LCD_blink(void) {
	_displaycontrol |= LCD_BLINKON;
	LCD_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void LCD_scrollDisplayLeft(void) {
	LCD_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void LCD_scrollDisplayRight(void) {
	LCD_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void LCD_leftToRight(void) {
	_displaymode |= LCD_ENTRYLEFT;
	LCD_command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void LCD_rightToLeft(void) {
	_displaymode &= ~LCD_ENTRYLEFT;
	LCD_command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void LCD_autoscroll(void) {
	_displaymode |= LCD_ENTRYSHIFTINCREMENT;
	LCD_command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void LCD_noAutoscroll(void) {
	_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
	LCD_command(LCD_ENTRYMODESET | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void LCD_createChar(uint8_t location, uint8_t charmap[]) {
	int i = 0;

	location &= 0x7; // we only have 8 locations 0-7
	LCD_command(LCD_SETCGRAMADDR | (location << 3));
	for (i = 0; i < 8; i++) {
		LCD_write(charmap[i]);
	}
}

/*********** mid level commands, for sending data/cmds */
inline void LCD_command(uint8_t value) {
	LCD_send(value, LOW);
}
uint8_t LCD_write(uint8_t value) {
	LCD_send(value, HIGH);
	return 1; // assume sucess
}

/************ low level data pushing commands **********/
// write either command or data, with automatic 4/8-bit selection
void LCD_send(uint8_t value, uint8_t mode) {
	gioSetBit(_rs_pin.PORT, _rs_pin.BIT, mode); // digitalWrite(_rs_pin, mode);

	// if there is a RW pin indicated, set it low to Write
	if (_rw_pin.BIT != 255) {
		gioSetBit(_rw_pin.PORT, _rw_pin.BIT, LOW); // digitalWrite(_rw_pin, LOW);
	}

	if (_displayfunction & LCD_8BITMODE) {
		LCD_write4bits(value);
	} else {
		LCD_write4bits(value>>4);
		LCD_write4bits(value);
	}
}
void LCD_pulseEnable(void) {
	gioSetBit(_enable_pin.PORT, _enable_pin.BIT, LOW); // digitalWrite(_enable_pin, LOW);
	delayMicroseconds(1);
	gioSetBit(_enable_pin.PORT, _enable_pin.BIT, HIGH); // digitalWrite(_enable_pin, HIGH);
	delayMicroseconds(1);    // enable pulse must be >450ns
	gioSetBit(_enable_pin.PORT, _enable_pin.BIT, LOW); // digitalWrite(_enable_pin, LOW);
	delayMicroseconds(100);// commands need > 37us to settle
}
void LCD_write4bits(uint8_t value) {
	int i = 0;
	for (i = 0; i < 4; i++) {
		gioSetBit(_data_pins[i].PORT, _data_pins[i].BIT, (value >> i) & 0x01); // digitalWrite(_data_pins[i], (value >> i) & 0x01);
	}
	LCD_pulseEnable();
}
void LCD_write8bits(uint8_t value) {
	uint16_t i = 0;
	for (i = 0; i < 8; i++) {
		gioSetBit(_data_pins[i].PORT, _data_pins[i].BIT, (value >> i) & 0x01); // digitalWrite(_data_pins[i], (value >> i) & 0x01);
	}
	LCD_pulseEnable();
}

void LCD_printLn(char* text) {
	uint16_t i = 0;
	for (i = 0; i < strlen(text); i++) {
		LCD_write(text[i]);
	}
}


void delayMicroseconds(uint32_t timeRef_us) {
	rtiStartCounter(rtiNOTIFICATION_COMPARE0);
	__pulseCount_1us = 0;
	while (__pulseCount_1us < timeRef_us);
	//	rtiStopCounter(rtiNOTIFICATION_COMPARE0);
}

// [INCIO] DECLARA AQUI LAS VARIABLES EXTERNAS
//variables para ADC
adcData_t ADC_RAW_AD1[3];
int ch_count = 0;
int Activador_Alerta = 0;
int duty=0;
float val=2.44;
int Vmin=4000;
int porcentaje=0;
char msgStr_1[40];
// [FIN] DECLARA AQUI LAS VARIABLES EXTERNAS

// |---> [Funciones para el TIMER]
void rtiNotification(uint32 notification) {
	// Configurado cada 1us.
	if (notification == rtiNOTIFICATION_COMPARE0) {
		++__pulseCount_1us;
		// Configurado cada 10ms.
	} else if (notification == rtiNOTIFICATION_COMPARE1) {
		// Inserta aqui tu codigo que va dentro de la interrupcion.
		//		NumberOfChar = sprintf((char *) command, "Aleta1=%d   Aleta2=%d",ADC_RAW_AD1[0].value,ADC_RAW_AD1[1].value);
		//		sciSend(scilinREG, NumberOfChar, command);
		//		sciSend(scilinREG, 2, (unsigned char *) "\r\n");
		//2350--> 2500
		//2500-> 2800
		if (ADC_RAW_AD1[0].value <= 2500 && ADC_RAW_AD1[0].value >= 2000
				&& ADC_RAW_AD1[0].value <= 2500
				&& ADC_RAW_AD1[1].value >= 2000) {
			Activador_Alerta = 0;
		}

		if ((ADC_RAW_AD1[0].value > 2500 && ADC_RAW_AD1[0].value <= 2800
				&& ADC_RAW_AD1[1].value > 2500 && ADC_RAW_AD1[1].value <= 2800)
				|| (ADC_RAW_AD1[0].value > 1600 && ADC_RAW_AD1[0].value < 2000
						&& ADC_RAW_AD1[1].value > 1600
						&& ADC_RAW_AD1[1].value < 2000)) {
			Activador_Alerta = 1;


		} else {

			if (ADC_RAW_AD1[1].value > 2800 && ADC_RAW_AD1[0].value > 2800) {
				Activador_Alerta = 2;
			} else {
				if (ADC_RAW_AD1[1].value < 1600
						&& ADC_RAW_AD1[0].value < 1600) {
					Activador_Alerta = 3;
				} else {
					Activador_Alerta = 0;
				}
			}
		}

//		duty = (int) (ADC_RAW_AD1[2].value) * (val);
//		porcentaje=(int) (duty * 100) / 10000;
//		if (duty < Vmin) {
			//perdida de los motores y activacion de la luz warning
			//gioSetBit(gioPORTB, 0, 1);
//			LCD_setCursor(0, 0); // col: 0 - 15 | row: 0 - 1
//			sprintf(msgStr_1, "row_1! [%d]", porcentaje);
//			LCD_printLn(msgStr_1);


			//				LCD_setCursor(0, 1); // col: 0 - 15 | row: 0 - 1
			//				sprintf(msgStr_1, "row_2! [%d]", 123);
			//				LCD_printLn(msgStr_1);
// Limpia la pantalla.
//		} else {
			//gioSetBit(gioPORTB, 0, 0);
//			LCD_setCursor(0, 0); // col: 0 - 15 | row: 0 - 1
//			sprintf(msgStr_1, "asawe");
//			LCD_printLn(msgStr_1);

//		}

	}
}
// <---| [Funciones para el TIMER]

#endif
