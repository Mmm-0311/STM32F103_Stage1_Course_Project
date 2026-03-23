#include "Delay.h"
volatile uint32_t g_delayTick = 0;
/* 全局毫秒节拍计数 */

/**
 * @brief  初始化延时模块
 * @param  无
 * @retval 无
 * @note   配置 SysTick 每 1ms 产生一次中断
 *         系统时钟默认为 72MHz
 */
void Delay_Init(void) {
    /*
        72MHz 时钟下：

        1ms = 72000 个时钟周期

        SysTick 计数次数 = LOAD + 1
        所以：
        LOAD = 72000 - 1
    */
    SysTick->LOAD = 72000U - 1U;

    /* 清空当前计数值 */
    SysTick->VAL = 0U;

    /*
        CTRL 寄存器：
        bit0 ENABLE = 1      启动计数器
        bit1 TICKINT = 1     开启 SysTick 中断
        bit2 CLKSOURCE = 1   选择 HCLK 作为时钟源
    */
    SysTick->CTRL = (1U << 0) | (1U << 1) | (1U << 2);
}

/**
 * @brief  获取当前系统毫秒节拍值
 * @param  无
 * @retval 当前毫秒节拍值
 */
uint32_t Delay_GetTick(void) {
    return g_delayTick;
}

/**
 * @brief  提供毫秒级延时
 * @param  ms 延时时长（单位：毫秒）
 * @retval 无
 * @note   本函数属于阻塞式延时
 */
void Delay_Ms(uint32_t ms) {
    uint32_t start = g_delayTick;

    while ((g_delayTick - start) < ms)
        ;
}

/**
 * @brief  提供秒级延时
 * @param  s 延时时长（单位：秒）
 * @retval 无
 */
void Delay_S(uint32_t s) {
    while (s--) {
        Delay_Ms(1000);
    }
}

void SysTick_Handler(void) {
    g_delayTick++;
}
