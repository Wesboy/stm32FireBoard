/**
  ******************************************************************************
  * @file    bsp_i2c_ee.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   i2c EEPROM(AT24C02)Ӧ�ú���bsp
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-ָ���� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

#include "bsp_i2c_eeprom.h"		


static uint16_t EEPROM_ADDRESS;

static __IO uint32_t  I2CTimeout = I2CT_LONG_TIMEOUT;   


static uint32_t I2C_TIMEOUT_UserCallback(uint8_t errorCode);

extern I2C_HandleTypeDef hi2c1;

/**
  * @brief  I2C I/O����
  * @param  ��
  * @retval ��
  */
static void I2C_GPIO_Config(void)
{
//  GPIO_InitTypeDef GPIO_InitStructure;

//  /* ʹ���� I2C �йص�ʱ�� */
//	EEPROM_I2C_APBxClock_FUN ( EEPROM_I2C_CLK, ENABLE );
//	EEPROM_I2C_GPIO_APBxClock_FUN ( EEPROM_I2C_GPIO_CLK, ENABLE );
//	
//    
//  /* I2C_SCL��I2C_SDA*/
//  GPIO_InitStructure.GPIO_Pin = EEPROM_I2C_SCL_PIN;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;	       // ��©���
//  GPIO_Init(EEPROM_I2C_SCL_PORT, &GPIO_InitStructure);
//	
//  GPIO_InitStructure.GPIO_Pin = EEPROM_I2C_SDA_PIN;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;	       // ��©���
//  GPIO_Init(EEPROM_I2C_SDA_PORT, &GPIO_InitStructure);	
	
	
}


/**
  * @brief  I2C ����ģʽ����
  * @param  ��
  * @retval ��
  */
static void I2C_Mode_Config(void)
{
//  I2C_InitTypeDef I2C_InitStructure; 

//  /* I2C ���� */
//  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
//	
//	/* �ߵ�ƽ�����ȶ����͵�ƽ���ݱ仯 SCL ʱ���ߵ�ռ�ձ� */
//  I2C_InitStructure.DutyCycle = I2C_DutyCycle_2;

//  I2C_InitStructure.OwnAddress1 = I2Cx_OWN_ADDRESS7;
//  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable ;
//	 
//	/* I2C��Ѱַģʽ */
//  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
//	
//	/* ͨ������ */
//  I2C_InitStructure.ClockSpeed = I2C_Speed;

//  /* I2C ��ʼ�� */
//  I2C_Init(EEPROM_I2Cx, &I2C_InitStructure);
//  
//	/* ʹ�� I2C */
//  I2C_Cmd(EEPROM_I2Cx, ENABLE);   
}


/**
  * @brief  I2C ����(EEPROM)��ʼ��
  * @param  ��
  * @retval ��
  */
void I2C_EE_Init(void)
{

//  I2C_GPIO_Config(); 

//  I2C_Mode_Config();

/* ����ͷ�ļ�i2c_ee.h�еĶ�����ѡ��EEPROM���豸��ַ */
#ifdef EEPROM_Block0_ADDRESS
  /* ѡ�� EEPROM Block0 ��д�� */
  EEPROM_ADDRESS = EEPROM_Block0_ADDRESS;
#endif

#ifdef EEPROM_Block1_ADDRESS  
	/* ѡ�� EEPROM Block1 ��д�� */
  EEPROM_ADDRESS = EEPROM_Block1_ADDRESS;
#endif

#ifdef EEPROM_Block2_ADDRESS  
	/* ѡ�� EEPROM Block2 ��д�� */
  EEPROM_ADDRESS = EEPROM_Block2_ADDRESS;
#endif

#ifdef EEPROM_Block3_ADDRESS  
	/* ѡ�� EEPROM Block3 ��д�� */
  EEPROM_ADDRESS = EEPROM_Block3_ADDRESS;
#endif
}


/**
  * @brief   ���������е�����д��I2C EEPROM��
  * @param   
  *		@arg pBuffer:������ָ��
  *		@arg WriteAddr:д��ַ
  *     @arg NumByteToWrite:д���ֽ���
  * @retval  ��
  */
