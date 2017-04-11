#include "M8266App.h"

void M8266Wifi_Init(void)
{
	u8 success = 0; 
	
	M8266HostIf_Init();	            // Initialise M8266WIFI module GPIOs for nCS/nRESET, STM32F7_SPI2
	success = M8266WIFI_Module_Init_Via_SPI();
	if(success)	printf("M8266WIFI_Module_Init_Via_SPI Successful!\r\n");
	else		printf("M8266WIFI_Module_Init_Via_SPI not Successful!\r\n");
	
	// UDP连接
	//u8 M8266WIFI_Reboot_And_Config_Connection(u8 tcp_udp, u16 local_port, u8* remote_ip, u16 remote_port, u8 link_no)
	if(M8266WIFI_Config_Connection_via_SPI(TEST_CONNECTION_TYPE, TEST_LOCAL_PORT, (u8*)TEST_REMOTE_IP_ADDR, TEST_REMOTE_PORT, 0) != 1) 
	{		
		// when error
		printf("M8266WIFI_Config_Connection_via_SPI Error!\r\n");	
	}
	else
	{
		 // when success 
		printf("M8266WIFI_Config_Connection_via_SPI Successful!\r\n");
		if(TEST_CONNECTION_TYPE)  printf("TCP Remote IP %s:%d\r\n", (u8*)TEST_REMOTE_IP_ADDR, (int)TEST_REMOTE_PORT);
		else                      printf("UDP Remote IP %s:%d\r\n", (u8*)TEST_REMOTE_IP_ADDR, (int)TEST_REMOTE_PORT);
	}
	
//	// 打开Airkiss自动配网
//	if(M8266WIFI_SPI_StartSmartConfig(1, NULL)==1) // smart config successfully
//	{
//		printf("wait for airkiss...\r\n"); 	 //add some codes here if airkiss successful
//	}
//	else
//	{
//		printf("airkiss error!\r\n");   //add some codes here if airkiss successful
//	}
//	while(1);
}

void M8266WIFI_Test(void)
{
	static u16 batch = 0;

	u8  data[TEST_DATA_SIZE] = {0};                 // 1KB缓存
	u16 sent = 0, received = 0, test_packet_size;
	u16 test_data_size; 

	u32 total_received = 0;
	u16 MBytes = 0;

	u16 i;
	u16 status = 0;
	u8  byte = 0;
	u8  link_no=0;
	
	/***********************************Test Transmition and Reception***********************************/
	
     // Transmission Test: to test send data to remote constantly
	test_packet_size = TEST_DATA_SIZE;                    // 1KB   1024
	for(i = 0; i < test_packet_size; i++) data[i] = i; 
	i = 0;
	for(batch = 0;   ; batch++)
	{	   
		data[0] = batch >> 8;         // 标记当前为第 batch KB
		data[1] = batch & 0xFF;

		//u16 M8266WIFI_SPI_Send_Data(u8 data[], u16 data_len, u8 link_no, u16* status)
		sent = M8266WIFI_SPI_Send_Data(data, test_packet_size, 0, &status); // 发送1KB数据 ，sent:SPI接口传递出去的个数
		//printf("sent = %d\r\n" , sent);
		if(batch % 128 == 0)          // 2048 2MB
		{
			printf("%dM\r\n" , i); 
			if(i == 10) break;
			i += 2;                 
		}
		//printf("Num %d KB Sended!\r\n" , batch);

//		if( (sent!=test_packet_size) || ( (status&0xFF)!= 0 ) ) 
//		{
//			sent += 0;
//		}

#if 0	// to avoid print bring about delay	during test
		if( (sent!=len) || ( (status&0xFF)!= 0 ) )
		{
			printf("\r\n Send Data[len=%d] to M8266 failed. Only %d bytes has been sent(0x%04X).\r\n", len, sent, status);
			break;
		}
		else
		{
			printf("\r\n Send Data[len=%d] to M8266 successfully(%04X).\r\n", len, status);
		}
#endif
	} // end of while(1)	 
}