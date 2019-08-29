#ifndef __MY_SYSTEM__H___
#define __MY_SYSTEM__H___

#include "main.h"
#include "bsp_esp8266.h"
#include "esp8266_test.h"
#include "tim.h"

#define PROTOCOL_LEN_MAX 12
#define COM_REBUFF_LEN_MAX 256


#define ESP8266_ApSsid "chen159"     //要连接的热点的名称
#define ESP8266_ApPwd "chun..134802" //要连接的热点的密钥

//#define ESP8266_TcpServer_IP "192.168.28.126" //要连接的服务器的 IP
//#define ESP8266_TcpServer_Port "8080"         //要连接的服务器的端口
#define      ESP8266_TcpServer_IP                 "192.168.28.203"      //要连接的服务器的 IP
#define      ESP8266_TcpServer_Port               "8080"               //要连接的服务器的端口

// #define ESP8266_ApSsid "TP-LINK_172932"     //要连接的热点的名称
// #define ESP8266_ApPwd "fly654312" //要连接的热点的密钥

// #define ESP8266_TcpServer_IP "192.128.1.100" //要连接的服务器的 IP
// #define ESP8266_TcpServer_Port "8080"         //要连接的服务器的端口

extern void SystemTaskInit(void);

#endif
