#ifndef __SPI_H
#define __SPI_H
#include "stm32f10x.h"

#define SPIPORT  SPI2
#define GPIOPORT GPIOB

 #define MISO_PIN    GPIO_Pin_14
 #define MOSI_PIN    GPIO_Pin_15
 #define SCK_PIN     GPIO_Pin_13

void spi_init(void);
void set_spix_baudrate(uint16_t SPI_BaudRatePrescaler);
u8 spi_read_write_byte(u8 TxData);


void SPI1_Init(void);
void SPI1_set_baudrate(uint16_t SPI_BaudRatePrescaler);
u8 SPI1_read_write_byte(u8 TxData);

#endif


