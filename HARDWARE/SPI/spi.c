#include "spi.h"

// SPI2&SPI4驱动代码

SPI_HandleTypeDef hspi2;    //SPI2句柄
SPI_HandleTypeDef hspi4;    //SPI4句柄

DMA_HandleTypeDef hdma_spi2_rx;
DMA_HandleTypeDef hdma_spi2_tx;
extern u16 xx[60][82];    //Flir 数据
u8 data_mask=0;

//以下是SPI模块的初始化代码，配置成主机模式 						  
//SPI口初始化
//这里针是对SPI2的初始化
void SPI2_Init(void)
{
	
		  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);
  /* DMA1_Stream4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);

  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_16BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi2.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi2.Init.NSS = SPI_NSS_HARD_OUTPUT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 7;
  hspi2.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi2.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
 //   Error_Handler();
  }
	

}

void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	if(hspi->Instance==SPI2)
	{
		/* USER CODE BEGIN SPI2_MspInit 0 */

		/* USER CODE END SPI2_MspInit 0 */
		/* Peripheral clock enable */
		__HAL_RCC_SPI2_CLK_ENABLE();
			__HAL_RCC_GPIOB_CLK_ENABLE();			//开启GPIOB时钟
		/**SPI2 GPIO Configuration    
		PB13     ------> SPI2_SCK
		PB14     ------> SPI2_MISO
		PB15     ------> SPI2_MOSI 
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
			   
			GPIO_InitStruct.Pin = GPIO_PIN_12;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		/* Peripheral DMA init*/
	  
		hdma_spi2_rx.Instance = DMA1_Stream3;
		hdma_spi2_rx.Init.Channel = DMA_CHANNEL_0;
		hdma_spi2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
		hdma_spi2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_spi2_rx.Init.MemInc = DMA_MINC_ENABLE;
		hdma_spi2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
		hdma_spi2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
		hdma_spi2_rx.Init.Mode = DMA_NORMAL;
		hdma_spi2_rx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
		hdma_spi2_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		if (HAL_DMA_Init(&hdma_spi2_rx) != HAL_OK)
		{
		//    Error_Handler();
		}

		__HAL_LINKDMA(hspi,hdmarx,hdma_spi2_rx);

		hdma_spi2_tx.Instance = DMA1_Stream4;
		hdma_spi2_tx.Init.Channel = DMA_CHANNEL_0;
		hdma_spi2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
		hdma_spi2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_spi2_tx.Init.MemInc = DMA_MINC_ENABLE;
		hdma_spi2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
		hdma_spi2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
		hdma_spi2_tx.Init.Mode = DMA_NORMAL;
		hdma_spi2_tx.Init.Priority = DMA_PRIORITY_LOW;
		hdma_spi2_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		if (HAL_DMA_Init(&hdma_spi2_tx) != HAL_OK)
		{
		//    Error_Handler();
		}

		__HAL_LINKDMA(hspi,hdmatx,hdma_spi2_tx);

		/* USER CODE BEGIN SPI2_MspInit 1 */

		/* USER CODE END SPI2_MspInit 1 */
	}
	if(hspi->Instance==SPI4)
	{
		__HAL_RCC_GPIOE_CLK_ENABLE();			//开启GPIOE时钟
		__HAL_RCC_SPI4_CLK_ENABLE();
		
		GPIO_InitStruct.Pin       = GPIO_PIN_2 | GPIO_PIN_5 | GPIO_PIN_6;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
		GPIO_InitStruct.Pull      = GPIO_PULLUP;
		GPIO_InitStruct.Alternate = GPIO_AF5_SPI4;	
		HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);        
	}
}


/**
* @brief This function handles DMA1 stream4 global interrupt.
*/
void DMA1_Stream4_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream4_IRQn 0 */

  /* USER CODE END DMA1_Stream4_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_spi2_tx);
  /* USER CODE BEGIN DMA1_Stream4_IRQn 1 */

  /* USER CODE END DMA1_Stream4_IRQn 1 */
}

//SPI速度设置函数
//SPI速度=fAPB1/分频系数
//@ref SPI_BaudRate_Prescaler:SPI_BAUDRATEPRESCALER_2~SPI_BAUDRATEPRESCALER_2 256
//fAPB1时钟一般为54Mhz：
void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//判断有效性
    __HAL_SPI_DISABLE(&hspi2);            //关闭SPI
    hspi2.Instance->CR1&=0XFFC7;          //位3-5清零，用来设置波特率
    hspi2.Instance->CR1|=SPI_BaudRatePrescaler;//设置SPI速度
    __HAL_SPI_ENABLE(&hspi2);             //使能SPI
    
}

//SPI2 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 SPI2_ReadWriteByte(u8 TxData)
{
    u8 Rxdata;
    HAL_SPI_TransmitReceive(&hspi2,&TxData,&Rxdata,1, 1000);       
 	return Rxdata;          		    //返回收到的数据		
}


u8 SPI2_Readbuff(u8 *TxData,u16 x)
{
    u8 Rxdata=1;
    HAL_SPI_Receive(&hspi2,TxData,x,1000);       
  	return Rxdata;          		    //返回收到的数据		
}


// SPI4初始化代码  SPI4 CLK = 108MHz
void SPI4_SetSpeed(u8 SPI_BaudRatePrescaler)
{
	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//判断有效性
    __HAL_SPI_DISABLE(&hspi4);            //关闭SPI
    hspi4.Instance->CR1&=0XFFC7;          //位3-5清零，用来设置波特率
    hspi4.Instance->CR1|=SPI_BaudRatePrescaler;//设置SPI速度
    __HAL_SPI_ENABLE(&hspi4);             //使能SPI
}

//SPI4 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 SPI4_ReadWriteByte(u8 TxData)
{
    u8 Rxdata;
    HAL_SPI_TransmitReceive(&hspi4,&TxData,&Rxdata,1, 1000);       
 	return Rxdata;          		    //返回收到的数据		
}

void SPI4_Init(void)
{
	hspi4.Instance = SPI4;
	hspi4.Init.Mode = SPI_MODE_MASTER;
	hspi4.Init.Direction = SPI_DIRECTION_2LINES;
	hspi4.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi4.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi4.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi4.Init.NSS = SPI_NSS_SOFT;
	hspi4.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;    //SPI4 CLK = 108MHz, 108/8=13.5MHz
	//hspi4.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;    //SPI4 CLK = 108MHz, 108/4=27MHz
	hspi4.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi4.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi4.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi4.Init.CRCPolynomial = 7;
	hspi4.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
	hspi4.Init.NSSPMode = SPI_NSS_PULSE_DISABLE; //SPI_NSS_PULSE_ENABLE;
	if (HAL_SPI_Init(&hspi4) != HAL_OK)
	{
		printf("SPI4 Init Error!\r\n");
	}
	__HAL_SPI_ENABLE(&hspi4);                        //使能SPI4
}

