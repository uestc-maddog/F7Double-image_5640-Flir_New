/********************************************************************
 * M8266HostIf.c
 * .Description
 *     Source file of M8266WIFI Host Interface 
 * .Copyright(c) Anylinkin Technology 2015.5-
 *     IoT@anylinkin.com
 *     http://www.anylinkin.com
 *     http://anylinkin.taobao.com
 *  Author
 *     wzuo
 *  Date
 *  Version
 ********************************************************************/
#include "sys.h"
#include "delay.h"
#include "stdio.h"
#include "string.h"	
#include "brd_cfg.h"
#include "M8266WIFIDrv.h"
#include "M8266HostIf.h"
#include "spi.h"

/***********************************************************************************
 * M8266HostIf_GPIO_SPInCS_nRESET_Pin_Init                                         *
 * Description                                                                     *
 *    To initialise the GPIOs for SPI nCS and nRESET output for M8266WIFI module   *
 *    You may update the macros of GPIO PINs usages for nRESET from brd_cfg.h      *
 *    You are not recommended to modify codes below please                         *
 * Parameter(s):                                                                   *
 *    None                                                                         *
 * Return:                                                                         *
 *    None                                                                         *
 ***********************************************************************************/
void M8266HostIf_GPIO_CS_RESET_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	__HAL_RCC_GPIOE_CLK_ENABLE();			//¿ªÆôGPIOEÊ±ÖÓ
	
	//Initial STM32's GPIO for M8266WIFI_SPI_nCS 
	GPIO_Initure.Pin = M8266WIFI_SPI_nCS_PIN;
	GPIO_Initure.Mode  = GPIO_MODE_OUTPUT_PP;				    // Set as output
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;						// 100MHz
	GPIO_Initure.Pull  = GPIO_NOPULL;							// pull-up
	HAL_GPIO_Init(M8266WIFI_SPI_nCS_GPIO,&GPIO_Initure);        
	HAL_GPIO_WritePin(M8266WIFI_SPI_nCS_GPIO, M8266WIFI_SPI_nCS_PIN,GPIO_PIN_SET); // M8266WIFI_SPI_nCS GPIO output high initially

	
	//Initial STM32's GPIO for M8266WIFI_nRESET 
	GPIO_Initure.Pin   = M8266WIFI_nRESET_PIN;
	GPIO_Initure.Mode  = GPIO_MODE_OUTPUT_PP;					// Set as output
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;						// 100MHz
	GPIO_Initure.Pull  = GPIO_NOPULL;							// pull-up
	HAL_GPIO_Init(M8266WIFI_nRESET_GPIO, &GPIO_Initure);	
	HAL_GPIO_WritePin(M8266WIFI_nRESET_GPIO, M8266WIFI_nRESET_PIN, GPIO_PIN_SET);  // M8266WIFI_nRESET GPIO output high initially
}
/***********************************************************************************
 * M8266HostIf_SPI_Init                                                            *
 * Description                                                                     *
 *    To initialise the SPI Interface for M8266WIFI module                         *
 *    You may update the macros of SPI usages for nRESET from brd_cfg.h            *
 *    You are not recommended to modify codes below please                         *
 * Parameter(s):                                                                   *
 *    None                                                                         *
 * Return:                                                                         *
 *    None                                                                         *
 ***********************************************************************************/
void M8266HostIf_SPI_Init(void)
{
	SPI4_Init();	
}
/***********************************************************************************
 * M8266HostIf_SPI_SetSpeed                                                        *
 * Description                                                                     *
 *    To setup the SPI Clock Speed for M8266WIFI module                            *
 * Parameter(s):                                                                   *
 *    SPI_BaudRatePrescaler: SPI BaudRate Prescaler                                *
 * Return:                                                                         *
 *    None                                                                         *
 ***********************************************************************************/
void M8266HostIf_SPI_SetSpeed(u8 SPI_BaudRatePrescaler)
{
	SPI4_SetSpeed(SPI_BaudRatePrescaler);
} 

/***********************************************************************************
 * M8266HostIf_Init                                                                *
 * Description                                                                     *
 *    To initialise the Host interface for M8266WIFI module                        *
 * Parameter(s):                                                                   *
 *    baud: baud rate to set                                                       *
 * Return:                                                                         *
 *    None                                                                         *
 ***********************************************************************************/ 
void M8266HostIf_Init(void)
{
	 M8266HostIf_GPIO_CS_RESET_Init();
	 M8266HostIf_SPI_Init();
	 //M8266HostIf_SPI_SetSpeed(SPI_BAUDRATEPRESCALER_4);		//SPI4 CLK = 108MHz  4·ÖÆµ
}

//////////////////////////////////////////////////////////////////////////////////////
// BELOW FUNCTIONS ARE REQUIRED BY M8266WIFIDRV.LIB. 
// PLEASE IMPLEMENTE THEM ACCORDING TO YOUR HARDWARE
//////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************
 * M8266HostIf_Set_nRESET_Pin                                                      *
 * Description                                                                     *
 *    To Outpout HIGH or LOW onto the GPIO pin for M8266WIFI nRESET                *
 *    You may update the macros of GPIO PIN usages for nRESET from brd_cfg.h       *
 *    You are not recommended to modify codes below please                         *
 * Parameter(s):                                                                   *
 *    1. level: LEVEL output to nRESET pin                                         *
 *              0 = output LOW  onto nRESET                                        *
 *              1 = output HIGH onto nRESET                                        *
 * Return:                                                                         *
 *    None                                                                         *
 ***********************************************************************************/
void M8266HostIf_Set_nRESET_Pin(u8 level)
{
	if(level) HAL_GPIO_WritePin(M8266WIFI_nRESET_GPIO, M8266WIFI_nRESET_PIN, GPIO_PIN_SET);
	else 	  HAL_GPIO_WritePin(M8266WIFI_nRESET_GPIO, M8266WIFI_nRESET_PIN, GPIO_PIN_RESET);
}
/***********************************************************************************
 * M8266HostIf_Set_SPI_nCS_PIN                                                     *
 * Description                                                                     *
 *    To Outpout HIGH or LOW onto the GPIO pin for M8266WIFI SPI nCS               *
 *    You may update the macros of GPIO PIN usages for SPI nCS from brd_cfg.h      *
 *    You are not recommended to modify codes below please                         *
 * Parameter(s):                                                                   *
 *    1. level: LEVEL output to SPI nCS pin                                        *
 *              0 = output LOW  onto SPI nCS                                       *
 *              1 = output HIGH onto SPI nCS                                       *
 * Return:                                                                         *
 *    None                                                                         *
 ***********************************************************************************/
void M8266HostIf_Set_SPI_nCS_Pin(u8 level)
{
	if(level) HAL_GPIO_WritePin(M8266WIFI_SPI_nCS_GPIO, M8266WIFI_SPI_nCS_PIN, GPIO_PIN_SET);
	else      HAL_GPIO_WritePin(M8266WIFI_SPI_nCS_GPIO, M8266WIFI_SPI_nCS_PIN, GPIO_PIN_RESET);
}

/***********************************************************************************
 * M8266WIFIHostIf_delay_us                                                        *
 * Description                                                                     *
 *    To loop delay some micro seconds.                                            *
 * Parameter(s):                                                                   *
 *    1. nus: the micro seconds to delay                                           *
 * Return:                                                                         *
 *    none                                                                         *
 ***********************************************************************************/
void M8266HostIf_delay_us(u8 nus)
{
   delay_us(nus);
}
