#include "Delay.h"
#include "LED.h"
#include "stm32f10x.h"

int main(void) {
    LED_Init();
    Delay_Init();

    while (1) {
        // LED_Turn(); // 切换LED电平状态
        // Delay_Ms(500);
    }
}
