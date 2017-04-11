/********************************************************************
 * M8266WIFI_ops.c
 * .Description
 *     source file of M8266WIFI HAL operations
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
#include "stdio.h"
#include "string.h"
#include "delay.h"   
#include "brd_cfg.h"
#include "M8266HostIf.h"
#include "M8266WIFIDrv.h"
#include "M8266WIFI_ops.h"

extern u8* SSID;
extern u8* PWD;

////////////////////////////////////////////////////////////////////////////////////
// Functions as examples of M8266WIFI Module Operations
//    -- Fundamentals
////////////////////////////////////////////////////////////////////////////////////

/***********************************************************************************
 * M8266WIFI_Module_Hardware_Reset                                                 *
 * Description                                                                     *
 *    To perform a hardware reset to M8266WIFI module and bring it                 *
 *    into normal boot up from external SPI flash                                  *
 * Parameter(s):                                                                   *
 *    none                                                                         *
 * Return:                                                                         *
 *    none                                                                         *
 ***********************************************************************************/
void M8266WIFI_Module_Hardware_Reset(void)
{
	 M8266HostIf_Set_SPI_nCS_Pin(0);    	 // Module nCS==M8266 IO15 as well, should be low during reset in order for a normal reset 
	 delay_us(1000);
	
	 M8266HostIf_Set_nRESET_Pin(0);		     // Pull low the nReset Pin to bring the module into reset state
	 delay_us(1000); 
	 M8266HostIf_Set_nRESET_Pin(1);		     // Pull high again the nReset Pin to bring the module exiting reset state
	 
	 delay_us(700*1000);      				 // delay 0.5s for boot print information. Wait time should be no less than 0.5s here. 
	//delay_ms(1000);						 // or delay_ms(1000) for 1s instead;  DO NOT use 'delay_us(1*1000*1000)' for delay of 1s in STM32
											 // since max nus for void delay_us(u32 nus) is 798915us
	 M8266HostIf_Set_SPI_nCS_Pin(1);
	 delay_us(1000);          				 // delay 1ms	
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
/////// BELOW FUNCTIONS ARE QUERYING OR CONFIGURATION VIA SPI
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
u8 M8266WIFI_SPI_wait_sta_connecting_to_ap_and_get_ip(char* sta_ip, u8 max_wait_time_in_s)
{
	u16  i;
	u16  status=0;

	for(i=0; i < 1*max_wait_time_in_s; i++)  // max wait
	{
		if( (M8266WIFI_SPI_Get_STA_IP_Addr(sta_ip , &status)==1) && (strcmp(sta_ip, "0.0.0.0")!=0) ) break;
		delay_ms(1000);
		continue;
	}
	if(i >= 1 * max_wait_time_in_s) return 0;  // false

	return 1;  // true
}
/***********************************************************************************
 * M8266WIFI_Module_Init_Via_SPI(void)                                             *
 * Description                                                                     *
 *    To perform a Initialization sequency to M8266WIFI module via SPI             *
 *    Will wait the Module connected to AP/routers if in STA or STA+AP mode        *
 * Parameter(s):                                                                   *
 *    none                                                                         *
 * Return:                                                                         *
 * Return:                                                                         *
 *       0 = failed                                                                *
 *       1 = success                                                               * 
 ***********************************************************************************/
u8 M8266WIFI_Module_Init_Via_SPI(void)
{
	u8   i = 0, Get_OpmodeTime =0, sta_ap_mode = 0;
	u16  status = 0;
	char sta_ip[15+1]={0};
	
	M8266WIFI_Module_Hardware_Reset();
	delay_ms(1000);
	
	M8266HostIf_SPI_Select((uint32_t)SPI4, 13500000);  // SPI4 13.5MHz
	//M8266HostIf_SPI_Select((uint32_t)SPI4, 27000000);  // SPI4 27MHz	
	printf("SPI4 8分频 13.5MHz\r\n");
	
	//u8 M8266WIFI_SPI_Interface_Communication_OK(u8* byte)
#if 1 // test SPI communication is OK. You may give a loop for stress test. Comment it for formal release
	{
		u16 i, j;
		u8  byte = 0;
		for(i=0, j=0; i<10000; i++)
		{
			if(M8266WIFI_SPI_Interface_Communication_OK(&byte)==0) 	  //	if SPI fundamental Communication failed
				j++;
		}
		printf("SPI4 Communication Error Times(Total:10000): %d\r\n", j);
	 }
#endif

	// if STA mode or STA+AP mode, then, wait connection to external AP and getting ip
	for(i = 0; i < 6; i++)
	{
		if(M8266WIFI_SPI_Get_Opmode(&sta_ap_mode, &status) == 0) 
		{
			printf("M8266WIFI module failed to get op mode!\r\n");
		}
		else break;
		delay_ms(20);
	}
	while(1)
	{
		if(sta_ap_mode != 1)
		{
			M8266WIFI_SPI_Set_Opmode(1, 1, &status);     // 设置为STA mode，并保存
			printf("Set to STA mode\r\n");
			delay_ms(2);
		}
		else
		{
			printf("It is STA mode Now\r\n");
			break;
		}
		
		if(M8266WIFI_SPI_Get_Opmode(&sta_ap_mode, &status) == 0) 
		{
			printf("M8266WIFI module failed to get op mode!\r\n");
			return 0;
		}
	}
	
	if(M8266WIFI_SPI_wait_sta_connecting_to_ap_and_get_ip(sta_ip, 10) == 0) // max wait 10s to get sta ip
	{
		printf("M8266WIFI module try to connect ap!\r\n");
		
//		//	// Airkiss自动配网
//		if(M8266WIFI_SPI_StartSmartConfig(1, NULL)==1) // smart config successfully
//		{
//			printf("wait for airkiss...\r\n"); 	 //add some codes here if airkiss successful
//		}
//		else
//		{
//			printf("airkiss error!\r\n");   //add some codes here if airkiss successful
//		}
		
		while(M8266WIFI_SPI_STA_Connect_Ap(SSID, PWD, 0, 15, &status) == 0) // 等待wifi连接成功  10s
		{
			printf("Connect_Ap Error %d!\r\n", status);
			delay_ms(50);
		}
		
		while(M8266WIFI_SPI_wait_sta_connecting_to_ap_and_get_ip(sta_ip, 10) == 0);   // 等待获取模块IP
		sta_ip[15] = 0;
		printf("module ip：%s\r\n", sta_ip);
		return 1; 
	}
	else	
	{
		sta_ip[15] = 0;
		printf("module ip：%s\r\n", sta_ip);
	}
	return 1;
 }
 
/*************************************************************************************
 * M8266WIFI_Config_Connection_Via_SPI                                               *
 * Description                                                                       *
 *    To reboot the module, wait wifi connection, and setup udp/tcp connection       *
 *    whether the response is what is expected via SPI                               *
 * Parameter(s):                                                                     *
  *    1. tcp_udp     : the type of connection to set up                             *
 *                     = 0, to set up connection of UDP                              *
 *                     = 1, to set up connection of TCP                              *
 *    2. local_port  : to specify the local port if any                              *
 *                     if local_port=0, then a random local port will be generated   *
 *    3. remote_ip   : the string of ip address of remote server                     *
 *                     e.g. "192.168.1.2"                                            *
 *    4. remote_port : the port value of remote server                               *
 *    5. link_no     : the link_number to be used. max 8 links as per mutitiple link *
 * Return:                                                                           *
 *       0 = failed                                                                  *
 *       1 = success                                                                 * 
 *************************************************************************************/
u8 M8266WIFI_Config_Connection_via_SPI(u8 tcp_udp, u16 local_port, u8* remote_ip, u16 remote_port, u8 link_no)
{
	u16 status=0;
  //Setup the TCP/UPD link
  //  u8 M8266WIFI_SPI_Setup_Connection(u8 tcp_udp, u16 local_port, u8* remote_ip, u16 remote_port, u8 link_no, u16* status);
	return M8266WIFI_SPI_Setup_Connection(tcp_udp, local_port, remote_ip, remote_port, link_no, &status);
}

