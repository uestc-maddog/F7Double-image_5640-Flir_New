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

u8 ovx_mode=0;							// bit0:0,RGB565ģʽ;1,JPEGģʽ 
u8 buffmask=0;
u8 DISmask =0;
u8 disbuff =0;
u8 dma_mark=0;
u16 curline=0;							// ����ͷ�������,��ǰ�б��
u16 yoffset=0;							// y�����ƫ����

#define jpeg_buf_size   640*240     	// ����JPEG���ݻ���jpeg_buf�Ĵ�С(1*4M�ֽ�)
#define jpeg_line_size	19200		    // ����DMA��������ʱ,һ�����ݵ����ֵ
#define threshold_rgb   2

u32 dcmi_line_buf[2][jpeg_line_size];   // __attribute__((at(0XC1000000+320*240*4)));	// RGB��ʱ,����ͷ����һ��һ�ж�ȡ,�����л���  
u32 jpeg_data_buf[2][jpeg_buf_size] __attribute__((at(0XC0000000+1280*800*2)));     // JPEG���ݻ���buf,������LCD֡����֮��
uint16_t rgbbuf2[240][320] __attribute__((at(0XC1000000+320*240*2)));
uint16_t sendbuf[100][30400] __attribute__((at(0XC1000000+320*240*4)));
uint16_t sendfps = 0;

void mculcd_dcmi_rx_callback(void);     //MCU�����ݽ��ջص�����
void OV5640_Config(void);               // OV5640����
void Flir_Display(void);
extern HAL_StatusTypeDef status2;
u32 zhenshu = 0;

u16 xx[60][82];    //Flir ����
extern volatile uint16_t nbuf2[70][82];

volatile u32 jpeg_data_len=0; 			//buf�е�JPEG��Ч���ݳ��� 
volatile u8 jpeg_data_ok=0;				//JPEG���ݲɼ���ɱ�־ 
extern u8 data_mask;

										//0,����û�вɼ���;
										//1,���ݲɼ�����,���ǻ�û����;
										//2,�����Ѿ����������,���Կ�ʼ��һ֡����

int main(void)
{ 
	
    Cache_Enable();                 // ��L1-Cache
    MPU_Memory_Protection();        // ������ش洢����
    HAL_Init();				        // ��ʼ��HAL��
    Stm32_Clock_Init(432,25,2,9);   // ����ʱ��,216Mhz 
    delay_init(216);                // ��ʱ��ʼ��
	uart_init(1382400);		        // ���ڳ�ʼ��
    LED_Init();                     // ��ʼ��LED
	Power_Init();                   // ��ʼ����ʹ�������Դ
    KEY_Init();                     // ��ʼ������
	MX_I2C4_Init();
	SPI2_Init();
    SDRAM_Init();                   // ��ʼ��SDRAM
    LCD_Init();                     // ��ʼ��LCD
	
	lepton_init();  
	//TIM3_Init(10000-1,10800-1);     // 10Khz����,1�����ж�һ��
 	POINT_COLOR=RED;                // ��������Ϊ��ɫ 
	LCD_ShowString(30,50,200,16,16,(u8*)"Double Image");	
	LCD_Clear(RED); 	
	delay_ms(500);
	while(OV5640_Init())            // ��ʼ��OV5640
	{
		LCD_ShowString(30,130,240,16,16,(u8*)"OV5640 ERR");
		delay_ms(200);
	    LCD_Fill(30,130,239,170,WHITE);
		delay_ms(200);
        LED0_Toggle;
	}	
    LCD_ShowString(30,130,200,16,16,(u8*)"OV5640 OK"); 
	OV5640_Config();
	M8266Wifi_Init();               // ��ʼ��wifi����

	init_lepton_command_interface();
	delay_ms(200);
	enable_FFC();

	Flir_Display();		
}

