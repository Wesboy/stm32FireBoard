#include "main.h"
#include "MySystem.h"
#include "bsp_esp8266.h"
#include "esp8266_test.h"
#include <string.h>

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;

extern uint16_t irUart1_low;
extern uint16_t irUart1_high;
extern uint8_t ComRxBuff[256];

uint32_t timer;

extern __IO uint32_t rgb_color;

void ESP8266_Config(char *p_ApSsid, char *p_ApPwd, char *dst_ip, char *dst_port)
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

static void ProtocolDataHandle(uint8_t *buf, uint8_t iLenIndex)
{
    uint8_t i = 0;

    if (buf[i] == 0xB1) //LED Red
    {
        if (buf[i + 1] == 0x01)
        {
            setLedColor(1, 255, 0, 0);
        }
        else
        {
            setLedColor(1, 0, 0, 0);
        }
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

void comRxHandle(void)
{
    ProtocolDecoder(ComRxBuff, &irUart1_low, irUart1_high);
}

void WifiESP8266_RxHandle(void)
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
