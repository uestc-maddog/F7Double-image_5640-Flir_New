/********************************************************************
 * M8266WIFIDrv.h
 * .Description
 *     Header file of M8266WIFI Dirver
 * .Copyright(c) Anylinkin Technology 2015.5-
 *     IoT@anylinkin.com
 *     http://www.anylinkin.com
 *     http://anylinkin.taobao.com
 *  Author
 *     wzuo
 *  Date
 *  Version
 ********************************************************************/
#ifndef _M8266WIFI_H_
#define _M8266WIFI_H_

typedef unsigned          char uint8_t;
typedef unsigned short    int  uint16_t;
typedef unsigned          int  uint32_t;

typedef uint32_t  	u32;
typedef uint16_t 		u16;
typedef uint8_t  		u8;


/***********************************************************************************
 * M8266HostIf_SPI_Select                                                          *
 * Description                                                                     *
 *  1.To specify the SPI used by providing the base address of SPI used            *
 *  2.Called when Initial M8266WIFI Module before perform SPI read/write           *
 * Parameter(s):                                                                   *
 *  1. spi_base_addr: the base address of used                                     *
 *     you may use M8266WIFI_INTERFACE_SPI defined in brd_cfg.h as example         *
 *  2. spi_clock you set in HZ                                                     *
 *     e.g.  10500000 for 10.5MHz                                                  *
 * Return:                                                                         *
 *    none                                                                         *
 ***********************************************************************************/
void M8266HostIf_SPI_Select(uint32_t spi_base_addr, uint32_t spi_clock);

/***********************************************************************************
 * M8266WIFI_SPI_Interface_Communication_OK                                        *
 * Description                                                                     *
 *  1. To write a byte and then read out from M8266WIFI module register            *
 *     to check whether the fundamentai SPI communication (read/write)             *
 *     is stablly OK under the clock and interconnection                           *
 *  2. Called during the initialization of M8266WIFI Module                        *
 *     DONOT CALL IT AFTER MODULE HAS STARTED THE NORMAL OPERATION                 *
 *  3. You may repeat calling it in your debug code for speed stablility test      *
 *     during your stress performance test to your produc                          * 
 * Parameter(s):                                                                   *
 *  1. byte:  a pointer to the byte read out during test                           *
 *     - normally the data should be 0x45 during test.                             *
 *       if it is 0x45, this function will return 1 for success as well            *
 *     - If readout other value, it may indicating the fundamental SPI             *
 *       communication is not OK. e.g. 0xFF may indicates that your spi            *
 *       port has problem such incoreect interconnection or initialization         *
 *     - user NULL if you don't expect this data                                   * 
 * Return:                                                                         *
 *   1. success                                                                    *
 *   0. failure                                                                    *
 ***********************************************************************************/
u8 M8266WIFI_SPI_Interface_Communication_OK(u8* byte);	

/********************************************************************
 * M8266WIFI_SPI_TxdBuffer_Free
 * .Description:
 *     To check whether the M8266WIFI module TxdBuffer is free
 * .Parameters
 *     None
 * .Return value:
 *     1, if the TxdBuffer is free
 *     0, if the TxdBuffer is not free
 ********************************************************************/
u8 M8266WIFI_SPI_TxdBuffer_Free(void);

/********************************************************************
 * M8266WIFI_SPI_Force_TxdBuffer_Free(void)
 * .Description:
 *     To force the M8266WIFI TxdBuffer Free
 * .Parameters
 *     None
 * .Return value:
 *     None
 *     None
 ********************************************************************/
void M8266WIFI_SPI_Force_TxdBuffer_Free(void);

/********************************************************************
 * M8266WIFI_SPI_Send_Data
 * .Description:
 *     To send Data to WIFI via M8266 module SPI
 * .Parameters
 *     1. Data   : the pointer to the Data buffer to be sent
 *     2. len    : the length the buffer to be sent
 *     3. link_no: the wifi service link number sent to
 *     4. pointer to return errcode(LSB) and status(MSB) when error encountered
 *        use NULL if you don't expect errcode and status 
 * .Return value:
 *     Actually length that has been sent successfuuly
 ********************************************************************/
u16 M8266WIFI_SPI_Send_Data(u8 Data[], u16 Data_len, u8 link_no, u16* status);

/********************************************************************
 * M8266WIFI_SPI_Has_DataReceived
 * .Description: 
 *     To check whether the M8266WIFI module has received data expecting master to fetch
 * .Parameters
 *     None
 * .Return value:
 *     1, if the M8266WIFI module has received data
 *     0, if the M8266WIFI module has not received data
 ********************************************************************/
u8 M8266WIFI_SPI_Has_DataReceived(void);

/********************************************************************
 * M8266WIFI_SPI_RcvData
 * .Description:
 *     To receive the data from M8266WIFI
 * .Parameters
 *     1. Data           - the buffer to contained the received Data
 *     2. max_len        - the max length of Data to fetech
 *     3. max_wait_in_ms - the max time of wait for the Data fetching
 *     4. link_no        - pointer to return the link_no that current Data fetched from
 *                       - use NULL if you don't expect to get the value
 *     5. status         - pointer to return errcode(LSB) and status(MSB) when error encountered
 *                         use NULL if you don't expect errcode and status 
 * .Return value:
 *     the actual length of Data received successfully
 ********************************************************************/
