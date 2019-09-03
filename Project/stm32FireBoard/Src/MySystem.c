#include "main.h"
#include "MySystem.h"
#include "bsp_esp8266.h" 
#include "bsp_i2c_eeprom.h"
#include "esp8266_test.h"
#include <string.h>


static TaskHandle_t AppTaskCreate_Handle = NULL;
static TaskHandle_t AppTaskConnectWifi_Handle = NULL;

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;

extern uint8_t rbuf;
extern uint8_t rbuf1;

extern uint16_t irUart1_low;
extern uint16_t irUart1_high;
extern uint8_t ComRxBuff[256];

#define EEP_Firstpage 0x00
#define EEPROM_TEST_READBUFF_MAX 64
#define EEPROM_TEST_WRITEBUFF_MAX 64

extern __IO uint32_t rgb_color;

static void ESP8266_Config(char *p_ApSsid, char *p_ApPwd, char *dst_ip, char *dst_port)
{
    printf("\r\n正在配置ESP8266_Config ......\r\n");
    macESP8266_CH_ENABLE();

    ESP8266_AT_Test();
    ESP8266_Net_Mode_Choose(STA);

    while (!ESP8266_JoinAP(p_ApSsid, p_ApPwd))
        ;

    ESP8266_Enable_MultipleId(DISABLE);

    while (!ESP8266_Link_Server(enumTCP, dst_ip, dst_port, Single_ID_0))
        ;

    while (!ESP8266_UnvarnishSend())
        ;

    printf("\r\n配置ESP8266 已完成\r\n");
		
    memset(strEsp8266_Fram_Record.Data_RX_BUF, 0, sizeof(strEsp8266_Fram_Record.Data_RX_BUF));
    strEsp8266_Fram_Record.iFramehigh = 0;
    strEsp8266_Fram_Record.iFramelow = 0;
}

static void setLedCtl(uint8_t iCmd, uint8_t bOn)
{
    uint8_t val = 0;
    if(bOn)
        val = 255;

    switch(iCmd)
    {
        case 0x01:
            SetRedColorValue(val);
            break;
        case 0x02:
            SetGreenColorValue(val);
            break;
        case 0x03:
            SetBlueColorValue(val);
            break;
        default:
            break;
    }
}

static void ProtocolDataHandle(uint8_t *buf, uint8_t iLenIndex)
{
    uint8_t i = 0;

    switch(buf[i]&0xF0)
    {
        case 0xB0:   //Led Control
            setLedCtl(buf[i]&0xF, buf[i+1]);
            break;
        default:
            break;
    }

}

static void ProtocolDecoder(uint8_t *buf, uint16_t *iLowIndex, uint16_t iHighIndex)
{

    static uint8_t iFrameStatus;
    static uint8_t iFrameLen;
    static uint8_t iFrameLenIndex;
    static uint8_t check;
    static uint8_t rbuf[PROTOCOL_LEN_MAX];
	

    if (*iLowIndex != iHighIndex)
    {
		//printf("[%d]buf[%d]:%x\r\n", iHighIndex,*iLowIndex, buf[*iLowIndex]);
        switch (iFrameStatus)
        {
        case 0:
            if (buf[*iLowIndex] == 0x55)
            {
                iFrameStatus = 1;
            }
            break;
        case 1:
            if (buf[*iLowIndex] == 0xaa)
            {
                iFrameStatus = 2;
            }
            else
            {
                iFrameStatus = 0;
            }

            break;
        case 2:
            if (buf[*iLowIndex] == 0)
            {
                iFrameStatus = 0;
            }
            else
            {
                iFrameStatus = 3;
                check = buf[*iLowIndex];
                iFrameLen = check;
                if (iFrameLen > PROTOCOL_LEN_MAX)

                {
                    iFrameStatus = 0;
                    check = 0;
                    printf("Protocol length=%d over error!\r\n", iFrameLen);
                }
                iFrameLenIndex = 0;
            }
            break;
        case 3:
            if (iFrameLenIndex < iFrameLen - 1)
            {
                rbuf[iFrameLenIndex] = buf[*iLowIndex];
                check += buf[*iLowIndex];
                iFrameLenIndex++;
            }
            else
            {
                if (buf[*iLowIndex] == check)
                {
                    printf("data ok %x %x\r\n", rbuf[0], rbuf[1]);
                    ProtocolDataHandle(rbuf, iFrameLenIndex);
                }
                else
                {
                    printf("data CRC ERROR!\r\n");
                }
                check = 0;
                iFrameStatus = 0;
            }
            break;
        default:
            check = 0;
            iFrameStatus = 0;
            iFrameLen = 0;
            break;
        }
        if (*iLowIndex > COM_REBUFF_LEN_MAX)
            (*iLowIndex) = 0;
        else
            (*iLowIndex)++;
    }
} 

static void comRxHandle(void)
{
    ProtocolDecoder(ComRxBuff, &irUart1_low, irUart1_high);
}

