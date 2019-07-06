#include "main.h"
#include "MySystem.h"
#include "bsp_esp8266.h"
#include "esp8266_test.h"

extern __IO uint32_t rgb_color;

void ESP8266_Config(char *p_ApSsid, char *p_ApPwd, char *dst_ip, char *dst_port)
{
    printf("\r\n正在配置ESP8266_Config ......\r\n");
    macESP8266_CH_ENABLE();

    ESP8266_AT_Test();
    ESP8266_Net_Mode_Choose(STA);

    while (!ESP8266_JoinAP(p_ApSsid, p_ApPwd));

    ESP8266_Enable_MultipleId(DISABLE);

    while (!ESP8266_Link_Server(enumTCP, dst_ip, dst_port, Single_ID_0));

    while (!ESP8266_UnvarnishSend());

    printf("\r\n配置ESP8266 已完成\r\n");
}

void comRxHandle(void)
{

    char sStr[20] = {0};
    char cStr[50] = {0};

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
