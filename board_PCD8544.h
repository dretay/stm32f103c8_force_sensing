/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

#include "gfx.h"
#include "stm32f1xx_hal.h"
#include "spi_helpers.h"

extern SPI_HandleTypeDef hspi1;

typedef struct {
	GPIO_TypeDef* port;
	int pin;
} Board_Pin;

//pin assignments
#define RST_Pin GPIO_PIN_2
#define RST_GPIO_Port GPIOA
#define DC_Pin GPIO_PIN_3
#define DC_GPIO_Port GPIOA
#define CS_Pin GPIO_PIN_4
#define CS_GPIO_Port GPIOA
#define LIGHT_Pin GPIO_PIN_0
#define LIGHT_GPIO_Port GPIOB

Board_Pin cs_pin = {
	CS_GPIO_Port,
	CS_Pin,
};
Board_Pin rst_pin = {
	RST_GPIO_Port,
	RST_Pin,
};
Board_Pin dc_pin = {
	DC_GPIO_Port,
	DC_Pin,
};
Board_Pin light_pin = {
	DC_GPIO_Port,
	DC_Pin,
};

Board_Pin pins[4];
const uint8_t CS_PIN = 0;
const uint8_t RST_PIN = 1;
const uint8_t DC_PIN = 2;
const uint8_t LIGHT_PIN = 2;

static GFXINLINE void init_board(GDisplay *g) {	
	pins[CS_PIN] = cs_pin;
	pins[RST_PIN] = rst_pin;
	pins[DC_PIN] = dc_pin;
	pins[LIGHT_PIN] = light_pin;
}

static GFXINLINE void post_init_board(GDisplay *g) {
	(void) g;
	

}

static GFXINLINE void setpin_reset(GDisplay *g, bool_t state) {
	(void) g;
	(void) state;
	HAL_GPIO_WritePin(pins[RST_PIN].port, pins[RST_PIN].pin, state ? GPIO_PIN_RESET : GPIO_PIN_SET);		
}

static GFXINLINE void set_backlight(GDisplay *g, uint8_t percent) {
	(void) g;
	(void) percent;
	if (percent == 0) {
		HAL_GPIO_WritePin(pins[LIGHT_PIN].port, pins[LIGHT_PIN].pin, GPIO_PIN_SET); 
	}
	else {
		HAL_GPIO_WritePin(pins[LIGHT_PIN].port, pins[LIGHT_PIN].pin, GPIO_PIN_RESET); 
	}
}

static GFXINLINE void acquire_bus(GDisplay *g) {
	(void) g;
	
	
}

static GFXINLINE void release_bus(GDisplay *g) {
	(void) g;
	
	
}

static GFXINLINE void write_cmd(GDisplay *g, uint8_t cmd) {
	(void) g;
	
	//set dc pin low
	HAL_GPIO_WritePin(pins[DC_PIN].port, pins[DC_PIN].pin, GPIO_PIN_RESET);		
	
	//set cs low		
	HAL_GPIO_WritePin(pins[CS_PIN].port, pins[CS_PIN].pin, GPIO_PIN_RESET); 
	
	
	//todo: figure out a better way to handle errors
#if RTE_SPI3_TX_DMA  == 1
	if(HAL_SPI_Transmit_DMA(&DisplaySpiHandle, &cmd, 1) != HAL_OK) {}

	//wait for the xfer to finish
	while(HAL_SPI_GetState(&DisplaySpiHandle) != HAL_SPI_STATE_READY) {}

#else		
	if (HAL_SPI_Transmit(&hspi1, &cmd, sizeof(uint8_t), 100) != HAL_OK) {}
#endif
		
	//set cs pin high
	HAL_GPIO_WritePin(pins[CS_PIN].port, pins[CS_PIN].pin, GPIO_PIN_SET);		
	
}

static GFXINLINE void write_data(GDisplay *g, uint8_t* data, uint16_t length) {	
	(void) g;	

	//set dc pin high
	HAL_GPIO_WritePin(pins[DC_PIN].port, pins[DC_PIN].pin, GPIO_PIN_SET);	
	
	//set cs low
	HAL_GPIO_WritePin(pins[CS_PIN].port, pins[CS_PIN].pin, GPIO_PIN_RESET); 
	

#if RTE_SPI3_TX_DMA  == 1
	if (HAL_SPI_Transmit_DMA(&DisplaySpiHandle, data, length) != HAL_OK) {}

	//wait for the xfer to finish
	while(HAL_SPI_GetState(&DisplaySpiHandle) != HAL_SPI_STATE_READY) {}

#else			
	if (HAL_SPI_Transmit(&hspi1, data, length, 100) != HAL_OK) {}	
#endif
	
	
	//set cs pin high	
	HAL_GPIO_WritePin(pins[CS_PIN].port, pins[CS_PIN].pin, GPIO_PIN_SET);				
	
}
//
//DMA interrupt handlers
//  note: these are hard-coded in rte_device for spi instance
//
#if RTE_SPI3_RX_DMA  == 1
void DMA1_Stream0_IRQHandler(void) {
	HAL_DMA_IRQHandler(DisplaySpiHandle.hdmarx);
}
#endif
#if RTE_SPI3_TX_DMA  == 1
void DMA1_Stream5_IRQHandler(void) {
	HAL_DMA_IRQHandler(DisplaySpiHandle.hdmatx);
}
#endif



/**
* TODO: what should i do with this? how can i free resources on the display?
  */
//void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
//{
//  
//#if RTE_SPI3_TX_DMA  == 1
//  static DMA_HandleTypeDef hdma_tx;
//#endif
//#if RTE_SPI3_RX_DMA  == 1
//  static DMA_HandleTypeDef hdma_rx;
//#endif
//  
//	//
//  // Reset spi bus
//	//
//  SPI_FORCE_RESET(3);
//  SPI_RELEASE_RESET(3);
//	
//	//
//	// Disable clocks and set pins to alternate function
//	//
//  HAL_GPIO_DeInit(RTE_SPI3_SCL_PORT, GPIO_PIN(RTE_SPI3_SCL_BIT));  
//  HAL_GPIO_DeInit(RTE_SPI3_MISO_PORT, GPIO_PIN(RTE_SPI3_MISO_BIT));  
//  HAL_GPIO_DeInit(RTE_SPI3_MOSI_PORT, GPIO_PIN(RTE_SPI3_MOSI_BIT));
//   
//	//
//	// Disable DMA streams and NVIC interrupts
//	//
//#if RTE_SPI3_TX_DMA  == 1
//  HAL_DMA_DeInit(&hdma_tx); 
//	HAL_NVIC_DisableIRQ(DMA_INSTANCE_IRQn(RTE_SPI2_TX_DMA_NUMBER,RTE_SPI2_TX_DMA_STREAM));
//#endif
//#if RTE_SPI3_RX_DMA   == 1
//  HAL_DMA_DeInit(&hdma_rx);
//	HAL_NVIC_DisableIRQ(DMA_INSTANCE_IRQn(RTE_SPI2_RX_DMA_NUMBER,RTE_SPI2_RX_DMA_STREAM));
//#endif  
//  
//}

#endif /* _GDISP_LLD_BOARD_H */