#include "LED.h"

/*
    @brief 初始化LED（开漏接法，PA3引脚）
    @param
    @retval
*/
void LED_Init(void) {
    // 开启时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    // 配置PA3引脚
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 设置默认熄灭状态
    GPIO_SetBits(GPIOA, GPIO_Pin_3);
}

/*
    @brief 开灯
    @param
    @retval
*/
void LED_On(void) {
    GPIO_ResetBits(GPIOA, GPIO_Pin_3);
}

/*
    @brief 熄灯
    @param
    @retval
*/
void LED_OFF(void) {
    GPIO_SetBits(GPIOA, GPIO_Pin_3);
}

void LED_Turn(void) {
    // 读取PA3当前电平状态
    uint8_t pin_state = GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_3);

    // 根据当前状态翻转：1（置位，熄灭）→0（复位，点亮）；0→1
    if (pin_state == SET) {
        GPIO_ResetBits(GPIOA, GPIO_Pin_3); // 当前熄灭，改为点亮
    } else {
        GPIO_SetBits(GPIOA, GPIO_Pin_3); // 当前点亮，改为熄灭
    }
}