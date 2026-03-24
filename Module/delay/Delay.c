#include "Delay.h"

volatile uint32_t g_delayTick = 0;  // 全局毫秒滴答计数器

/**
 * @brief 使用 TIM2 初始化延时模块
 * @param 无
 * @retval 无
 * @note 在 72MHz 系统时钟下配置 TIM2 生成 1ms 中断
 */
void Delay_Init(void) {
    // 启用 TIM2 时钟
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    // 配置 TIM2 为 1ms 中断
    // 预分频器：72MHz / 72000 = 1kHz (1ms 滴答)
    TIM2->PSC = 72000 - 1;
    // 自动重载：1ms 周期
    TIM2->ARR = 1;
    // 启用更新中断
    TIM2->DIER |= TIM_DIER_UIE;
    // 启用 TIM2
    TIM2->CR1 |= TIM_CR1_CEN;

    // 在 NVIC 中启用 TIM2 中断
    NVIC_EnableIRQ(TIM2_IRQn);
    NVIC_SetPriority(TIM2_IRQn, 0);  // 设置优先级（根据需要调整）
}

/**
 * @brief 获取当前系统毫秒滴答值
 * @param 无
 * @retval 当前毫秒滴答值
 */
uint32_t Delay_GetTick(void) {
    return g_delayTick;
}

/**
 * @brief 提供毫秒级延时
 * @param ms 延时时长（单位：毫秒）
 * @retval 无
 * @note 这是阻塞式延时
 */
void Delay_Ms(uint32_t ms) {
    uint32_t start = g_delayTick;
    while ((g_delayTick - start) < ms);
}

/**
 * @brief 提供秒级延时
 * @param s 延时时长（单位：秒）
 * @retval 无
 */
void Delay_S(uint32_t s) {
    while (s--) {
        Delay_Ms(1000);
    }
}

// TIM2 中断处理程序（替换 SysTick_Handler）
void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR &= ~TIM_SR_UIF;  // 清除中断标志
        g_delayTick++;            // 递增滴答计数器
    }
}