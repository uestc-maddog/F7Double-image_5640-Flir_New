#ifndef __USART2_H
#define __USART2_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F7������
//USART��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/12/17
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
extern UART_HandleTypeDef USART2_Handler;  //USART2���

void usart2_init(u32 bound);
#endif

