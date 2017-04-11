#include <stdio.h>
#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_spi.h"
#include <string.h>
#include "lepton.h"
#include "project_config.h"
#include "delay.h"
#include "spi.h"
#include "lcd.h"

#ifdef USART_DEBUG
#define DEBUG_PRINTF(...) printf( __VA_ARGS__);
#else
#define DEBUG_PRINTF(...)
#endif

#define LEPTON_USART_PORT (USART2)
#define Y 128
#define LEPTON_RESET_L_HIGH	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15 , GPIO_PIN_SET)
#define LEPTON_RESET_L_LOW	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET)

#define LEPTON_PW_DWN_HIGH	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET)
#define LEPTON_PW_DWN_LOW	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET)

extern SPI_HandleTypeDef hspi2;
extern DMA_HandleTypeDef hdma_spi2_rx;

#define RING_SIZE (4)
#define TUNE(r) ( r < 0 ? 0 : (r > 255 ? 255 : r) )


const int colormap_ironblack[] = {255, 255, 255, 253, 253, 253, 251, 251, 251, 
	249, 249, 249, 247, 247, 247, 245, 245, 245, 243, 
	243, 243, 241, 241, 241, 239, 239, 239, 237, 237, 
	237, 235, 235, 235, 233, 233, 233, 231, 231, 231, 
	229, 229, 229, 227, 227, 227, 225, 225, 225, 223, 
	223, 223, 221, 221, 221, 219, 219, 219, 217, 217, 
	217, 215, 215, 215, 213, 213, 213, 211, 211, 211, 
	209, 209, 209, 207, 207, 207, 205, 205, 205, 203, 
	203, 203, 201, 201, 201, 199, 199, 199, 197, 197, 
	197, 195, 195, 195, 193, 193, 193, 191, 191, 191, 
	189, 189, 189, 187, 187, 187, 185, 185, 185, 183, 
	183, 183, 181, 181, 181, 179, 179, 179, 177, 177, 
	177, 175, 175, 175, 173, 173, 173, 171, 171, 171, 
	169, 169, 169, 167, 167, 167, 165, 165, 165, 163, 
	163, 163, 161, 161, 161, 159, 159, 159, 157, 157, 
	157, 155, 155, 155, 153, 153, 153, 151, 151, 151, 
	149, 149, 149, 147, 147, 147, 145, 145, 145, 143, 
	143, 143, 141, 141, 141, 139, 139, 139, 137, 137, 
	137, 135, 135, 135, 133, 133, 133, 131, 131, 131, 
	129, 129, 129, 126, 126, 126, 124, 124, 124, 122, 
	122, 122, 120, 120, 120, 118, 118, 118, 116, 116, 
	116, 114, 114, 114, 112, 112, 112, 110, 110, 110, 
	108, 108, 108, 106, 106, 106, 104, 104, 104, 102, 
	102, 102, 100, 100, 100, 98, 98, 98, 96, 96, 96, 
	94, 94, 94, 92, 92, 92, 90, 90, 90, 88, 88, 88, 
	86, 86, 86, 84, 84, 84, 82, 82, 82, 80, 80, 80, 
	78, 78, 78, 76, 76, 76, 74, 74, 74, 72, 72, 72, 
	70, 70, 70, 68, 68, 68, 66, 66, 66, 64, 64, 64, 
	62, 62, 62, 60, 60, 60, 58, 58, 58, 56, 56, 56, 
	54, 54, 54, 52, 52, 52, 50, 50, 50, 48, 48, 48, 
	46, 46, 46, 44, 44, 44, 42, 42, 42, 40, 40, 40, 
	38, 38, 38, 36, 36, 36, 34, 34, 34, 32, 32, 32, 
	30, 30, 30, 28, 28, 28, 26, 26, 26, 24, 24, 24, 
	22, 22, 22, 20, 20, 20, 18, 18, 18, 16, 16, 16, 
	14, 14, 14, 12, 12, 12, 10, 10, 10, 8, 8, 8, 6, 
	6, 6, 4, 4, 4, 2, 2, 2, 0, 0, 0, 0, 0, 9, 2, 0, 
	16, 4, 0, 24, 6, 0, 31, 8, 0, 38, 10, 0, 45, 12, 
	0, 53, 14, 0, 60, 17, 0, 67, 19, 0, 74, 21, 0, 
	82, 23, 0, 89, 25, 0, 96, 27, 0, 103, 29, 0, 
	111, 31, 0, 118, 36, 0, 120, 41, 0, 121, 46, 
	0, 122, 51, 0, 123, 56, 0, 124, 61, 0, 125, 
	66, 0, 126, 71, 0, 127, 76, 1, 128, 81, 1, 129,
	86, 1, 130, 91, 1, 131, 96, 1, 132, 101, 1, 133,
	106, 1, 134, 111, 1, 135, 116, 1, 136, 121, 1, 
	136, 125, 2, 137, 130, 2, 137, 135, 3, 137, 139, 
	3, 138, 144, 3, 138, 149, 4, 138, 153, 4, 139, 158,
	5, 139, 163, 5, 139, 167, 5, 140, 172, 6, 140, 177, 
	6, 140, 181, 7, 141, 186, 7, 141, 189, 10, 137, 191,
	13, 132, 194, 16, 127, 196, 19, 121, 198, 22, 116, 
	200, 25, 111, 203, 28, 106, 205, 31, 101, 207, 34,
	95, 209, 37, 90, 212, 40, 85, 214, 43, 80, 216, 46,
	75, 218, 49, 69, 221, 52, 64, 223, 55, 59, 224, 57,
	49, 225, 60, 47, 226, 64, 44, 227, 67, 42, 228, 71,
	39, 229, 74, 37, 230, 78, 34, 231, 81, 32, 231, 85,
	29, 232, 88, 27, 233, 92, 24, 234, 95, 22, 235, 99,
	19, 236, 102, 17, 237, 106, 14, 238, 109, 12, 239, 
	112, 12, 240, 116, 12, 240, 119, 12, 241, 123, 12, 
	241, 127, 12, 242, 130, 12, 242, 134, 12, 243, 138, 
	12, 243, 141, 13, 244, 145, 13, 244, 149, 13, 245, 
	152, 13, 245, 156, 13, 246, 160, 13, 246, 163, 13, 
	247, 167, 13, 247, 171, 13, 248, 175, 14, 248, 178, 
	15, 249, 182, 16, 249, 185, 18, 250, 189, 19, 250, 
	192, 20, 251, 196, 21, 251, 199, 22, 252, 203, 23, 
	252, 206, 24, 253, 210, 25, 253, 213, 27, 254, 217, 
	28, 254, 220, 29, 255, 224, 30, 255, 227, 39, 255, 
	229, 53, 255, 231, 67, 255, 233, 81, 255, 234, 95, 
	255, 236, 109, 255, 238, 123, 255, 240, 137, 255, 
	242, 151, 255, 244, 165, 255, 246, 179, 255, 248, 
193, 255, 249, 207, 255, 251, 221, 255, 253, 235, 255, 255, 24};

