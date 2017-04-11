#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "ltdc.h"
#include "sdram.h"
#include "pcf8574.h"
#include "timer.h" 
#include "ov5640.h"
#include "usart2.h"
#include "dcmi.h"
#include "mpu.h"
#include "usmart.h"
#include "lepton.h"
#include "lepton_i2c.h"
#include "spi.h"
#include "M8266App.h"

u8* SSID = (u8*)"MadDog";
u8* PWD  = (u8*)"1234abcd";

#define Bytes (800U)                    // 800B * 76 = 60800B
#define Times (76U)

//#define LCD_ON      1
#define WifiSend_ON 1

u8 ovx_mode=0;							// bit0:0,RGB565模式;1,JPEG模式 
u8 buffmask=0;
u8 DISmask =0;
u8 disbuff =0;
u8 dma_mark=0;
u16 curline=0;							// 摄像头输出数据,当前行编号
u16 yoffset=0;							// y方向的偏移量

#define jpeg_buf_size   640*240     	// 定义JPEG数据缓存jpeg_buf的大小(1*4M字节)
#define jpeg_line_size	19200		    // 定义DMA接收数据时,一行数据的最大值
#define threshold_rgb   2

u32 dcmi_line_buf[2][jpeg_line_size];   // __attribute__((at(0XC1000000+320*240*4)));	// RGB屏时,摄像头采用一行一行读取,定义行缓存  
u32 jpeg_data_buf[2][jpeg_buf_size] __attribute__((at(0XC0000000+1280*800*2)));     // JPEG数据缓存buf,定义在LCD帧缓存之后
uint16_t rgbbuf2[240][320] __attribute__((at(0XC1000000+320*240*2)));
uint16_t sendbuf[100][30400] __attribute__((at(0XC1000000+320*240*4)));
uint16_t sendfps = 0;

void mculcd_dcmi_rx_callback(void);     //MCU屏数据接收回调函数
void OV5640_Config(void);               // OV5640配置
void Flir_Display(void);
extern HAL_StatusTypeDef status2;
u32 zhenshu = 0;

u16 xx[60][82];    //Flir 数据
extern volatile uint16_t nbuf2[70][82];

volatile u32 jpeg_data_len=0; 			//buf中的JPEG有效数据长度 
volatile u8 jpeg_data_ok=0;				//JPEG数据采集完成标志 
extern u8 data_mask;

										//0,数据没有采集完;
										//1,数据采集完了,但是还没处理;
										//2,数据已经处理完成了,可以开始下一帧接收

int main(void)
{ 
	
    Cache_Enable();                 // 打开L1-Cache
    MPU_Memory_Protection();        // 保护相关存储区域
    HAL_Init();				        // 初始化HAL库
    Stm32_Clock_Init(432,25,2,9);   // 设置时钟,216Mhz 
    delay_init(216);                // 延时初始化
	uart_init(1382400);		        // 串口初始化
    LED_Init();                     // 初始化LED
	Power_Init();                   // 初始化、使能外设电源
    KEY_Init();                     // 初始化按键
	MX_I2C4_Init();
	SPI2_Init();
    SDRAM_Init();                   // 初始化SDRAM
    LCD_Init();                     // 初始化LCD
	
	lepton_init();  
	//TIM3_Init(10000-1,10800-1);     // 10Khz计数,1秒钟中断一次
 	POINT_COLOR=RED;                // 设置字体为红色 
	LCD_ShowString(30,50,200,16,16,(u8*)"Double Image");	
	LCD_Clear(RED); 	
	delay_ms(500);
	while(OV5640_Init())            // 初始化OV5640
	{
		LCD_ShowString(30,130,240,16,16,(u8*)"OV5640 ERR");
		delay_ms(200);
	    LCD_Fill(30,130,239,170,WHITE);
		delay_ms(200);
        LED0_Toggle;
	}	
    LCD_ShowString(30,130,200,16,16,(u8*)"OV5640 OK"); 
	OV5640_Config();
	M8266Wifi_Init();               // 初始化wifi配置

	init_lepton_command_interface();
	delay_ms(200);
	enable_FFC();

	Flir_Display();		
}