static void WifiESP8266_RxHandle(void)
{
    uint8_t bStatus;

    if (strEsp8266_Fram_Record.FrameFinishFlag) //处理esp8266的串口数据
    {
        strEsp8266_Fram_Record.FrameFinishFlag = 0;
        while (strEsp8266_Fram_Record.iFramelow != strEsp8266_Fram_Record.iFramehigh)
        {
            ProtocolDecoder(strEsp8266_Fram_Record.Data_RX_BUF, &strEsp8266_Fram_Record.iFramelow, strEsp8266_Fram_Record.iFramehigh);
        }
    }

    if (ucTcpClosedFlag) //检测是否失去连接
    {
        ESP8266_ExitUnvarnishSend(); //退出透传模式

        do
            bStatus = ESP8266_Get_LinkStatus(); //获取连接状态
        while (!bStatus);

        if (bStatus == 4) //确认失去连接后重连
        {
            printf("\r\n正在重连热点和服务器 ......\r\n");
            while (!ESP8266_JoinAP(ESP8266_ApSsid, ESP8266_ApPwd))
                ;
            while (!ESP8266_Link_Server(enumTCP, ESP8266_TcpServer_IP, ESP8266_TcpServer_Port, Single_ID_0))
                ;
            printf("\r\n重连热点和服务器成功\r\n");
        }

        while (!ESP8266_UnvarnishSend())
            ;
    }
}

/**
  * @brief  I2C(AT24C02)读写测试
  * @param  无
  * @retval 正常返回1，异常返回0
  */
void EEPROM_Test(void *pvParameters)
{
    uint16_t i;
    uint8_t I2c_Buf_Write[EEPROM_TEST_READBUFF_MAX];
    uint8_t I2c_Buf_Read[EEPROM_TEST_WRITEBUFF_MAX];

    for(;;)
	{
        printf("写入的数据\r\n");

        for (i = 0; i < EEPROM_TEST_WRITEBUFF_MAX; i++) //填充缓冲
        {
            I2c_Buf_Write[i] = i;

            printf("0x%02X ", I2c_Buf_Write[i]);
            if (i % 16 == 15)
                printf("\r\n");
        }

        //将I2c_Buf_Write中顺序递增的数据写入EERPOM中
        I2C_EE_BufferWrite(I2c_Buf_Write, EEP_Firstpage, EEPROM_TEST_WRITEBUFF_MAX);

        EEPROM_INFO("写成功\r\n");

        EEPROM_INFO("读出的数据\r\n");
        //将EEPROM读出数据顺序保持到I2c_Buf_Read中
        I2C_EE_BufferRead(I2c_Buf_Read, EEP_Firstpage, EEPROM_TEST_WRITEBUFF_MAX);

        //将I2c_Buf_Read中的数据通过串口打印
        for (i = 0; i < EEPROM_TEST_WRITEBUFF_MAX; i++)
        {
            if (I2c_Buf_Read[i] != I2c_Buf_Write[i])
            {
//                EEPROM_ERROR("0x%02X ", I2c_Buf_Read[i]);
//                EEPROM_ERROR("错误:I2C EEPROM写入与读出的数据不一致\r\n");
//                break;
            }
            printf("0x%02X ", I2c_Buf_Read[i]);
            if (i % 16 == 15)
                printf("\r\n");
        }

        if (EEPROM_TEST_WRITEBUFF_MAX == i)
            EEPROM_INFO("I2C(AT24C02)读写测试成功\r\n");

		vTaskDelay(2000);
	}
}
/*********************************************END OF FILE**********************/

static void vRxDataTask( void * pvParameters )
{
	for(;;)
	{
        comRxHandle();
        WifiESP8266_RxHandle();
        vTaskDelay(10);
    }
	
	//delete task
}

static void vLedFlash( void * pvParameters )
{
	for(;;)
	{
        SetColorValue(255, 255, 255);
        vTaskDelay(1000);
        SetColorValue(0, 0, 0);
        vTaskDelay(1000);
	}
	
	//delete task
}

static void ConnectWifiESP8266( void * pvParameters )
{

    ESP8266_Init(); 
    if(HAL_UART_Receive_IT(&huart3, &rbuf, 1)!= HAL_OK)
    {
        Error_Handler();
    }
    printf ( "\r\nWF-ESP8266 WiFi Mode Test!!!\r\n" );

    ESP8266_Config(ESP8266_ApSsid, ESP8266_ApPwd, ESP8266_TcpServer_IP, ESP8266_TcpServer_Port);

    vTaskDelete(AppTaskConnectWifi_Handle);
	//delete task
}

static void vTaskStart( void * pvParameters )
{
    taskENTER_CRITICAL();           

    xTaskCreate(vRxDataTask, "RxDataTask", 256, NULL, 2, NULL);
//    xTaskCreate(vLedFlash, "LedFlash", 256, NULL, 3, NULL);
    xTaskCreate(ConnectWifiESP8266, "ConnectWifi", 256, NULL, 3, &AppTaskConnectWifi_Handle);
//    xTaskCreate(EEPROM_Test, "EEPROM_Test", 256, NULL, 2, NULL);
    //delete task

    vTaskDelete(AppTaskCreate_Handle);

    taskEXIT_CRITICAL();
}

void SystemTaskInit(void)
{
    
    I2C_EE_Init();
    BaseType_t xReturn = pdPASS;
	        																									
	if(HAL_UART_Receive_IT(&huart1, &rbuf1, 1)!= HAL_OK)
	{
		Error_Handler();
	}

    xReturn = xTaskCreate(vTaskStart, "TaskStart", 256, NULL, 1, &AppTaskCreate_Handle);
    if(xReturn == pdPASS)
        vTaskStartScheduler();
}
