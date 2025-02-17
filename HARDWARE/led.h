#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"
void LedInit(void);
void Led1Toggle(void);
void Led0Toggle(void);

#define ON  1
#define OFF 0

//PE5
#define LED1(n)  do{ n ? GPIO_SetBits(GPIOE,GPIO_Pin_5) : GPIO_ResetBits(GPIOE,GPIO_Pin_5);} while(0)

//PB5

#define LED0(n)   do{ n ? GPIO_SetBits(GPIOB,GPIO_Pin_5) : GPIO_ResetBits(GPIOB,GPIO_Pin_5);} while(0)

#endif



