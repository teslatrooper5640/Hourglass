/*
 *    LedControl.h - A library for controling Leds with a MAX7219/MAX7221
 *    Copyright (c) 2007 Eberhard Fahle
 *
 *    Permission is hereby granted, free of charge, to any person
 *    obtaining a copy of this software and associated documentation
 *    files (the "Software"), to deal in the Software without
 *    restriction, including without limitation the rights to use,
 *    copy, modify, merge, publish, distribute, sublicense, and/or sell
 *    copies of the Software, and to permit persons to whom the
 *    Software is furnished to do so, subject to the following
 *    conditions:
 *
 *    This permission notice shall be included in all copies or
 *    substantial portions of the Software.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *    OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef LedControl_h
#define LedControl_h
#include "stm32f10x.h"
#include "string.h"
#include "spi.h"
#include "mydelay.h"

//the opcodes for the MAX7221 and MAX7219
#define OP_NOOP   0
#define OP_DIGIT0 1
#define OP_DIGIT1 2
#define OP_DIGIT2 3
#define OP_DIGIT3 4
#define OP_DIGIT4 5
#define OP_DIGIT5 6
#define OP_DIGIT6 7
#define OP_DIGIT7 8
#define OP_DECODEMODE  9
#define OP_INTENSITY   10
#define OP_SCANLIMIT   11
#define OP_SHUTDOWN    12
#define OP_DISPLAYTEST 15

#define LEDMatrix_CS(n) do{ n ? GPIO_SetBits(GPIOE,GPIO_Pin_6) : GPIO_ResetBits(GPIOE,GPIO_Pin_6);} while(0) 
#define LOW  0
#define HIGH 1
typedef uint8_t byte;

typedef struct
{
  int x;
  int y;
}coord;

void LedControlInit(void);

/* Send out a single command to the device */
void spiTransfer(int addr, byte opcode, byte data);




void setRotation(int rot);

/*
 * Gets the number of devices attached to this LedControl.
 * Returns :
 * int	the number of devices on this LedControl
 */
int getDeviceCount();

/*
 * Set the shutdown (power saving) mode for the device
 * Params :
 * addr	The address of the display to control
 * status	If true the device goes into power-down mode. Set to false
 *		for normal operation.
 */
void shutdown(int addr, uint8_t status);

/*
 * Set the number of digits (or rows) to be displayed.
 * See datasheet for sideeffects of the scanlimit on the brightness
 * of the display.
 * Params :
 * addr	address of the display to control
 * limit	number of digits to be displayed (1..8)
 */
void setScanLimit(int addr, int limit);

/*
 * Set the brightness of the display.
 * Params:
 * addr		the address of the display to control
 * intensity	the brightness of the display. (0..15)
 */
void setIntensity(int addr, int intensity);

/*
 * Switch all Leds on the display off.
 * Params:
 * addr	address of the display to control
 */
void clearDisplay(int addr);



void setRawXY(int addr, int x, int y, uint8_t state);
uint8_t getRawXY(int addr, int x, int y);
void invertRawXY(int addr, int x, int y);

/*
 * Set the status of a single Led.
 * Params :
 * addr	address of the display
 * row	the row of the Led (0..7)
 * col	the column of the Led (0..7)
 * state	If true the led is switched on,
 *		if false it is switched off
 */
void setLed(int addr, int row, int col, uint8_t state);
void setXY(int addr, int x, int y, uint8_t state);
void _setXY(int addr, coord xy, uint8_t state);
void invertXY(int addr, int x, int y);
uint8_t getLed(int addr, int row, int col);
uint8_t getXY(int addr, int x, int y);
uint8_t _getXY(int addr, coord xy);
coord transform(coord xy);
coord _transform(int x, int y);
coord flipHorizontally(coord xy);
coord flipVertically(coord xy);
coord rotate90(coord xy);
coord rotate180(coord xy);
coord rotate270(coord xy);

void backup();
void restore();

/*
 * Set all 8 Led's in a row to a new state
 * Params:
 * addr	address of the display
 * row	row which is to be set (0..7)
 * value	each bit set to 1 will light up the
 *		corresponding Led.
 */
void setRow(int addr, int row, byte value);

/*
 * Set all 8 Led's in a column to a new state
 * Params:
 * addr	address of the display
 * col	column which is to be set (0..7)
 * value	each bit set to 1 will light up the
 *		corresponding Led.
 */
void setColumn(int addr, int col, byte value);

/*
 * Display a hexadecimal digit on a 7-Segment Display
 * Params:
 * addr	address of the display
 * digit	the position of the digit on the display (0..7)
 * value	the value to be displayed. (0x00..0x0F)
 * dp	sets the decimal point.
 */
void setDigit(int addr, int digit, byte value, uint8_t dp);

/*
 * Display a character on a 7-Segment display.
 * There are only a few characters that make sense here :
 *	'0','1','2','3','4','5','6','7','8','9','0',
 *  'A','b','c','d','E','F','H','L','P',
 *  '.','-','_',' '
 * Params:
 * addr	address of the display
 * digit	the position of the character on the display (0..7)
 * value	the character to be displayed.
 * dp	sets the decimal point.
 */
void setChar(int addr, int digit, char value, uint8_t dp);


#endif	//LedControl.h
