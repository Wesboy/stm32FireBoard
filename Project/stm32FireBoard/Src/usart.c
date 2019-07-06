/**
  ******************************************************************************
  * File Name          : USART.c
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
#include <stdarg.h>
#include <string.h>
#include "bsp_esp8266.h"

uint8_t rbuf;
uint8_t rbuf1;
uint8_t readbuf[256];
uint8_t irx_Cnt = 0;
extern volatile uint8_t ucTcpClosedFlag;
USART_RECEIVETYPE UsartType;

/* USER CODE END 0 */

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart3_rx;
DMA_HandleTypeDef hdma_usart3_tx;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
}
/* USART3 init function */

void MX_USART3_UART_Init(void)
{

  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
}

void HAL_UART_MspInit(UART_HandleTypeDef *uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if (uartHandle->Instance == USART1)
  {
    /* USER CODE BEGIN USART1_MspInit 0 */

    /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
    /* USER CODE BEGIN USART1_MspInit 1 */
    /* USER CODE END USART1_MspInit 1 */
  }
  else if (uartHandle->Instance == USART3)
  {
    /* USER CODE BEGIN USART3_MspInit 0 */

    /* USER CODE END USART3_MspInit 0 */
    /* USART3 clock enable */
    __HAL_RCC_USART3_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**USART3 GPIO Configuration    
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* USART3 DMA Init */
    /* USART3_RX Init */
    hdma_usart3_rx.Instance = DMA1_Channel3;
    hdma_usart3_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart3_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart3_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart3_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart3_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart3_rx.Init.Mode = DMA_NORMAL;
    hdma_usart3_rx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart3_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle, hdmarx, hdma_usart3_rx);

    /* USART3_TX Init */
    hdma_usart3_tx.Instance = DMA1_Channel2;
    hdma_usart3_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart3_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart3_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart3_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart3_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart3_tx.Init.Mode = DMA_NORMAL;
    hdma_usart3_tx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart3_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle, hdmatx, hdma_usart3_tx);

    /* USART3 interrupt Init */
    HAL_NVIC_SetPriority(USART3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART3_IRQn);
    /* USER CODE BEGIN USART3_MspInit 1 */

    __HAL_UART_ENABLE_IT(uartHandle, UART_IT_IDLE);
    /* USER CODE END USART3_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef *uartHandle)
{

  if (uartHandle->Instance == USART1)
  {
    /* USER CODE BEGIN USART1_MspDeInit 0 */

    /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9 | GPIO_PIN_10);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
    /* USER CODE BEGIN USART1_MspDeInit 1 */

    /* USER CODE END USART1_MspDeInit 1 */
  }
  else if (uartHandle->Instance == USART3)
  {
    /* USER CODE BEGIN USART3_MspDeInit 0 */

    /* USER CODE END USART3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART3_CLK_DISABLE();

    /**USART3 GPIO Configuration    
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX 
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10 | GPIO_PIN_11);

    /* USART3 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);
    HAL_DMA_DeInit(uartHandle->hdmatx);

    /* USART3 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART3_IRQn);
    /* USER CODE BEGIN USART3_MspDeInit 1 */
    /* USER CODE END USART3_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

void itoa(int n, char *s)
{
  int i, sign;
  if ((sign = n) < 0)
    n = -n;          
  i = 0;
  do
  {
    s[i++] = n % 10 + '0'; 
  } while ((n /= 10) > 0);
  if (sign < 0)
    s[i++] = '-';
  s[i] = '\0';
}

void USART_printf(USART_TypeDef *USARTx, char *Data, ...)
{
  const char *s;
  int d;
  uint8_t buf[16];
  UART_HandleTypeDef *uartHandle;

  if (USARTx == USART1)
    uartHandle = &huart1;
  else if (USARTx == USART3)
    uartHandle = &huart3;
  else
    return;

  va_list ap;
  va_start(ap, Data);

  while (*Data != 0)
  {
    if (*Data == 0x5c) //'\'
    {
      switch (*++Data)
      {
      case 'r':
        buf[0] = 0x0d;
        HAL_UART_Transmit(uartHandle, &buf[0], 1, 0xFFFF);
        Data++;
        break;

      case 'n':
        buf[0] = 0x0a;
        HAL_UART_Transmit(uartHandle, &buf[0], 1, 0xFFFF);
        Data++;
        break;

      default:
        Data++;
        break;
      }
    }

    else if (*Data == '%')
    { //
      switch (*++Data)
      {
      case 's':
        s = va_arg(ap, const char *);

        for (; *s; s++)
        {
          HAL_UART_Transmit(uartHandle, (uint8_t *)s, 1, 0xFFFF);
        }

        Data++;

        break;

      case 'd':

        d = va_arg(ap, int);

        itoa(d, (char *)buf);

        for (s = (char *)buf; *s; s++)
        {
          HAL_UART_Transmit(uartHandle, (uint8_t *)s, 1, 0xFFFF);
        }

        Data++;

        break;

      default:
        Data++;

        break;
      }
    }
    else
      HAL_UART_Transmit(uartHandle, (uint8_t *)Data++, 1, 0xFFFF);
  }
}

void UsartReceive_IDLE(UART_HandleTypeDef *huart)
{
  uint32_t temp;

  if ((__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) != RESET))
  {
    __HAL_UART_CLEAR_IDLEFLAG(huart);
//    HAL_UART_DMAStop(huart);
//    temp = huart->hdmarx->Instance->CNDTR;
//    UsartType.RX_Size = RX_LEN - temp;
//    UsartType.RX_flag = 1;
//    HAL_UART_Receive_DMA(huart, UsartType.RX_pData, RX_LEN);
		  strEsp8266_Fram_Record.InfBit.FramFinishFlag = 1;
      ucTcpClosedFlag = strstr(strEsp8266_Fram_Record.Data_RX_BUF, "CLOSED\r\n") ? 1 : 0;
  }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  UNUSED(huart);
  if (huart == &huart1)
  {
    if (irx_Cnt > 255)
    {
      HAL_UART_Transmit(&huart1, readbuf, irx_Cnt, 100);
      irx_Cnt = 0;
      memset(readbuf, 0, sizeof(readbuf));
    }
    else
    {
      /* code */
      readbuf[irx_Cnt++] = rbuf1;
      if ((readbuf[irx_Cnt - 1] == 0x0a) && (readbuf[irx_Cnt - 2] == 0x0d))
      {

        HAL_UART_Transmit(&huart1, readbuf, irx_Cnt, 100);
        irx_Cnt = 0;
        memset(readbuf, 0, sizeof(readbuf));
      }
    }
    HAL_UART_Receive_IT(&huart1, &rbuf1, 1);
  }
  else if (huart == &huart3)
  {
    if (strEsp8266_Fram_Record.InfBit.FramLength < (RX_BUF_MAX_LEN - 1)) 
      strEsp8266_Fram_Record.Data_RX_BUF[strEsp8266_Fram_Record.InfBit.FramLength++] = rbuf;
    if (HAL_UART_Receive_IT(&huart3, &rbuf, 1) != HAL_OK)
    {
      Error_Handler();
    }
  }
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
