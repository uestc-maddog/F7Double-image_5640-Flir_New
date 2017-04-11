#ifndef __SCCB_H
#define __SCCB_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F7������
//OVϵ������ͷ SCCB ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/12/16
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
//IO��������
#define SCCB_SDA_IN()  {GPIOH->MODER&=~(3<<(8*2));GPIOH->MODER|=0<<8*2;}	//PB3����ģʽ
#define SCCB_SDA_OUT() {GPIOH->MODER&=~(3<<(8*2));GPIOH->MODER|=1<<8*2;}    //PB3���ģʽ
//IO����
#define SCCB_SCL(n)  (n?HAL_GPIO_WritePin(GPIOH,GPIO_PIN_7,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOH,GPIO_PIN_7,GPIO_PIN_RESET)) //SCL
#define SCCB_SDA(n)  (n?HAL_GPIO_WritePin(GPIOH,GPIO_PIN_8,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOH,GPIO_PIN_8,GPIO_PIN_RESET)) //SDA

#define SCCB_READ_SDA    HAL_GPIO_ReadPin(GPIOH,GPIO_PIN_8)     //����SDA
#define SCCB_ID         0X60                                    //OV2640��ID

///////////////////////////////////////////
void SCCB_Init(void);
void SCCB_Start(void);
void SCCB_Stop(void);
void SCCB_No_Ack(void);
u8 SCCB_WR_Byte(u8 dat);
u8 SCCB_RD_Byte(void);
//u8 SCCB_WR_Reg(u8 reg,u8 data);
//u8 SCCB_RD_Reg(u8 reg);
#endif

