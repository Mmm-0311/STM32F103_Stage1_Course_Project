#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f10x.h"

extern volatile uint32_t g_delayTick;

void Delay_Init(void);
void Delay_Ms(uint32_t ms);
void Delay_S(uint32_t s);
uint32_t Delay_GetTick(void);

#endif