void I2C_EE_BufferWrite(uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite)
{
  uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0;

  Addr = WriteAddr % I2C_PageSize;
  count = I2C_PageSize - Addr;
  NumOfPage =  NumByteToWrite / I2C_PageSize;
  NumOfSingle = NumByteToWrite % I2C_PageSize;
 
  /* If WriteAddr is I2C_PageSize aligned  */
  if(Addr == 0) 
  {
    /* If NumByteToWrite < I2C_PageSize */
    if(NumOfPage == 0) 
    {
      I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      I2C_EE_WaitEepromStandbyState();
    }
    /* If NumByteToWrite > I2C_PageSize */
    else  
    {
      while(NumOfPage--)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, I2C_PageSize); 
    	  I2C_EE_WaitEepromStandbyState();
        WriteAddr +=  I2C_PageSize;
        pBuffer += I2C_PageSize;
      }

      if(NumOfSingle!=0)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
        I2C_EE_WaitEepromStandbyState();
      }
    }
  }
  /* If WriteAddr is not I2C_PageSize aligned  */
  else 
  {
    /* If NumByteToWrite < I2C_PageSize */
    if(NumOfPage== 0) 
    {
      I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      I2C_EE_WaitEepromStandbyState();
    }
    /* If NumByteToWrite > I2C_PageSize */
    else
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / I2C_PageSize;
      NumOfSingle = NumByteToWrite % I2C_PageSize;	
      
      if(count != 0)
      {  
        I2C_EE_PageWrite(pBuffer, WriteAddr, count);
        I2C_EE_WaitEepromStandbyState();
        WriteAddr += count;
        pBuffer += count;
      } 
      
      while(NumOfPage--)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, I2C_PageSize);
        I2C_EE_WaitEepromStandbyState();
        WriteAddr +=  I2C_PageSize;
        pBuffer += I2C_PageSize;  
      }
      if(NumOfSingle != 0)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle); 
        I2C_EE_WaitEepromStandbyState();
      }
    }
  }  
}


/**
  * @brief   дһ���ֽڵ�I2C EEPROM��
  * @param   
  *		@arg pBuffer:������ָ��
  *		@arg WriteAddr:д��ַ 
  * @retval  ��
  */
uint32_t I2C_EE_ByteWrite(uint8_t* pBuffer, uint8_t WriteAddr) 
{
  HAL_StatusTypeDef bStatus;

  bStatus = HAL_I2C_Mem_Write(&hi2c1, EEPROM_ADDRESS, WriteAddr, 1, pBuffer, 1, I2CT_LONG_TIMEOUT);

  printf("I2C_EE_ByteWrite Status:%d\r\n", bStatus);
  return 1;
}


/**
  * @brief   ��EEPROM��һ��дѭ���п���д����ֽڣ���һ��д����ֽ���
  *          ���ܳ���EEPROMҳ�Ĵ�С��AT24C02ÿҳ��8���ֽ�
  * @param   
  *		@arg pBuffer:������ָ��
  *		@arg WriteAddr:д��ַ
  *     @arg NumByteToWrite:д���ֽ���
  * @retval  ��
  */
uint32_t I2C_EE_PageWrite(uint8_t* pBuffer, uint8_t WriteAddr, uint8_t NumByteToWrite)
{
  HAL_StatusTypeDef bStatus;

  bStatus = HAL_I2C_Mem_Write(&hi2c1, EEPROM_ADDRESS, WriteAddr, NumByteToWrite, pBuffer, NumByteToWrite, I2CT_LONG_TIMEOUT);

  printf("I2C_EE_PageWrite Status:%d\r\n", bStatus);

  return 1;
}


/**
  * @brief   ��EEPROM�����ȡһ������ 
  * @param   
  *		@arg pBuffer:��Ŵ�EEPROM��ȡ�����ݵĻ�����ָ��
  *		@arg WriteAddr:�������ݵ�EEPROM�ĵ�ַ
  *     @arg NumByteToWrite:Ҫ��EEPROM��ȡ���ֽ���
  * @retval  ��
  */
uint32_t I2C_EE_BufferRead(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead)
{
  HAL_StatusTypeDef bStatus;

  bStatus = HAL_I2C_Mem_Read(&hi2c1, EEPROM_ADDRESS, ReadAddr, NumByteToRead, pBuffer, NumByteToRead, I2CT_LONG_TIMEOUT);

  printf("I2C_EE_BufferRead Status:%d\r\n", bStatus);

  return 1;
}


/**
  * @brief  Wait for EEPROM Standby state 
  * @param  ��
  * @retval ��
  */
void I2C_EE_WaitEepromStandbyState(void)      
{
  
}




/**
  * @brief  Basic management of the timeout situation.
  * @param  errorCode��������룬����������λ���ĸ����ڳ���.
  * @retval ����0����ʾIIC��ȡʧ��.
  */
static  uint32_t I2C_TIMEOUT_UserCallback(uint8_t errorCode)
{
  /* Block communication and all processes */
  EEPROM_ERROR("I2C �ȴ���ʱ!errorCode = %d",errorCode);
  
  return 0;
}
/*********************************************END OF FILE**********************/

