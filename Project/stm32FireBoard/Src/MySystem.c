#include "main.h"
#include "MySystem.h"
#include "bsp_esp8266.h"
#include "esp8266_test.h"
#include <string.h>

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;

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
}

void comRxHandle(void)
{

    char sStr[20] = {0};
    char cStr[50] = {0};
    if (ReadUserTimer(&timer) > 2000)
    {
        ResetUserTimer(&timer);

        sprintf(sStr, "Led Status:");
        if (rgb_color & 0xFFFFFF)
        {
            if ((rgb_color & RGB_LED_RED) == RGB_LED_RED)
                sprintf(cStr, "RED ON\r\n");
            if ((rgb_color & RGB_LED_GREEN) == RGB_LED_GREEN)
                sprintf(cStr, "GREEN ON\r\n");
            if ((rgb_color & RGB_LED_BLUE) == RGB_LED_BLUE)
                sprintf(cStr, "BLUE ON\r\n");
        }
        else
        {
            /* code */
            sprintf(cStr, "OFF\r\n");
        }

        ESP8266_SendString(ENABLE, sStr, 0, Single_ID_0);
        ESP8266_SendString(ENABLE, cStr, 0, Single_ID_0);
    }

}

void WifiESP8266_RxHandle(void)
{
    uint8_t bStatus;

    if (strEsp8266_Fram_Record.InfBit.FramFinishFlag)  //处理esp8266的串口数据
    {
        strEsp8266_Fram_Record.InfBit.FramFinishFlag = 0;
        
        //数据发送给到串口COM1
        HAL_UART_Transmit(&huart1, (uint8_t *)strEsp8266_Fram_Record.Data_RX_BUF, strEsp8266_Fram_Record.InfBit.FramLength, 100);

        if (strEsp8266_Fram_Record.Data_RX_BUF[0] == 0xaa && strEsp8266_Fram_Record.Data_RX_BUF[1] == 0x55)
        {
            rgb_color = 0x0;
            if (strEsp8266_Fram_Record.Data_RX_BUF[2] & 0x01)
                rgb_color |= RGB_LED_RED;
            if (strEsp8266_Fram_Record.Data_RX_BUF[2] & 0x02)
                rgb_color |= RGB_LED_GREEN;
            if (strEsp8266_Fram_Record.Data_RX_BUF[2] & 0x04)
                rgb_color |= RGB_LED_BLUE;
        }
        strEsp8266_Fram_Record.InfBit.FramLength = 0;
        memset(strEsp8266_Fram_Record.Data_RX_BUF, 0, sizeof(strEsp8266_Fram_Record.Data_RX_BUF));
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

void setLedColor(uint8_t iled)
{
    uint32_t rgb_t = 0x0;

    if (iled & 0x7)
    {
        if (iled & 0x01)
        {
            rgb_t |= RGB_LED_RED;
        }
        if (iled & 0x02)
        {
            rgb_t |= RGB_LED_GREEN;
        }
        if (iled & 0x04)
        {
            rgb_t |= RGB_LED_BLUE;
        }
    }

    rgb_color = rgb_t;
}