volatile uint16_t nbuf2[70][82];
//volatile uint16_t rgbbuf[480][640];
uint16_t rgbbuf[240][320] __attribute__((at(0XC1000000)));
lepton_xfer_state xfer_state = LEPTON_XFER_STATE_START;


// These replace HAL library functions as they're a lot shorter and more specialized
static HAL_StatusTypeDef setup_lepton_spi_rx(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size);



lepton_status complete_lepton_transfer(lepton_buffer* buffer)
{
  // TODO: additional synchronization desired?
  return buffer->status;
}
lepton_buffer m;

u8 dma_work_mask = 1;
lepton_buffer* lepton_transfer(void)
{
  lepton_buffer *buf;
  HAL_StatusTypeDef status;
	buf=&m;
  // DEBUG_PRINTF("Transfer starting: %p@%p\r\n", buf, packet);


  if (status != HAL_OK)
  {
    DEBUG_PRINTF("Error setting up SPI DMA receive: %d\r\n", status);
    buf->status = LEPTON_STATUS_RESYNC;
    return buf;
  }

  buf->status = LEPTON_STATUS_TRANSFERRING;
  return buf;
}

void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
  DEBUG_PRINTF("SPI error!\n\r");
}
HAL_StatusTypeDef status2=HAL_ERROR;

void lepton_spi_rx_dma_cplt(void)
{
	xfer_state = LEPTON_XFER_STATE_LAST;
	dma_work_mask=1;
}



/**
* @brief This function handles DMA1 stream1 global interrupt.
*/
void DMA1_Stream3_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream1_IRQn 0 */

  /* USER CODE END DMA1_Stream1_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_spi2_rx);
  /* USER CODE BEGIN DMA1_Stream1_IRQn 1 */
	lepton_spi_rx_dma_cplt();

  /* USER CODE END DMA1_Stream1_IRQn 1 */
}

void lepton_init(void )
{

  LEPTON_RESET_L_LOW;
  LEPTON_PW_DWN_LOW;

  delay_ms(200);
  LEPTON_PW_DWN_HIGH;

	delay_ms(200);
  LEPTON_RESET_L_HIGH;


}


static HAL_StatusTypeDef setup_lepton_spi_rx(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size)
{
  /* Configure communication */


  /* Enable the Tx DMA Stream */
 // start_lepton_spi_dma(hspi->hdmatx, (uint32_t)hspi->pTxBuffPtr, (uint32_t)&hspi->Instance->DR, hspi->TxXferCount);

  /* Enable the Rx DMA Stream */
 // start_lepton_spi_dma(hspi->hdmarx, (uint32_t)&hspi->Instance->DR, (uint32_t)hspi->pRxBuffPtr, hspi->RxXferCount);
	HAL_SPI_Receive_DMA(hspi,(uint8_t*)pData,Size);
	dma_work_mask = 0;
  /* Enable Rx DMA Request */
  /* Enable Tx DMA Request */
 // hspi->Instance->CR2 |= SPI_CR2_RXDMAEN | SPI_CR2_TXDMAEN;

  return HAL_OK;
}

uint16_t YUV2RGB422(unsigned char u)
{
	uint8_t r,g,b,temp;
	uint16_t rgb;
	
	if(1)  
	{
		r = colormap_ironblack[3*u];
		g = colormap_ironblack[3*u+1];
		b = colormap_ironblack[3*u + 2];
//		r = colormap_ironblack[3*u + 2];     // ¿í
//		g = colormap_ironblack[3*u+1];
//		b = colormap_ironblack[3*u];
	}
	else if(1)
	{
//		if(u > 160)
//		{
//			r = 0;
//			b = 0;
//			g = u;
//		}
//		else
//		{
//			r = u;
//			b = u;
//			g = u;
//		}
		r = 0;
		b = 0;
		g = u;
	}
	else 
	{
		r = g = b = u;
	}
		rgb=((r&0xF8)<<8)+((g&0xFC)<<3)+((b&0xF8)>>3);
//		temp = (rgb & 0xff00) >> 8;
//	  rgb = ((rgb & 0x00ff) << 8);
//		rgb = rgb + temp;
		return rgb;
}
