#include "led.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F7������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/11/27
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//��ʼ��PB0,PB1Ϊ���.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��

#define LEPTON_PW_DWN_L_Pin GPIO_PIN_11
#define LEPTON_PW_DWN_L_GPIO_Port GPIOB
#define LEPTON_RESET_L_Pin GPIO_PIN_15
#define LEPTON_RESET_L_GPIO_Port GPIOA

void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure,GPIO_InitStruct;
    __HAL_RCC_GPIOB_CLK_ENABLE();			//����GPIOBʱ��
	__HAL_RCC_GPIOA_CLK_ENABLE();			//����GPIOBʱ��
	
    GPIO_Initure.Pin=GPIO_PIN_1|LEPTON_PW_DWN_L_Pin; //PB0,1
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
	
	/*Configure GPIO pins : SYSTEM_LED_Pin LEPTON_PW_DWN_L_Pin LEPTON_RESET_L_Pin */
	GPIO_InitStruct.Pin = LEPTON_RESET_L_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, LEPTON_RESET_L_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, LEPTON_PW_DWN_L_Pin, GPIO_PIN_RESET);
	
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET);	//PB1��1 
}

//��ʼ����ʹ�������Դ
void Power_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	__HAL_RCC_GPIOA_CLK_ENABLE();			//����GPIOAʱ��
    __HAL_RCC_GPIOH_CLK_ENABLE();			//����GPIOHʱ��
	
    GPIO_Initure.Pin=GPIO_PIN_3|GPIO_PIN_5; //PH3
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
    GPIO_Initure.Pull=GPIO_NOPULL;          //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
    HAL_GPIO_Init(GPIOH,&GPIO_Initure);
	
	HAL_GPIO_WritePin(GPIOH,GPIO_PIN_3,GPIO_PIN_SET);	//PH3�ø� 
	HAL_GPIO_WritePin(GPIOH,GPIO_PIN_2|GPIO_PIN_5,GPIO_PIN_RESET);	
	

	GPIO_Initure.Pin=GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;         
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,GPIO_PIN_RESET);	
	
	
	GPIO_Initure.Pin=GPIO_PIN_2;            //PH2
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
    GPIO_Initure.Pull=GPIO_PULLDOWN;        //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
    HAL_GPIO_Init(GPIOH,&GPIO_Initure);
	
	HAL_GPIO_WritePin(GPIOH,GPIO_PIN_2,GPIO_PIN_RESET);	
}	