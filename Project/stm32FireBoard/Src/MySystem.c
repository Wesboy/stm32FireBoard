#include "main.h"
#include "MySystem.h"
#include "bsp_esp8266.h"
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

static void setLedOn(uint8_t iLed, uint8_t bOn)
{
    if (bOn == 0x01)
    {
        setLedColor(iLed, 255, 255, 255);
    }
    else
    {
        setLedColor(iLed, 0, 0, 0);
    }
}

static void ProtocolDataHandle(uint8_t *buf, uint8_t iLenIndex)
{
    uint8_t i = 0;

    switch(buf[i]&0xF0)
    {
        case 0xB0:
            setLedOn(buf[i]&0xF, buf[i+1]);
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
                    printf("Protocol length over error!\r\n");
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
            while (!ESP8266_JoinAP(macUser_ESP8266_ApSsid, macUser_ESP8266_ApPwd))
                ;
            while (!ESP8266_Link_Server(enumTCP, macUser_ESP8266_TcpServer_IP, macUser_ESP8266_TcpServer_Port, Single_ID_0))
                ;
            printf("\r\n重连热点和服务器成功\r\n");
        }

        while (!ESP8266_UnvarnishSend())
            ;
    }
}






static void vRxDataTask( void * pvParameters )
{
	for(;;)
	{
        comRxHandle();
        WifiESP8266_RxHandle();
	}
	
	//delete task
}

static void vLedFlash( void * pvParameters )
{
	for(;;)
	{
        setLedColor(3, 255, 255, 255);
        vTaskDelay(1000);
        setLedColor(3, 0, 0, 0);
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
    xTaskCreate(vLedFlash, "LedFlash", 256, NULL, 3, NULL);
    xTaskCreate(ConnectWifiESP8266, "ConnectWifi", 256, NULL, 4, &AppTaskConnectWifi_Handle);
    //delete task

    vTaskDelete(AppTaskCreate_Handle);

    taskEXIT_CRITICAL();
}

void SystemTaskInit(void)
{
    
    BaseType_t xReturn = pdPASS;
	        																									
	if(HAL_UART_Receive_IT(&huart1, &rbuf1, 1)!= HAL_OK)
	{
		Error_Handler();
	}

    xReturn = xTaskCreate(vTaskStart, "TaskStart", 256, NULL, 1, &AppTaskCreate_Handle);
    if(xReturn == pdPASS)
        vTaskStartScheduler();
}