// OV5640配置
void OV5640_Config(void)
{ 
	u16 outputheight=0;
	
	LCD_Clear(WHITE);
    POINT_COLOR=RED; 
	LCD_ShowString(30,50,200,16,16,(u8*)"Doule image test!");
	//自动对焦初始化
	OV5640_RGB565_Mode();	//RGB565模式	
	OV5640_Focus_Init();
	OV5640_Light_Mode(0);	//自动模式
	OV5640_Color_Saturation(6);//色彩饱和度0
	OV5640_Brightness(1);	//亮度0
	OV5640_Contrast(6);		//对比度0
	OV5640_Sharpness(20);	//自动锐度
	OV5640_Focus_Constant();//启动持续对焦
	DCMI_Init();			//DCMI配置
	
	lcddev.width=320;
	lcddev.height=240;
	
	dcmi_rx_callback=mculcd_dcmi_rx_callback;
	DCMI_DMA_Init((u32)jpeg_data_buf[0],(u32)jpeg_data_buf[1],38400,DMA_MDATAALIGN_HALFWORD,DMA_MINC_ENABLE);
	
	TIM3->CR1&=~(0x01);		//关闭定时器3,关闭帧率统计，打开的话，RGB屏，在串口打印的时候，会抖

	yoffset=0;
	outputheight=lcddev.height;
	OV5640_WR_Reg(0x3035,0X51);//降低输出帧率，否则可能抖动

	curline=yoffset;		//行数复位
	OV5640_OutSize_Set(16,4,lcddev.width,outputheight);		//满屏缩放显示
	HAL_Delay(800);
	curline = 1;
	DCMI_Start(); 			//启动传输、
	HAL_Delay(800);
	OV5640_Special_Effects(3); //开启灰度模式
	sendfps = 1;
}

	

/**
* @brief This function handles DMA2 stream7 global interrupt.
*/
void DMA2_Stream7_IRQHandler(void)
{
	/* USER CODE BEGIN DMA2_Stream7_IRQn 0 */

	/* USER CODE END DMA2_Stream7_IRQn 0 */
	HAL_DMA_IRQHandler(&hdma_usart1_tx);
	/* USER CODE BEGIN DMA2_Stream7_IRQn 1 */
	sendfps = 1;
	/* USER CODE END DMA2_Stream7_IRQn 1 */
}

