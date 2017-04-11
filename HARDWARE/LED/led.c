#include "led.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F7开发板
//LED驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/11/27
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//初始化PB0,PB1为输出.并使能这两个口的时钟		    
//LED IO初始化

#define LEPTON_PW_DWN_L_Pin GPIO_PIN_11
#define LEPTON_PW_DWN_L_GPIO_Port GPIOB
#define LEPTON_RESET_L_Pin GPIO_PIN_15
#define LEPTON_RESET_L_GPIO_Port GPIOA

void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure,GPIO_InitStruct;
    __HAL_RCC_GPIOB_CLK_ENABLE();			//开启GPIOB时钟
	__HAL_RCC_GPIOA_CLK_ENABLE();			//开启GPIOB时钟
	
    GPIO_Initure.Pin=GPIO_PIN_1|LEPTON_PW_DWN_L_Pin; //PB0,1
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
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
	
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET);	//PB1置1 
}

//初始化、使能外设电源
void Power_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	__HAL_RCC_GPIOA_CLK_ENABLE();			//开启GPIOA时钟
    __HAL_RCC_GPIOH_CLK_ENABLE();			//开启GPIOH时钟
	
    GPIO_Initure.Pin=GPIO_PIN_3|GPIO_PIN_5; //PH3
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
    GPIO_Initure.Pull=GPIO_NOPULL;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
    HAL_GPIO_Init(GPIOH,&GPIO_Initure);
	
	HAL_GPIO_WritePin(GPIOH,GPIO_PIN_3,GPIO_PIN_SET);	//PH3置高 
	HAL_GPIO_WritePin(GPIOH,GPIO_PIN_2|GPIO_PIN_5,GPIO_PIN_RESET);	
	

	GPIO_Initure.Pin=GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;         
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,GPIO_PIN_RESET);	
	
	
	GPIO_Initure.Pin=GPIO_PIN_2;            //PH2
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
    GPIO_Initure.Pull=GPIO_PULLDOWN;        //下拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
    HAL_GPIO_Init(GPIOH,&GPIO_Initure);
	
	HAL_GPIO_WritePin(GPIOH,GPIO_PIN_2,GPIO_PIN_RESET);	
}	