// OV5640����
void OV5640_Config(void)
{ 
	u16 outputheight=0;
	
	LCD_Clear(WHITE);
    POINT_COLOR=RED; 
	LCD_ShowString(30,50,200,16,16,(u8*)"Doule image test!");
	//�Զ��Խ���ʼ��
	OV5640_RGB565_Mode();	//RGB565ģʽ	
	OV5640_Focus_Init();
	OV5640_Light_Mode(0);	//�Զ�ģʽ
	OV5640_Color_Saturation(6);//ɫ�ʱ��Ͷ�0
	OV5640_Brightness(1);	//����0
	OV5640_Contrast(6);		//�Աȶ�0
	OV5640_Sharpness(20);	//�Զ����
	OV5640_Focus_Constant();//���������Խ�
	DCMI_Init();			//DCMI����
	
	lcddev.width=320;
	lcddev.height=240;
	
	dcmi_rx_callback=mculcd_dcmi_rx_callback;
	DCMI_DMA_Init((u32)jpeg_data_buf[0],(u32)jpeg_data_buf[1],38400,DMA_MDATAALIGN_HALFWORD,DMA_MINC_ENABLE);
	
	TIM3->CR1&=~(0x01);		//�رն�ʱ��3,�ر�֡��ͳ�ƣ��򿪵Ļ���RGB�����ڴ��ڴ�ӡ��ʱ�򣬻ᶶ

	yoffset=0;
	outputheight=lcddev.height;
	OV5640_WR_Reg(0x3035,0X51);//�������֡�ʣ�������ܶ���

	curline=yoffset;		//������λ
	OV5640_OutSize_Set(16,4,lcddev.width,outputheight);		//����������ʾ
	HAL_Delay(800);
	curline = 1;
	DCMI_Start(); 			//�������䡢
	HAL_Delay(800);
	OV5640_Special_Effects(3); //�����Ҷ�ģʽ
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
	u8 *wifi_date;                          // wifi����������ָ��
	u8 FrameHeader[3] = {0x55,0xaa,0x55};   // UDP�շ�ͬ��֡ͷ
	
	LCD_Clear(WHITE);
	while(1)        
	{
		SPI2_Readbuff((u8 *)xx,82); //��ȡ��һ��     
		if((xx[0][0] & 0x00ff) == 0)
		{
			SPI2_Readbuff((u8 *)(&xx[1][0]),82*59);
			if((xx[59][0] & 0xff) == 59)
			{
				jpeg=(u16 *)jpeg_data_buf[disbuff];

#ifdef WifiSend_ON				
				for(j=0;j<320*80;j++)               // OV5640_Data       UDP Wifi_Data����
				{
					i = j * 3;
					sendbuf[sendfps][j] = (jpeg[i] & 0x1f) + ((jpeg[i+1] & 0x1f)<<5) + ((jpeg[i+2] & 0x1f)<<10);
				}
				for(i=0;i<60;i++)                   // Flir_Data         UDP Wifi_Data����
				{
					for(j=0;j<80;j++)
					{
						sendbuf[sendfps][(i+320)*80+j] = xx[i][j+2];
					}
				}			
				
				//WIFI���� sendbuf[sendfps][30400] ��һ֡���ݼ���
				//���� 0 - 320*80*2��51200��Ϊ����ͷ����    51200 - 30400*2 ΪFlir ԭʼ���ݡ�
				/*************************wifi����***********************/
				wifi_date = (u8*)sendbuf[sendfps];                        // 30400 * 16bit ------> 60800 * 8bit
				M8266WIFI_SPI_Send_Data(FrameHeader, 3, 0, &status);      // 3Bͬ��֡
				for(i = 0; i < Times; i++)                            // 800B * 76 = 60800B 
				{
					sent = M8266WIFI_SPI_Send_Data(wifi_date+(i*Bytes), Bytes, 0, &status); // ����6080B���� ��sent:SPI�ӿڴ��ݳ�ȥ�ĸ���
				}
				printf("status=%d\r\n", status);
				//printf("sent=%d,status=%d\r\n", Bytes*i, status);
				/*************************wifi����***********************/
#endif	
				
#ifdef LCD_ON
//				for(i=0;i<30;i++)
//				{
//					for(j=0;j<320;j++)
//					{
//						rgbbuf2[i][j] =0;
//					}
//				}
				
				for(i=0;i<240;i++)                  // LCD��ʾ���棨OV5640_Data��    ��ά����תһά����
				{
					for(j=0;j<320;j++)
					{
						rgbbuf2[i][j] = jpeg[i*320+j] & 0x1f;       
					}
				}
				for(i=0;i<240;i++)                  // 240*320 OV5640_Dataͼ����
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
				for(i=0;i<60;i++)            // �ҳ����ֵ�;�ֵ	
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
				// Flir ��� AGC + �����	
				tempreter = (int)(max_data - 8192)*0.026 + 38; //����¶�
				aveg = aveg / 4800;                            //�¶�ƽ��ֵ
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

//MCU�����ݽ��ջص�����
void mculcd_dcmi_rx_callback(void)
{  
	if(DMA2_Stream1->CR&(1<<19))//DMAʹ��buf1,��ȡbuf0
	{ 
		disbuff = 0;
	}
	else 						//DMAʹ��buf0,��ȡbuf1
	{
		disbuff = 1;
	} 	
	
}	