u16 M8266WIFI_SPI_RecvData(u8 Data[], u16 max_len, uint16_t max_wait_in_ms, u8* link_no, u16* status);


///////////////////////////////////////////////////////////////////////////////////
// Below functions related to query/config module via SPI-Only interface
///////////////////////////////////////////////////////////////////////////////////

/********************************************************************
 * M8266WIFI_SPI_Get_Opmode
 * .Description:
 *     To get the op_mode(STA, AP, or STA+AP) of M8266WIFI module via SPI
 * .Parameter(s)
 *     1. op_mode : pointer to the op_mode returned
 *        =1, 			STA mode
 *        =2, 			AP mode
 *        =3, 			STA+AP mode
 *     2. status  : pointer to the status upon failure
 * .Return value:
 *     =1, success
 *     =0, has error(s)
 ********************************************************************/
u8 M8266WIFI_SPI_Get_Opmode(u8* op_mode, u16* status);

/********************************************************************
 * M8266WIFI_SPI_Set_Opmode
 * .Description:
 *     To set op_mode(STA, AP, or STA+AP) of M8266WIFI module via SPI
 * .Parameter(s)
 *     1. op_mode : the op_mode to set
 *        =1, 			set to STA mode
 *        =2, 			set to AP mode
 *        =3, 			set to STA+AP mode
 *        =others, 	set to STA+AP mode
 *     2. saved   : to save the opmode or not
 *        =0,       setting not saved, i.e. after reboot setting will restore to previous
 *        =others,  setting     saved, i.e. after reboot setting will be loaded with the saved one
 *     3. status  : pointer to the status upon failure
 * .Return value:
 *     =1, success
 *     =0, has error(s)
 ********************************************************************/
u8 M8266WIFI_SPI_Set_Opmode(u8 op_mode, u8 saved, u16* status);

/********************************************************************
 * M8266WIFI_SPI_STA_Connect_Ap
 * .Description:
 *     To connect an AP via SPI
 * .Parameter(s)
 *     1. ssid    : the ssid of AP connecting to. Max Size=13 Bytes
 *     2. password: the passowrd of AP connecting to. Max Size=13 Bytes
 *     3. saved   : to save or not the ssid and password after connect
 *        =0,       not saved, i.e. the ssid and password just used for connecting of this time
 *        =others,  saved, i.e. after reboot ssid and password will be used for automatically connecting to ap
 *        !!!!PLEASE DO NOT SAVE IF YOU USE THIS API DURING INITIALIZATION AND EXECUTE IT EACH TIME OF BOOTUP
 *            OR, THE FLASH ON MODULE MIGHT GO TO FAILURE DUE TO LIFT CYCLE OF WRITE!!!! 
 *     3. timeout_in_s: max time in seconds waiting for connected
 *     4. status  : pointer to the status upon failure
 * .Return value:
 *     =1, success
 *     =0, has error(s)
 ********************************************************************/
u8 M8266WIFI_SPI_STA_Connect_Ap(u8 ssid[13+1], u8 password[13+1], u8 saved, u8 timeout_in_s, u16* status);

/********************************************************************
 * M8266WIFI_SPI_Get_STA_Connection_Status
 * .Description:
 *     To get SSID of M8266WIFI STA Connection Status via SPI
 * .Parameter(s)
 *     1. connection_status :  the connection status returned
 *          =0, if station is in idle
 *          =1, if station is connecting to an AP
 *          =2, if station is to connect to an AP but provided with an wrong password
 *          =3, if station is to connect to an AP which is not found
 *          =4, if station failed to connect to the AP
 *          =5, if station is connected to the AP and has got the IP successfully
 *          =255, if the module is in AP-only mode
 *     2. status  : pointer to the status upon failure
 * .Return value:
 *     =1, success
 *     =0, has error(s)
 ********************************************************************/
u8 M8266WIFI_SPI_Get_STA_Connection_Status(u8* connection_status, u16* status);


/********************************************************************
 * M8266WIFI_SPI_Get_STA_IP_Addr
 * .Description:
 *     To get ip address of M8266WIFI STA via SPI
 * .Parameter(s)
 *     1. sta_ip :  the sta ip address returned
 *                  if in AP-only mode or ip not achieved, "0.0.0.0" will returned
 *     2. status  : pointer to the status upon failure
 * .Return value:
 *     =1, success
 *     =0, has error(s)
 ********************************************************************/
u8 M8266WIFI_SPI_Get_STA_IP_Addr(char* sta_ip , u16* status);

/********************************************************************
 * M8266WIFI_SPI_STA_Query_Current_SSID_And_RSSI
 * .Description:
 *     To get current AP info (SSID and RSSI) M8266WIFI STA connected to
 * .Parameter(s)
 *     1. ssid    : the current ssid returned which m8266wifi sta connected to
 *     2. rssi    : the rssi of current connected rssid. 31 if error
 *     3. status  : pointer to the status upon failure
 * .Return value:
 *     =1, success
 *     =0, has error(s)
 ********************************************************************/
