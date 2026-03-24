#include "esp01s.h"
#include <stdio.h>
#include <string.h>

uint8_t ESP_WifiFlag = 0;

// 发送AT指令并等待响应
uint8_t ESP01S_Send_AT_Cmd(char* cmd, char* expected_resp, uint32_t timeout) {
    memset(USART2_RxBuffer, 0, USART2_BUF_SIZE);
    USART2_RxLen = 0;
    USART2_RxFlag = 0;

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
                return 0;
            } else {
                USART_SendString(USART1, "Response failed:");
                USART_SendString(USART1, (char*)USART2_RxBuffer);
                return 1;
            }
        }
        Delay_Ms(1);
        t++;
    }
    USART_SendString(USART1, "AT instruction timeout\r\n");
    return 2;
}

// 连接WiFi
uint8_t ESP01S_Connect_WiFi(char* ssid, char* password) {
    char cmd[128];
    // 1. AT测试
    if (ESP01S_Send_AT_Cmd("AT\r\n", "OK", 1000) != 0) {
        USART_SendString(USART1, "ESP01S AT test failed\r\n");
        return 1;
    }

    // 2. 关闭回显
    if (ESP01S_Send_AT_Cmd("ATE0\r\n", "OK", 1000) != 0) {
        USART_SendString(USART1, "Close echo failed\r\n");
    }

    // 3. 设置STA模式
    if (ESP01S_Send_AT_Cmd("AT+CWMODE=1\r\n", "OK", 1000) != 0) {
        USART_SendString(USART1, "Set STA mode failed\r\n");
        return 1;
    }

    // 4. 断开当前WiFi
    if (ESP01S_Send_AT_Cmd("AT+CWQAP\r\n", "OK", 2000) != 0) {
        USART_SendString(USART1, "Disconnect WiFi failed\r\n");
    }

    Delay_S(2);

    // 5. 连接WiFi
    memset(cmd, 0, sizeof(cmd));
    sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, password);

    if (ESP01S_Send_AT_Cmd(cmd, "WIFI CONNECTED", 20000) != 0) {
        USART_SendString(USART1, "WiFi connection failed\r\n");
        return 1;
    }

    USART_SendString(USART1, "WiFi connected successfully\r\n");

    Delay_S(3);
    // 新增：连接阿里云TCP服务器（替换为你的服务器公网IP和端口）
    if (ESP01S_Connect_TCP_Server("121.41.231.209", 9003) != 0) {
        USART_SendString(USART1, "Connect TCP server failed\r\n");
        return 2; // 区别于WiFi连接失败
    }

    // 示例：发送WiFi名称到服务器
    char tcp_data[64] = {0};
    sprintf(tcp_data, "WiFi connected: %s\n", ssid);
    ESP01S_Send_TCP_Data(tcp_data);
    ESP01S_Send_TCP_Data("hello aliyun\n");

    USART_SendString(USART1, "WiFi + TCP server connect successful!\r\n");
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
