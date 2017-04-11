#ifndef __M8266APP_H
#define __M8266APP_H
#include "sys.h"
#include "stdio.h"
#include "delay.h"
#include "brd_cfg.h"
#include "M8266HostIf.h"
#include "M8266WIFIDrv.h"
#include "M8266WIFI_ops.h"

#define TEST_DATA_SIZE   2048    //1472 // 2048  1024

// Set TCP/UDP Connections  
#define TEST_CONNECTION_TYPE    0	// 0 for UDP, 1 for TCP
#define TEST_LOCAL_PORT  		4321
#define TEST_REMOTE_IP_ADDR     "192.168.0.113"
//#define TEST_REMOTE_IP_ADDR   "192.168.191.1"
#define TEST_REMOTE_PORT  	    6661

void M8266WIFI_Test(void);       // SPI4_wifi模块测试程序
void M8266Wifi_Init(void);       // M8266Wifi App初始化设置

#endif

