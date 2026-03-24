#ifndef __BSP_BLUETOOTH_H
#define __BSP_BLUETOOTH_H

#include "usart.h"
#include "stm32f10x.h"

#define WIFI_INFO_MAX_LEN 64

// 全局变量声明
extern char wifi_ssid[WIFI_INFO_MAX_LEN];
extern char wifi_password[WIFI_INFO_MAX_LEN];

// 函数声明
void Parse_WiFi_Info(char* data);             // 解析WiFi配置信息
void Send_Bluetooth_Response(uint8_t status); // 发送蓝牙回复消息

#endif /* __BSP_BLUETOOTH_H */