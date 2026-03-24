#ifndef __BSP_USART_H
#define __BSP_USART_H

#include <stdint.h>
// 以下两个头文件中的某些函数声明，在实现printf1函数时需要用到
#include <stdarg.h>
#include <stdio.h>
#include "stm32f10x.h"

// 串口缓冲区大小定义
#define USART1_BUF_SIZE 32
#define USART2_BUF_SIZE 32
#define USART3_BUF_SIZE 32

// 全局变量声明（供其他模块使用）
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