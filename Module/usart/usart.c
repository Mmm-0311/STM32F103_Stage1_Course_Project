#include "usart.h"
#include <string.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "timers.h"

#define UART2_FRAME_GAP_MS 8
#define UART3_FRAME_GAP_MS 8

// 全局队列句柄
QueueHandle_t queue_raw_wifi = NULL;
QueueHandle_t queue_wifi_rsp = NULL;
QueueHandle_t queue_esp_at_rsp = NULL;

// 定时器句柄
TimerHandle_t usart2_frame_timer = NULL;
TimerHandle_t usart3_frame_timer = NULL;

// 全局变量定义
uint8_t USART1_RxBuffer[USART1_BUF_SIZE] = {0};
uint16_t USART1_RxLen = 0;
volatile uint8_t USART1_RxFlag = 0;

uint8_t USART2_RxBuffer[USART2_BUF_SIZE] = {0};
uint16_t USART2_RxLen = 0;
volatile uint8_t USART2_RxFlag = 0;

uint8_t USART3_RxBuffer[USART3_BUF_SIZE] = {0};
uint16_t USART3_RxLen = 0;
volatile uint8_t USART3_RxFlag = 0;

void USART2_FrameTimeoutCallback(TimerHandle_t xTimer) {
    if (USART2_RxLen == 0) {
        return;
    }

    RawEspData_t esp_data;
    esp_data.len = USART2_RxLen;
    if (esp_data.len >= USART2_BUF_SIZE) {
        esp_data.len = USART2_BUF_SIZE - 1;
    }
    memcpy(esp_data.buffer, USART2_RxBuffer, esp_data.len);
    esp_data.buffer[esp_data.len] = '\0';

    USART2_RxLen = 0;
    memset(USART2_RxBuffer, 0, USART2_BUF_SIZE);

    if (queue_esp_at_rsp != NULL) {
        xQueueSend(queue_esp_at_rsp, &esp_data, 0);
    }
}

void USART3_FrameTimeoutCallback(TimerHandle_t xTimer) {
    if (USART3_RxLen == 0) {
        return;
    }

    RawWifiData_t raw_data;
    raw_data.len = USART3_RxLen;
    if (raw_data.len >= USART3_BUF_SIZE) {
        raw_data.len = USART3_BUF_SIZE - 1;
    }
    memcpy(raw_data.buffer, USART3_RxBuffer, raw_data.len);
    raw_data.buffer[raw_data.len] = '\0';

    USART3_RxLen = 0;
    memset(USART3_RxBuffer, 0, USART3_BUF_SIZE);

    if (queue_raw_wifi != NULL) {
        xQueueSend(queue_raw_wifi, &raw_data, 0);
    }
}

// USART1初始化（PA9-TX, PA10-RX）
void USART1_Config(uint32_t baudrate) {
    GPIO_InitTypeDef GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    // TX引脚配置
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // RX引脚配置
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 串口参数配置
    USART_InitStruct.USART_BaudRate = baudrate;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART1, &USART_InitStruct);

    // 开启接收中断
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    // NVIC配置
    NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 12;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    USART_Cmd(USART1, ENABLE);
}

// USART2初始化（PA2-TX, PA3-RX）
void USART2_Config(uint32_t baudrate) {
    GPIO_InitTypeDef GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    // TX引脚配置
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // RX引脚配置
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 串口参数配置
    USART_InitStruct.USART_BaudRate = baudrate;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART2, &USART_InitStruct);

    // 开启接收中断
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

    // NVIC配置
    NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 12;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    USART_Cmd(USART2, ENABLE);
}

// USART3初始化（PB10-TX, PB11-RX）
void USART3_Config(uint32_t baudrate) {
    GPIO_InitTypeDef GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    // TX引脚配置
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    // RX引脚配置
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    // 串口参数配置
    USART_InitStruct.USART_BaudRate = baudrate;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART3, &USART_InitStruct);

    // 开启接收中断
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

    // NVIC配置
    NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 12;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    USART_Cmd(USART3, ENABLE);
}

// 串口发送字符串
void USART_SendString(USART_TypeDef* USARTx, char* str) {
    while (*str != '\0') {
        while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET)
            ;
        USART_SendData(USARTx, *str);
        str++;
    }
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET)
        ;
}

// USART1中断服务函数
void USART1_IRQHandler(void) {
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        USART_ReceiveData(USART1);
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}

// USART2中断服务函数（ESP01S）
void USART2_IRQHandler(void) {
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
        uint8_t rx_data = (uint8_t)USART_ReceiveData(USART2);

        if (USART2_RxLen < USART2_BUF_SIZE - 1) {
            USART2_RxBuffer[USART2_RxLen++] = rx_data;
        } else {
            USART_SendString(USART1, "USART2_RxBuffer overflow!\r\n");
            USART2_RxLen = 0;
        }

        // 重启帧间隔定时器
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        if (usart2_frame_timer != NULL) {
            xTimerResetFromISR(usart2_frame_timer, &xHigherPriorityTaskWoken);
        }
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    }
}

// USART3中断服务函数（蓝牙）
void USART3_IRQHandler(void) {
    uint8_t rx_data;
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) {
        rx_data = USART_ReceiveData(USART3);

        if (USART3_RxLen < USART3_BUF_SIZE - 1) {
            USART3_RxBuffer[USART3_RxLen++] = rx_data;
        } else {
            USART3_RxLen = 0;
            memset(USART3_RxBuffer, 0, USART3_BUF_SIZE);
        }

        // 重启帧间隔定时器
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        if (usart3_frame_timer != NULL) {
            xTimerResetFromISR(usart3_frame_timer, &xHigherPriorityTaskWoken);
        }
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
    }
}
