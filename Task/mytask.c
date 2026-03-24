#include "mytask.h"
#include <string.h>
#include "bluetooth.h"
#include "esp01s.h"
#include "usart.h"

//接收与解析
void Task_Bluetooth(void) {
    USART_SendString(USART1, "Task_Bluetooth\r\n");
    while (1) {
        if ((USART3_RxFlag == 1)) {
            // 处理蓝牙数据
            USART3_RxBuffer[USART3_RxLen] = '\0';
            USART_SendString(USART1, "Received Bluetooth data: ");
            USART_SendString(USART1, (char*)USART3_RxBuffer);
            USART_SendString(USART1, "\r\n");

            // 解析WiFi信息
            Parse_WiFi_Info((char*)USART3_RxBuffer);

            // 打印解析结果
            USART_SendString(USART1, "sparse result: \r\n");
            USART_SendString(USART1, "SSID = ");
            USART_SendString(USART1, wifi_ssid);
            USART_SendString(USART1, "\r\n");
            USART_SendString(USART1, "PASS = ");
            USART_SendString(USART1, wifi_password);

            USART3_RxFlag = 0;
            USART3_RxLen = 0;
            memset(USART3_RxBuffer, 0, USART3_BUF_SIZE);
        }
    }
}

//连接 WiFi / AT 指令
void Task_ESP(void) {
    while (1) {
        if (1 == ESP_WifiFlag) {
            // 连接WiFi
            USART_SendString(USART1, "ESP01S starts connecting WiFi...\r\n");
        }
        uint8_t connect_status = ESP01S_Connect_WiFi(wifi_ssid, wifi_password);

        // 蓝牙回复结果
        Send_Bluetooth_Response(connect_status);
        ESP_WifiFlag = 0;
    }
}

//状态回复 + 可能的日志
void Task_Report(void);