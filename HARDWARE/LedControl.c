/*
 *    LedControl.cpp - A library for controling Leds with a MAX7219/MAX7221
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


#include "LedControl.h"


/* We keep track of the led-status for all 8 devices in this array */
byte status[64] = {0};//最多可以记录8个LED矩阵的状态
byte backupStatus[64];
/* The maximum number of devices we use */
int maxDevices = 2;

int rotation;
/* The array for shifting the data to the devices */
byte spidata[16] = {0};


void LedControlInit(void)
{
	uint8_t i;
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_SetBits(GPIOE,GPIO_Pin_6);

	spi_init();
	
	 for(i=0;i<maxDevices;i++)
	{
        spiTransfer(i,OP_DISPLAYTEST,0);
        //scanlimit is set to max on startup
        setScanLimit(i,7);
        //decode is done in source
        spiTransfer(i,OP_DECODEMODE,0);
		//亮度设置为最暗
		setIntensity(i,0);
        clearDisplay(i);
        shutdown(i,0);
    }
}


int getDeviceCount() {
    return maxDevices;
}

void shutdown(int addr, uint8_t b) {
    if(addr<0 || addr>=maxDevices)
        return;
    if(b)
        spiTransfer(addr, OP_SHUTDOWN,0);
    else
        spiTransfer(addr, OP_SHUTDOWN,1);
}

void setScanLimit(int addr, int limit) {
    if(addr<0 || addr>=maxDevices)
        return;
    if(limit>=0 && limit<8)
        spiTransfer(addr, OP_SCANLIMIT,limit);
}

void setIntensity(int addr, int intensity) {
    if(addr<0 || addr>=maxDevices)
        return;
    if(intensity>=0 && intensity<16)
        spiTransfer(addr, OP_INTENSITY,intensity);
}

void clearDisplay(int addr) {
    int offset;
	int i;

    if(addr<0 || addr>=maxDevices)
        return;
    offset=addr*8;
    for(i=0;i<8;i++) {
        status[offset+i]=0;
        spiTransfer(addr, i+1,status[offset+i]);
    }
}

void setRotation(int rot) {
  rotation = rot;
}

coord flipHorizontally(coord xy) {
  xy.x = 7- xy.x;
  return xy;
}

coord flipVertically(coord xy) {
  xy.y = 7- xy.y;
  return xy;
}

coord rotate90(coord xy) {
  int tmp = xy.y;
  xy.y = xy.x;
  xy.x = tmp;
  return flipHorizontally(xy);
}

coord rotate180(coord xy) {
  return flipHorizontally(flipVertically(xy));
}

coord rotate270(coord xy) {
  return rotate180(rotate90(xy));
}

coord transform(coord xy) {
  if (rotation == 90) {
    xy = rotate90(xy);
  } else if (rotation == 180) {
    xy = rotate180(xy);
  } else if (rotation == 270) {
    xy = rotate270(xy);
  }
  return xy;
}

coord _transform(int x, int y) {
  coord xy;
  xy.x = x;
  xy.y =y;
  return transform(xy);
}

void setXY(int addr, int x, int y, uint8_t state) {
  coord xy;
  xy.x = x;
  xy.y = y;
  xy = transform(xy);
  if(addr == 0)
  {
	 xy = rotate90(xy);
  } 
  setLed(addr, xy.y, xy.x, state);
}

void setRawXY(int addr, int x, int y, uint8_t state) {
  setLed(addr, y, x, state);
}

uint8_t getXY(int addr, int x, int y) {
  coord xy;
  xy.x = x;
  xy.y = y;
  xy = transform(xy);
  if(addr == 0)
  {
	 xy = rotate90(xy);
  }
  return getLed(addr, xy.y, xy.x);
}

uint8_t getRawXY(int addr, int x, int y) {
  return getLed(addr, y, x);
}

void _setXY(int addr, coord xy, uint8_t state) {
  setXY(addr, xy.x, xy.y, state);
}