void Flir_Display(void)
{
	float index;
	u16 u,u1,u2,u3,u4,sent;
	u16 max_data,max_data_i,max_data_j,status;
	u32 i,j;
	u16 *jpeg;
	u32 aveg; 
	int tempreter = 0;
	u8 *wifi_date;                          // wifi待发送数据指针
	u8 FrameHeader[3] = {0x55,0xaa,0x55};   // UDP收发同步帧头
	
	LCD_Clear(WHITE);
	while(1)        
	{
		SPI2_Readbuff((u8 *)xx,82); //读取第一行     
		if((xx[0][0] & 0x00ff) == 0)
		{
			SPI2_Readbuff((u8 *)(&xx[1][0]),82*59);
			if((xx[59][0] & 0xff) == 59)
			{
				jpeg=(u16 *)jpeg_data_buf[disbuff];

#ifdef WifiSend_ON				
				for(j=0;j<320*80;j++)               // OV5640_Data       UDP Wifi_Data缓存
				{
					i = j * 3;
					sendbuf[sendfps][j] = (jpeg[i] & 0x1f) + ((jpeg[i+1] & 0x1f)<<5) + ((jpeg[i+2] & 0x1f)<<10);
				}
				for(i=0;i<60;i++)                   // Flir_Data         UDP Wifi_Data缓存
				{
					for(j=0;j<80;j++)
					{
						sendbuf[sendfps][(i+320)*80+j] = xx[i][j+2];
					}
				}			
				
				//WIFI发送 sendbuf[sendfps][30400] 这一帧数据即可
				//数据 0 - 320*80*2（51200）为摄像头数据    51200 - 30400*2 为Flir 原始数据。
				/*************************wifi发送***********************/
				wifi_date = (u8*)sendbuf[sendfps];                        // 30400 * 16bit ------> 60800 * 8bit
				M8266WIFI_SPI_Send_Data(FrameHeader, 3, 0, &status);      // 3B同步帧
				for(i = 0; i < Times; i++)                            // 800B * 76 = 60800B 
				{
					sent = M8266WIFI_SPI_Send_Data(wifi_date+(i*Bytes), Bytes, 0, &status); // 发送6080B数据 ，sent:SPI接口传递出去的个数
				}
				printf("status=%d\r\n", status);
				//printf("sent=%d,status=%d\r\n", Bytes*i, status);
				/*************************wifi发送***********************/
#endif	
				
#ifdef LCD_ON
//				for(i=0;i<30;i++)
//				{
//					for(j=0;j<320;j++)
//					{
//						rgbbuf2[i][j] =0;
//					}
//				}
				
				for(i=0;i<240;i++)                  // LCD显示缓存（OV5640_Data）    二维数组转一维数组
				{
					for(j=0;j<320;j++)
					{
						rgbbuf2[i][j] = jpeg[i*320+j] & 0x1f;       
					}
				}
				for(i=0;i<240;i++)                  // 240*320 OV5640_Data图像处理
				{
					for(j=0;j<320;j++)
					{
						if(((int)(rgbbuf2[i+1][j] - rgbbuf2[i][j]) > threshold_rgb)||((int)(rgbbuf2[i+1][j] - rgbbuf2[i][j]) < -threshold_rgb))
							rgbbuf2[i][j] = rgbbuf2[i][j]/2;
						else if(((int)(rgbbuf2[i+1][j+1] - rgbbuf2[i][j+1]) > threshold_rgb)||((int)(rgbbuf2[i+1][j+1] - rgbbuf2[i][j]) < -threshold_rgb))
							rgbbuf2[i][j] = rgbbuf2[i][j]/2;
						else if(((int)(rgbbuf2[i][j+1] - rgbbuf2[i][j]) > threshold_rgb)||((int)(rgbbuf2[i][j+1] - rgbbuf2[i][j]) < -threshold_rgb))
							rgbbuf2[i][j] = rgbbuf2[i][j]/2;
						else
							rgbbuf2[i][j] = rgbbuf2[i][j]/10;
					}
				}
				
				aveg = 0;
				max_data = 0;
				for(i=0;i<60;i++)            // 找出最大值和均值	
				{
					for(j=0;j<80;j++)
					{
						if((max_data < xx[i][j+2]) && (xx[i][j+2] < 0x3FFF))
						{
							max_data = xx[i][j+2];
							max_data_i = i;
							max_data_j = j;
						}
						aveg = aveg + xx[i][j+2] ;
					}
				}	
				// Flir 软件 AGC + 抗锯齿	
				tempreter = (int)(max_data - 8192)*0.026 + 38; //最大处温度
				aveg = aveg / 4800;                            //温度平均值
				index = (float)80 / (float)(max_data - aveg);

				for(i=0;i<60;i++)          // 60*80-->240*320
				{
					for(j=0;j<80;j++)
					{
						u=(int)(xx[i][j+2]-aveg)*index + 175;
						if(j < 79)
							u1=(int)(xx[i][j+3]-aveg)*index + 175;
						else
							u1=(int)(xx[i][j+2]-aveg)*index + 175;
						if(i < 59)
							u2=(int)(xx[i+1][j+2]-aveg)*index + 175;
						else
							u2=(int)(xx[i][j+2]-aveg)*index + 175;
						if(i > 1)
							u3=(int)(xx[i-1][j+2]-aveg)*index + 175;
						else
							u3=(int)(xx[i][j+2]-aveg)*index + 175;
						if(j > 1)
							u4=(int)(xx[i][j+1]-aveg)*index + 175;
						else
							u4=(int)(xx[i][j+2]-aveg)*index + 175;

						rgbbuf[4*i][4*j]  =YUV2RGB422((u3+u4)/2);
						rgbbuf[4*i][4*j+1]=YUV2RGB422((u+u3)/2);
						rgbbuf[4*i][4*j+2]=YUV2RGB422((u+u3)/2);
						rgbbuf[4*i][4*j+3]=YUV2RGB422((u1+u3)/2);

						rgbbuf[4*i+1][4*j]  =YUV2RGB422((u+u4)/2);
						rgbbuf[4*i+1][4*j+1]=YUV2RGB422(u);
						rgbbuf[4*i+1][4*j+2]=YUV2RGB422(u);
						rgbbuf[4*i+1][4*j+3]=YUV2RGB422((u+u1)/2);

						rgbbuf[4*i+2][4*j]=YUV2RGB422((u+u4)/2);
						rgbbuf[4*i+2][4*j+1]=YUV2RGB422(u);
						rgbbuf[4*i+2][4*j+2]=YUV2RGB422(u);
						rgbbuf[4*i+2][4*j+3]=YUV2RGB422((u+u1)/2);

						rgbbuf[4*i+3][4*j]  =YUV2RGB422((u2+u4)/2);
						rgbbuf[4*i+3][4*j+1]=YUV2RGB422((u+u2)/2);
						rgbbuf[4*i+3][4*j+2]=YUV2RGB422((u+u2)/2);
						rgbbuf[4*i+3][4*j+3]=YUV2RGB422((u2+u1)/2);
					}
				}
				
				//LCD_Color_Fill(0,0,319,239,(u16 *)rgbbuf2);
				LCD_two_Color_Fill(0,0,319,239,(u16 *)rgbbuf,(u16 *)rgbbuf2);
				
				
				LCD_Draw_Circle(max_data_j*4,max_data_i*4,5);
				LCD_ShowxNum(max_data_j*4,max_data_i*4,(u8)tempreter,4,16,1);
#endif
				printf("frame%d\r\n", zhenshu++);
				LCD_ShowxNum(0,0,zhenshu,8,16,0);
			}
		}
		else
		{
			HAL_Delay(1);
			//printf("Flir Data Error\r\n");
		}
	}
}

//MCU屏数据接收回调函数
void mculcd_dcmi_rx_callback(void)
{  
	if(DMA2_Stream1->CR&(1<<19))//DMA使用buf1,读取buf0
	{ 
		disbuff = 0;
	}
	else 						//DMA使用buf0,读取buf1
	{
		disbuff = 1;
	} 	
	
}	
