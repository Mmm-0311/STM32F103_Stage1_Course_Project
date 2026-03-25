#ifndef __BSP_USART_H
#define __BSP_USART_H

#include <stdint.h>
// 以下两个头文件中的某些函数声明，在实现printf1函数时需要用到
#include <stdarg.h>
#include <stdio.h>
#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "timers.h"

// 串口缓冲区大小定义
#define USART1_BUF_SIZE 32
#define USART2_BUF_SIZE 64
#define USART3_BUF_SIZE 32

#define WIFI_SSID_MAX_LEN     64
#define WIFI_PASSWORD_MAX_LEN 64

typedef struct {
    char buffer[USART3_BUF_SIZE];
    uint16_t len;
} RawWifiData_t;

typedef struct {
    char ssid[WIFI_SSID_MAX_LEN];
    char password[WIFI_PASSWORD_MAX_LEN];
} WifiReq_t;

typedef struct {
    uint8_t status;      // 0: 成功, 1:失败
    char ssid[WIFI_SSID_MAX_LEN];
} WifiRsp_t;

typedef struct {
    char buffer[USART2_BUF_SIZE];
    uint16_t len;
} RawEspData_t;

// 串口帧间隔（调试波特率 + 10ms 近似）
#define UART2_FRAME_GAP_MS 8
#define UART3_FRAME_GAP_MS 8

// 全局队列句柄
extern QueueHandle_t queue_raw_wifi;
extern QueueHandle_t queue_wifi_rsp;
extern QueueHandle_t queue_esp_at_rsp;

// 定时器句柄
extern TimerHandle_t usart2_frame_timer;
extern TimerHandle_t usart3_frame_timer;

// 定时器回调
void USART2_FrameTimeoutCallback(TimerHandle_t xTimer);
void USART3_FrameTimeoutCallback(TimerHandle_t xTimer);

extern uint8_t USART2_RxBuffer[USART2_BUF_SIZE];
extern uint16_t USART2_RxLen;
extern volatile uint8_t USART2_RxFlag;

extern uint8_t USART3_RxBuffer[USART3_BUF_SIZE];
extern uint16_t USART3_RxLen;
extern volatile uint8_t USART3_RxFlag;

// 函数声明
void USART1_Config(uint32_t baudrate); // 调试串口
void USART2_Config(uint32_t baudrate); // ESP01S串口
void USART3_Config(uint32_t baudrate); // 蓝牙串口
void USART_SendString(USART_TypeDef* USARTx, char* str);
void printf1(const char* format, ...);

#endif /* __BSP_USART_H */