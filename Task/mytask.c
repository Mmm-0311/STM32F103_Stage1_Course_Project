#include "mytask.h"
#include <string.h>
#include "FreeRTOS.h"
#include "bluetooth.h"
#include "esp01s.h"
#include "task.h"
#include "usart.h"

//接收与解析
void Task_Bluetooth(void* pvParameters) {
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

            ESP_WifiFlag = 1;
            USART3_RxFlag = 0;
            USART3_RxLen = 0;
            memset(USART3_RxBuffer, 0, USART3_BUF_SIZE);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

//连接 WiFi / AT 指令
void Task_ESP(void* pvParameters) {
    while (1) {
        if (1 == ESP_WifiFlag) {
            // 连接WiFi
            USART_SendString(USART1, "\r\nESP01S starts connecting WiFi...\r\n");
            uint8_t connect_status = ESP01S_Connect_WiFi(wifi_ssid, wifi_password);
            // 蓝牙回复结果
            Send_Bluetooth_Response(connect_status);
            ESP_WifiFlag = 0;

            ESP01S_Connect_TCP_Server(ESP_TCP_SERVER_IP, ESP_TCP_SERVER_PORT);
            ESP01S_Send_TCP_Data("hello aliyun\n");
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

//状态回复 + 可能的日志
void Task_Report(void* pvParameters) {
    ;
}