u8 M8266WIFI_SPI_STA_Query_Current_SSID_And_RSSI(char* ssid, char* rssi, u16* status);

/********************************************************************
 * M8266WIFI_SPI_StartSmartConfig
 * .Description:
 *     To start/stop smart config of M8266WIFI via SPI
 *     After perform a start operation, the M8266WIFI will be set in STA mode
 * .Parameter(s)
 *     1. en      : to start or stop
 *                  =0, to stop
 *                  others, to start
 *                  if in AP-only mode or ip not achieved, "0.0.0.0" will returned
 *     2. status  : pointer to the status upon failure
 * .Return value:
 *     =1, success
 *     =0, has error(s)
 ********************************************************************/
u8 M8266WIFI_SPI_StartSmartConfig(u8 en, u16* status);


/********************************************************************
 * M8266WIFI_SPI_StartWpsConfig
 * .Description:
 *     To start/stop wps config of M8266WIFI via SPI
 *     After perform a start operation, the M8266WIFI will be set in STA mode
 * .Parameter(s)
 *     1. en      : to start or stop
 *                  =0, to stop
 *                  others, to start
 *                  if in AP-only mode or ip not achieved, "0.0.0.0" will returned
 *     2. status  : pointer to the status upon failure
 * .Return value:
 *     =1, success
 *     =0, has error(s)
 ********************************************************************/
u8 M8266WIFI_SPI_StartWpsConfig(u8 en, u16* status);

/********************************************************************
 * M8266WIFI_SPI_Setup_Connection
 * .Description:
 *     To setup a client connection on M8266WIFI via SPI
 * .Parameter(s)
 *     1. tcp_udp      : connection type
 *                       =0, udp
 *                       =1, tcp
 *     2. local_port   : local_port specified
 *                       =0,  M8266WIFI module will generate a random local port
 *                      !=0, the local_port specified here will be used
 *     3. remote_ip    : string of ip address of remote connection
 *     4. remote_port  : port of remote connection
 *     5. link_no      : the number of link used for multiple links. Max 4
 *     6. status  : pointer to the status upon failure
 * .Return value:
 *     =1, success
 *     =0, has error(s)
 ********************************************************************/
u8 M8266WIFI_SPI_Setup_Connection(u8 tcp_udp, u16 local_port, u8* remote_ip, u16 remote_port, u8 link_no, u16* status);


/********************************************************************
 * M8266WIFI_SPI_Delete_Connection
 * .Description:
 *     To delete/disconnect a client connection on M8266WIFI via SPI
 * .Parameter(s)
 *     1. link_no : the number of link to be deleted/disconnected
 *     2. status  : pointer to the status upon failure
 * .Return value:
 *     =1, success
 *     =0, has error(s)
 ********************************************************************/
 u8 M8266WIFI_SPI_Delete_Connection(u8 link_no, u16* status);
 /********************************************************************
 * M8266WIFI_SPI_Query_Connection
 * .Description:
 *     To Query the information about a connection on M8266WIFI via SPI
 * .Parameter(s)
 *     1. link_no 				: the number of link to be queried
 *     2. connection_type	:	pointer to the connection type returned
 *                          If you don't expected this value returned, just give NULL
 *                          = 0, udp
 *                          = 1, tcp
 *                          = others, invalid
 *     3. connection_state:	pointer to the connection state returned
 *                          If you don't expected this value returned, just give NULL
 *                          = 0, invalid
 *                          = 1, wait
 *                          = 2, listen
 *                          = 3, connect
 *                          = 4, write
 *                          = 5, read
 *                          = 6, close
 *                          = others, invalid
 *     4. local_port      : pointer to the local port returned
 *                          If you don't expected this value returned, just give NULL
 *     5. remote_ip       : pointer to the remote ip returned
 *                          If you don't expected this value returned, just give NULL
 *     6. remote_port     : pointer to the remote port returned
 *                          If you don't expected this value returned, just give NULL
 *     7. status  : pointer to the status upon failure
 *                          If you don't expected this value returned, just give NULL
 * .Return value:
 *     =1, success
 *     =0, has error(s)
 ********************************************************************/
u8 M8266WIFI_SPI_Query_Connection(u8 link_no, u8* connection_type, u8* connection_state, u16* local_port, u8* remote_ip, u16* remote_port, u16* status);






//////////////////////////////////////////////////////////////////////////////////////
// BELOW FUNCTION SPECIFICTIONS ARE REQUIRED BY M8266WIFIDRV.LIB
// PLEASE IMPLEMENTE THEM ACCORDING TO YOUR HARDWARE IN M8266HostIf.c
//////////////////////////////////////////////////////////////////////////////////////
void M8266HostIf_Set_nRESET_Pin(u8 level);
void M8266HostIf_Set_SPI_nCS_Pin(u8 level);
u8   M8266HostIf_SPI_ReadWriteByte(u8 byte);
void M8266HostIf_delay_us(u8 nus);

#endif
