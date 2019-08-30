#include "ring_buffer.h"

/**
  * @brief  Is has buffer available
  * @param  gRingBuffer is outer poiter
  * @retval 1: available 0: disavailate 
  */
uint8_t Is_Buffer_Available(RBuffer *gRingBuffer)
{
  uint8_t iRet;
  if (gRingBuffer->pEnd != gRingBuffer->pStart)
  {
    iRet = 1;
  }
  else
  {
    iRet = 0;
    gRingBuffer->iSaveNum = 0;
  }

  return iRet;
}

/**
  * @brief  Ring Buffer init
  * @param  gRingBuffer is outer poiter
  * @retval None
  */
void Ring_Buffer_Init(RBuffer *gRingBuffer)
{
  memset(gRingBuffer->buff, 0, sizeof(uint8_t) * (gRingBuffer->size));
  gRingBuffer->pHead = gRingBuffer->pbuf;
  gRingBuffer->pStart = gRingBuffer->pbuf;
  gRingBuffer->pEnd = gRingBuffer->pbuf;
}

/**
  * @brief  Ring_Buffer_Read
  * @param  gRingBuffer is outer poiter
  * @retval buf
  */
uint8_t Ring_Buffer_Read(RBuffer *gRingBuffer)
{
  uint8_t iRet;

  iRet = *gRingBuffer->pStart;

  if ((gRingBuffer->pEnd - gRingBuffer->pStart) % (gRingBuffer->size - 1))
  {
    gRingBuffer->pStart++;
  }
  else
  {
    //超出buf长度，覆盖最前的数据
    gRingBuffer->pStart = gRingBuffer->pHead;
  }

  if(gRingBuffer->iSaveNum != 0)
    gRingBuffer->iSaveNum--;
    
  return iRet;
}

/**
  * @brief  Ring_Buffer_Write
  * @param  gRingBuffer is outer poiter,and data is need to save
  * @retval None
  */
void Ring_Buffer_Write(RBuffer *gRingBuffer, uint8_t *pdata, uint32_t count)
{
  if (count <= 0 && count > RING_BUFFER_MAX_LEN)
  {
    return;
  }

  uint32_t i;

  for (i = 0; i < count; i++)
  {
    if ((gRingBuffer->pEnd - gRingBuffer->pStart) % (gRingBuffer->size - 1))
    {
      gRingBuffer->pEnd++;
    }
    else
    {
      //超出buf长度，覆盖最前的数据
      gRingBuffer->pEnd = gRingBuffer->pbuf;
    }
    *gRingBuffer->pEnd = pdata[i];
    gRingBuffer->iSaveNum++;
  }
}