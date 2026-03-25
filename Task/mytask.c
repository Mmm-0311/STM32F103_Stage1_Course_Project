#include "mytask.h"
#include <string.h>
#include "FreeRTOS.h"
#include "bluetooth.h"
#include "esp01s.h"
#include "task.h"
#include "usart.h"

//接收与解析 -> 从队列获取原始蓝牙数据，解析后处理WiFi连接
void Task_Bluetooth(void* pvParameters) {
    RawWifiData_t raw_data;
    while (xQueueReceive(queue_raw_wifi, &raw_data, portMAX_DELAY) == pdTRUE) {
        USART_SendString(USART1, "Received raw Bluetooth data from queue: ");
        USART_SendString(USART1, raw_data.buffer);
        USART_SendString(USART1, "\r\n");

        // 解析WiFi信息
        Parse_WiFi_Info(raw_data.buffer);

        // 打印解析结果
        USART_SendString(USART1, "Parsed result: \r\n");
        USART_SendString(USART1, "SSID = ");
        USART_SendString(USART1, wifi_ssid);
        USART_SendString(USART1, "\r\n");
        USART_SendString(USART1, "PASS = ");
        USART_SendString(USART1, wifi_password);
        USART_SendString(USART1, "\r\n");

        //vTaskDelay(pdMS_TO_TICKS(2000));

        // 连接WiFi
        USART_SendString(USART1, "\r\nESP01S starts connecting WiFi...\r\n");
        uint8_t connect_status = ESP01S_Connect_WiFi(wifi_ssid, wifi_password);

        // 发送结果到响应队列
        WifiRsp_t rsp;
        rsp.status = connect_status;
        strncpy(rsp.ssid, wifi_ssid, WIFI_SSID_MAX_LEN);

        if (queue_wifi_rsp != NULL) {
            xQueueSend(queue_wifi_rsp, &rsp, 0);
        }

        // 如果成功，连接TCP服务器并发送数据
        if (connect_status == 0) {
            ESP01S_Connect_TCP_Server(ESP_TCP_SERVER_IP, ESP_TCP_SERVER_PORT);
            ESP01S_Send_TCP_Data("hello aliyun\n");
        }
    }
}

//连接 WiFi / AT 指令（保留旧接口）
void Task_ESP(void* pvParameters) {
    (void)pvParameters;
    while (1) {
        // 可选：从queue_wifi_req中接收请求，由Task_Bluetooth完成时，该任务可用作状态上报或备用策略。
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

//状态回复 + 可能的日志
void Task_Report(void* pvParameters) {
    WifiRsp_t rsp;
    while (xQueueReceive(queue_wifi_rsp, &rsp, portMAX_DELAY) == pdTRUE) {
        // 通知蓝牙模块
        if (rsp.status == 0) {
            USART_SendString(USART3, "{\"status\":0, \"wifi_name\": \"");
            USART_SendString(USART3, rsp.ssid);
            USART_SendString(USART3, "\"}\r\n");
        } else {
            USART_SendString(USART3, "Distribution network failure!\r\n");
        }

        USART_SendString(USART1, "Report sent via Bluetooth\r\n");
    }
}