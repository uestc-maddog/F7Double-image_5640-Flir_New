/********************************************************************
 * brd_cfg.h
 * .Description
 *     header file of main board configuration macros 
 * .Copyright(c) Anylinkin Technology 2015.5-
 *     IoT@anylinkin.com
 *     http://www.anylinkin.com
 *     http://anylinkin.taobao.com
 *  Author
 *     wzuo
 *  Date
 *  Version
 ********************************************************************/

#ifndef _BRG_CFG_H_
#define _BRG_CFG_H_

#define M8266WIFI_CONFIG_VIA_SPI

///////////////////////////////////////////////////////////////
// M8266WIFI Module Interface Board Usages Macros
//      -- nRESET= PE3, nCS=PE4
///////////////////////////////////////////////////////////////
#define  M8266WIFI_nRESET_GPIO	 GPIOE 
#define  M8266WIFI_nRESET_PIN	 GPIO_PIN_3

#define  M8266WIFI_SPI_nCS_GPIO	 GPIOE
#define  M8266WIFI_SPI_nCS_PIN	 GPIO_PIN_4

#define  M8266WIFI_INTERFACE_SPI SPI4


#endif
