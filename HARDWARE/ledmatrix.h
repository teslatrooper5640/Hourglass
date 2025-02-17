#ifndef __LEDMATRIX_H
#define __LEDMATRIX_H

#include "stm32f10x.h"
#include "spi.h"
#include "adxl345.h"

#define LED_SIZE 8

#define MAX7219_CS(n) do{ n ? GPIO_SetBits(GPIOE,GPIO_Pin_6) : GPIO_ResetBits(GPIOE,GPIO_Pin_6);} while(0) 

 
void Write_Maxtrix(uint8_t addr, uint8_t data);
void Matrix_Init(void);
void Write_Maxtrix(uint8_t addr, uint8_t data);
void MatrixDrawOneFrame(uint64_t data);


uint64_t GetPixelData(uint8_t x, uint8_t y);
void grounder(float pitch, float roll);

#endif