//纯粹设置灯珠状态不包含位置信息，为什么复杂化？直接点亮60个灯不可以吗
void setLed(int addr, int row, int column, uint8_t state)
{
    int offset;
    byte val=0x00;

    if(addr<0 || addr>=maxDevices)
        return;
    if(row<0 || row>7 || column<0 || column>7)
        return;
    offset=addr*8;//地址为0或1 offset = 0 或 8 （确定是第几行，第二个矩阵是从第八行开始）
//	if(addr == 1)
    val=0x80 >> column;
//	else
//	val=0x01<< column;
	//设置1个灯的状态数据
    if(state)
        status[offset+row]=status[offset+row]|val;//第二个矩阵 offset = 8 从第8行到第15行 
    else {
        val=~val;
        status[offset+row]=status[offset+row]&val;
    }
    spiTransfer(addr, row+1,status[offset+row]);
}

void invertRawXY(int addr, int x, int y) {
  setRawXY(addr, x, y, !getRawXY(addr, x, y));
  return ;
}

void invertXY(int addr, int x, int y) {
  setXY(addr, x, y, !getXY(addr, x, y));
  return ;
}

uint8_t _getXY(int addr, coord xy) {
  return getXY(addr, xy.x, xy.y);
}

//获取某个LED的状态
uint8_t getLed(int addr, int row, int column) {
    int offset;
    uint8_t state;

    if(addr<0 || addr>=maxDevices)
        return 0;
    if(row<0 || row>7 || column<0 || column>7)
        return 0;
    offset=addr*8;
    state = (1 == ( (status[offset+row] >> (7-column)) & 1));
    return state;
}

void setRow(int addr, int row, byte value)
{
    int offset;
    if(addr<0 || addr>=maxDevices)
        return;
    if(row<0 || row>7)
        return;
    offset=addr*8;
    status[offset+row]=value;
    spiTransfer(addr, row+1,status[offset+row]);
}

void setColumn(int addr, int col, byte value)
{
    byte val;
	int row;

    if(addr<0 || addr>=maxDevices)
        return;
    if(col<0 || col>7)
        return;
    for(row=0;row<8;row++) {
        val=value >> (7-row);
        val=val & 0x01;
        setLed(addr,row,col,val);
    }
}

//void setDigit(int addr, int digit, byte value, uint8_t dp) 
//{
//    int offset;
//    byte v;

//    if(addr<0 || addr>=maxDevices)
//        return;
//    if(digit<0 || digit>7 || value>15)
//        return;
//    offset=addr*8;
////    v=pgm_read_byte_near(charTable + value);
//    if(dp)
//        v|=0x80;//B10000000
//    status[offset+digit]=v;
//    spiTransfer(addr, digit+1,v);
//}

//opcode = row+1
//max7219串联要求发送完有效数据后还要发送0xX0XX 16位no_op数据 如果要点亮第四个矩阵发完有效数据后还要要发三个no_op数据
void spiTransfer(int addr, volatile byte opcode, volatile byte data)
{
	int i;
    //Create an array with the data to shift out
    int offset=addr*2;//0或2
    int maxbytes=maxDevices*2;//4

    for(i=0;i<maxbytes;i++)
        spidata[i]=(byte)0;
    //put our device data into the array
    spidata[offset+1]=opcode;
    spidata[offset]=data;
    //enable the line
    LEDMatrix_CS(LOW);
    //Now shift out the data
    for(i=maxbytes;i>0;i--)
		spi_read_write_byte(spidata[i-1]);//发四个字节
    //latch the data onto the display
    LEDMatrix_CS(HIGH);
}

//void backup() {
//  memcpy(backupStatus, status, 64);
//}
//void restore() {
//	int offset,addr,i;
//  memcpy(status, backupStatus, 64);
//  
//  for (addr=0; addr<maxDevices; addr++) {
//    offset=addr*8;
//    for(i=0;i<8;i++) {
//      spiTransfer(addr, i+1,status[offset+i]);
//    }
//  }
//}
