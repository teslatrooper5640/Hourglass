#ifndef __I2C_H
#define __I2C_H

#include "stm32f10x.h"

//#include "main.h"
 
 
#define IIC_SDA_PORT		GPIOA
#define IIC_SDA_CLK			(RCC_APB2Periph_GPIOA)
#define IIC_SDA_PIN			GPIO_Pin_5
#define IIC_SCL_PORT		GPIOA
#define IIC_SCL_CLK			(RCC_APB2Periph_GPIOA)
#define IIC_SCL_PIN			GPIO_Pin_6
 
#define IIC_SCL_OUT_1		IIC_SCL_PORT->BSRR = (uint32_t)IIC_SCL_PIN // ÖÃ1
#define IIC_SCL_OUT_0		IIC_SCL_PORT->BRR = (uint32_t)IIC_SCL_PIN
#define IIC_SDA_OUT_1		IIC_SDA_PORT->BSRR = (uint32_t)IIC_SDA_PIN // ÖÃ1
#define IIC_SDA_OUT_0		IIC_SDA_PORT->BRR = (uint32_t)IIC_SDA_PIN
#define IIC_SDA_IN()		((IIC_SDA_PORT->IDR & IIC_SDA_PIN) != 0)


void Set_IIC_SDA_OUT(void);
void Set_IIC_SDA_IN(void);
void IIC_init(void);
void IIC_start(void);
void IIC_stop(void);
void IIC_ack(void);
void IIC_noack(void);
uint8_t IIC_wait_ack(void);
void IIC_send_byte(uint8_t txd);
uint8_t IIC_read_byte(uint8_t ack);

#endif



