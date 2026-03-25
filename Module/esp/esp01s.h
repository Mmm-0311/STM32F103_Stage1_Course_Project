#ifndef __BSP_ESP01S_H
#define __BSP_ESP01S_H

#include "Delay.h"
#include "stm32f10x.h"
#include "usart.h"

#define ESP_TCP_SERVER_IP    "121.41.231.209"
#define ESP_TCP_SERVER_PORT  9003

extern uint8_t ESP_WifiFlag;

// 函数声明
uint8_t ESP01S_Send_AT_Cmd(char* cmd, char* expected_resp, uint32_t timeout);
uint8_t ESP01S_Connect_WiFi(char* ssid, char* password);
uint8_t ESP01S_Connect_TCP_Server(char* ip, uint16_t port);
uint8_t ESP01S_Send_TCP_Data(char* data);
uint8_t ESP01S_Get_Current_SSID(char* ssid_out);
#endif /* __BSP_ESP01S_H */