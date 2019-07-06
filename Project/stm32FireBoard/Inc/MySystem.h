#ifndef __MY_SYSTEM__H___
#define __MY_SYSTEM__H___

#include "main.h"
#include "bsp_esp8266.h"
#include "esp8266_test.h"

#define RGB_LED_RED 0xFF0000
#define RGB_LED_GREEN 0x00FF00
#define RGB_LED_BLUE 0x0000FF

#define ESP8266_ApSsid "chen159"     //要连接的热点的名称
#define ESP8266_ApPwd "chun..134802" //要连接的热点的密钥

#define ESP8266_TcpServer_IP "192.168.28.126" //要连接的服务器的 IP
#define ESP8266_TcpServer_Port "8080"         //要连接的服务器的端口

extern void comRxHandle(void);
extern void setLedColor(uint8_t iled);
extern void ESP8266_Config(char *p_ApSsid, char *p_ApPwd, char *dst_ip, char *dst_port);

#endif
