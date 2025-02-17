#ifndef __MYDELAY_H
#define __MYDELAY_H

#include "stm32f10x.h"

void TIM3_Int_Init(u16 arr,u16 psc);

uint32_t millis(void);
void NonBlockDelay_Delay_ms (unsigned long t);
uint8_t NonBlockDelay_Timeout (void);
unsigned long NonBlockDelay_Time(void);

#endif