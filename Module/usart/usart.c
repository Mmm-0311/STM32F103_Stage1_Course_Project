#include "usart.h"
#include <string.h>

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

// 函数声明
static uint8_t check_wifi_format(uint8_t* buf, uint16_t len);

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
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
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
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;
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
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
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

// 参考：Module/usart/usart.c
void USART2_IRQHandler(void) {
    uint8_t rx_data;
    if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET && 0 == USART2_RxFlag) {
        rx_data = (uint8_t)USART_ReceiveData(USART2);
        // USART_SendString(USART1, "\r\n");

        // 不再使用 0 == USART2_RxFlag 作为接收保护，避免丢数据
        // 如果你要避免覆盖可用数据，可在上层读取后才清
        if (USART2_RxLen < USART2_BUF_SIZE - 1) {
            USART2_RxBuffer[USART2_RxLen++] = rx_data;
        } else {
            // 过长直接丢帧（或根据需求返回错误）
            USART2_RxLen = 0;
            USART2_RxFlag = 0;
            memset(USART2_RxBuffer, 0, USART2_BUF_SIZE);
        }

        // 按行结束 `\r\n` 作为一条可解析结果，保留整行数据
        if (USART2_RxLen >= 2 && USART2_RxBuffer[USART2_RxLen - 2] == '\r' &&
            USART2_RxBuffer[USART2_RxLen - 1] == '\n') {
            // 以 null 终止，去掉尾部 "\r\n"
            USART2_RxLen -= 2;
            USART2_RxBuffer[USART2_RxLen] = '\0';
            USART2_RxFlag = 1;
        }

        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    }
}

// USART3中断服务函数（蓝牙）
void USART3_IRQHandler(void) {
    uint8_t rx_data;

    static uint8_t receiving = 0; // 是否正在接收一帧

    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET && 0 == USART3_RxFlag) {
        rx_data = USART_ReceiveData(USART3);

        // 1. 起始符处理（关键！）
        if (rx_data == '!' && 0 == receiving) {
            USART3_RxLen = 0; // 清空缓冲区
            receiving = 1;    // 开始接收
        }

        //  2. 只有在接收状态才存数据
        if (receiving) {
            if (USART3_RxLen < USART3_BUF_SIZE - 1) {
                USART3_RxBuffer[USART3_RxLen++] = rx_data;

            } else {
                // 缓冲区溢出，直接丢弃本帧
                receiving = 0;
                USART3_RxLen = 0;
                memset(USART3_RxBuffer, 0, USART3_BUF_SIZE);
            }
        }

        //  3. 结束符处理
        if (rx_data == '!' && receiving && USART3_RxLen > 1) {
            receiving = 0; // 一帧结束

            // 加字符串结束符（给调试用）
            USART3_RxBuffer[USART3_RxLen] = '\0';

            if (check_wifi_format(USART3_RxBuffer, USART3_RxLen)) {
                USART3_RxFlag = 1;
                USART_SendString(USART1, "\r\nOK\r\n");
            } else {
                USART_SendString(USART1, "\r\nFORMAT ERROR\r\n");
                USART3_RxFlag = 0;
                USART3_RxLen = 0;
                memset(USART3_RxBuffer, 0, USART3_BUF_SIZE);
            }
        }
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
    }
}

// 格式校验函数：检查缓冲区是否符合!xxx=xxx!格式
static uint8_t check_wifi_format(uint8_t* buf, uint16_t len) {
    // 最小长度校验：!a=b! 至少5个字符
    if (len < 5) {
        return 0;
        USART_SendString(USART1, "len error\r\n");
    }

    // 1. 检查开头是否为!
    if (buf[0] != '!') {
        USART_SendString(USART1, "start error\r\n");
        return 0;
    }

    // 2. 检查结尾是否为!
    if (buf[len - 1] != '!') {
        USART_SendString(USART1, "tail error\r\n");
        return 0;
    }

    // 3. 检查是否包含且仅包含一个=（避免多个=的非法格式）
    uint8_t equal_count = 0;
    uint16_t equal_pos = 0;
    for (uint16_t i = 1; i < len - 1; i++) {
        if (buf[i] == '=') {
            equal_count++;
            equal_pos = i;
        }
    }
    // 必须有且仅有一个=，且=不能在开头/结尾附近
    if (equal_count != 1 || equal_pos == 1 || equal_pos == len - 2) {
        USART_SendString(USART1, "equal error\r\n");
        return 0;
    }

    // 所有校验通过
    return 1;
}
