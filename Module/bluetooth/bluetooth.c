#include "bluetooth.h"
#include <stdio.h>
#include <string.h>

// 全局变量定义
char wifi_ssid[WIFI_INFO_MAX_LEN] = {0};
char wifi_password[WIFI_INFO_MAX_LEN] = {0};

// 解析WiFi配置信息（格式：!SSID=PASSWORD!）
void Parse_WiFi_Info(char* data) {
    USART_SendString(USART1, "Parse_WiFi_Info\n");
    memset(wifi_ssid, 0, WIFI_INFO_MAX_LEN);
    memset(wifi_password, 0, WIFI_INFO_MAX_LEN);
    if (data[0] != '!' || data[strlen(data) - 1] != '!') {
        USART_SendString(USART1, "Data format error: The beginning and end are not correct'!'\r\n");
        return;
    }

    char* equal_sign = strchr(data, '=');
    if (equal_sign == NULL) {
        USART_SendString(USART1, "Data format error: No equal sign\r\n");
        return;
    }

    // 提取SSID
    int ssid_len = equal_sign - data - 1;
    if (ssid_len > 0 && ssid_len < WIFI_INFO_MAX_LEN) {
        strncpy(wifi_ssid, data + 1, ssid_len);
    }

    // 提取密码
    char* end_mark = strrchr(data, '!');
    int pass_len = end_mark - equal_sign - 1;
    if (pass_len > 0 && pass_len < WIFI_INFO_MAX_LEN) {
        strncpy(wifi_password, equal_sign + 1, pass_len);
    }
    USART_SendString(USART1, wifi_ssid);
    USART_SendString(USART1, wifi_password);
}

// 发送蓝牙回复消息（status:0成功，1失败）
void Send_Bluetooth_Response(uint8_t status) {
    char response[128] = {0};
    sprintf(response, "{\"status\":%d, \"wifi_name\": \"%s\"}\r\n", status, wifi_ssid);
    USART_SendString(USART3, response);
    USART_SendString(USART1, "Bluetooth reply sent: ");
    USART_SendString(USART1, response);
}