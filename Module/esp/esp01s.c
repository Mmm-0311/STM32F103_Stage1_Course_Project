#include "esp01s.h"
#include <stdio.h>
#include <string.h>

uint8_t ESP_WifiFlag = 0;

// 发送AT指令并等待响应
uint8_t ESP01S_Send_AT_Cmd(char* cmd, char* expected_resp, uint32_t timeout) {
    memset(USART2_RxBuffer, 0, USART2_BUF_SIZE);
    USART2_RxLen = 0;
    USART2_RxFlag = 0;

    USART_SendString(USART1, "\r\nESP01S_Send_AT_Cmd\r\n");
    // 发送AT指令
    USART_SendString(USART2, cmd);
    USART_SendString(USART1, "Send AT command: ");
    USART_SendString(USART1, cmd);

    // 等待响应
    uint32_t t = 0;
    while (t < timeout) {
        if (USART2_RxFlag == 1) {
            USART2_RxBuffer[USART2_RxLen] = '\0';
            if (strstr((char*)USART2_RxBuffer, expected_resp) != NULL) {
                USART_SendString(USART1, "Response successful: ");
                USART_SendString(USART1, (char*)USART2_RxBuffer);
                USART_SendString(USART1, "\r\n");
                return 0;
            } else {
                USART_SendString(USART1, "Response failed:");
                USART_SendString(USART1, (char*)USART2_RxBuffer);
                USART_SendString(USART1, "\r\n");
                return 1;
            }
            USART2_RxFlag = 0;
            USART2_RxLen = 0;
            memset(USART2_RxBuffer, 0, USART2_RxLen);
        }
        Delay_Ms(1);
        t++;
    }
    USART_SendString(USART1, "AT instruction timeout\r\n");
    return 2;
}

// 连接WiFi
uint8_t ESP01S_Connect_WiFi(char* ssid, char* password) {
    char cmd[64];
    char current_ssid[32] = {0};

    // // 1. AT测试
    // if (ESP01S_Send_AT_Cmd("AT\r\n", "OK", 1000) != 0) {
    //     USART_SendString(USART1, "ESP01S AT test failed\r\n");
    //     return 1;
    // }

    // 1. 关闭回显
    ESP01S_Send_AT_Cmd("ATE0\r\n", "OK", 1000);

    // 3. 设置STA模式
    if (ESP01S_Send_AT_Cmd("AT+CWMODE=1\r\n", "OK", 1000) != 0) {
        USART_SendString(USART1, "Set STA mode failed\r\n");
        return 1;
    }

    // 4. 查询当前WiFi
    if (ESP01S_Get_Current_SSID(current_ssid) == 0) {
        USART_SendString(USART1, "Current SSID: ");
        USART_SendString(USART1, current_ssid);
        USART_SendString(USART1, "\r\n");

        // 如果已经连接目标WiFi
        if (strcmp(current_ssid, ssid) == 0) {
            USART_SendString(USART1, "Already connected target WiFi\r\n");
            return 0;
        } else {
            // 5. 断开旧WiFi
            USART_SendString(USART1, "the connected wifi name does not match\r\n");
            ESP01S_Send_AT_Cmd("AT+CWQAP\r\n", "OK", 1000);
        }
    } else {
        USART_SendString(USART1, "There is currently no wifi connection\r\n");
    }

    // Delay_Ms(50);

    // 6. 连接新WiFi
    memset(cmd, 0, sizeof(cmd));
    sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, password);
    USART_SendString(USART1, cmd);

    if (ESP01S_Send_AT_Cmd(cmd, "WIFI CONNECTED", 1000) != 0) {
        USART_SendString(USART1, "WiFi connection failed\r\n");
        return 1;
    }

    USART_SendString(USART1, "WiFi connected successfully\r\n");

    return 0;
}

// bsp_esp01s.c 中新增函数：连接TCP服务器
uint8_t ESP01S_Connect_TCP_Server(char* ip, uint16_t port) {
    // 1. 关闭透传（若之前开启）
    if (ESP01S_Send_AT_Cmd("AT+CIPMODE=0\r\n", "OK", 1000) != 0) {
        USART_SendString(USART1, "Close transparent transmission failed\r\n");
    }

    // 2. 建立TCP连接（格式：AT+CIPSTART="TCP","IP",端口\r\n）
    char tcp_cmd[64] = {0};
    sprintf(tcp_cmd, "AT+CIPSTART=\"TCP\",\"%s\",%d\r\n", ip, port);
    if (ESP01S_Send_AT_Cmd(tcp_cmd, "CONNECT", 3000) != 0) {
        USART_SendString(USART1, "TCP connect failed\r\n");
        return 1;
    }
    USART_SendString(USART1, "TCP connect successful!\r\n");
    return 0;
}

// 发送数据到TCP服务器
uint8_t ESP01S_Send_TCP_Data(char* data) {
    // 1. 发送数据长度（格式：AT+CIPSEND=长度\r\n）
    char send_len_cmd[32] = {0};
    uint16_t data_len = strlen(data);
    sprintf(send_len_cmd, "AT+CIPSEND=%d\r\n", data_len);

    // 等待模块返回">"后，再发送实际数据
    if (ESP01S_Send_AT_Cmd(send_len_cmd, ">", 1000) != 0) {
        USART_SendString(USART1, "Prepare send data failed\r\n");
        return 1;
    }

    // 2. 发送实际数据
    USART_SendString(USART2, data);
    USART_SendString(USART1, "Send TCP data: ");
    USART_SendString(USART1, data);
    USART_SendString(USART1, "\r\n");

    // 3. 等待发送成功响应（SEND OK）
    Delay_Ms(500); // 等待数据发送完成
    if (strstr((char*)USART2_RxBuffer, "SEND OK") == NULL) {
        USART_SendString(USART1, "TCP data send failed\r\n");
        return 1;
    }
    return 0;
}

// 获取当前WiFi名称
uint8_t ESP01S_Get_Current_SSID(char* ssid_out) {
    char* p;

    if (ESP01S_Send_AT_Cmd("AT+CWJAP?\r\n", "+CWJAP:", 1000) != 0) {
        return 1;
    }

    // 示例返回：+CWJAP:"your_ssid"
    p = strstr((char*)USART2_RxBuffer, "\"");
    if (p == NULL) {
        return 1;
    }

    char* p2 = strstr(p + 1, "\"");
    if (p2 == NULL) {
        return 1;
    }

    uint16_t len = p2 - (p + 1);
    strncpy(ssid_out, p + 1, len);
    ssid_out[len] = '\0';

    return 0;
}
