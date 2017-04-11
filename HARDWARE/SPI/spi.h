#ifndef __SPI_H
#define __SPI_H
#include "sys.h"

// SPI2&SPI4Ч§ЖЏДњТы	

void SPI2_Init(void);
void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler);
u8 SPI2_ReadWriteByte(u8 TxData);
u8 SPI2_Readbuff(u8 *TxData,u16 x);

void SPI4_Init(void);
void SPI4_SetSpeed(u8 SPI_BaudRatePrescaler);
u8   SPI4_ReadWriteByte(u8 TxData);
#endif
