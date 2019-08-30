
/**
  ******************************************************************************
  * @file           : ring_buffer.h
  * @brief          : Header for ring_buffer.c file.
  *                   This file contains the common defines of the application.
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
#ifndef __RING_BUFFER__H___
#define __RING_BUFFER__H___

#ifdef __cplusplus
extern "C"
{
#endif

#define RING_BUFFER_MAX_LEN 64

struct
{
    uint8_t *pStart;    //store data point of start
    uint8_t *pEnd;      //store data point of end
    uint8_t *pHead;

    //uint8_t buff[RING_BUFFER_MAX_LEN];
    uint8_t *pbuf;
    uint32_t iSaveNum;
    uint32_t size;
}ring_buffer;

typedef struct ring_buffer RBuffer;

extern uint8_t Is_Buffer_Available(RBuffer *gRingBuffer);
extern void Ring_Buffer_Init(RBuffer *gRingBuffer);
extern uint8_t Ring_Buffer_Read(RBuffer *gRingBuffer);
extern void Ring_Buffer_Write(RBuffer *gRingBuffer, uint8_t *pdata, uint32_t count);

#ifdef __cplusplus
}
#endif

#endif