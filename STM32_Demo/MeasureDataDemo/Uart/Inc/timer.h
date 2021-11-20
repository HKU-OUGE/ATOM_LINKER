#ifndef __timer_H
#define __timer_H



/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "main.h"
#include "stm32f4xx_hal_tim.h"



extern void Atom_Tim_IRQHandler(TIM_HandleTypeDef *htim);
extern void ATOM_TIM_INIT(void);

#endif