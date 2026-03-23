#ifndef __LED_H__
#define __LED_H__

#include "stm32f10x.h"

// 初始化LED（开漏接法，PA3引脚）
void LED_Init(void);

// 开灯
void LED_On(void);

// 熄灭
void LED_OFF(void);

// 切换状态
void LED_Turn(void);

#endif //!__LED_H__