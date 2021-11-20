#include "global.h"
#include "stm32f4xx_hal_tim.h"
#define TIM2_INT_PRIORITY              ((uint32_t)0x06)
TIM_HandleTypeDef Set_IIC_Tim;
extern u8 TxBuf[128];
extern I2C_HandleTypeDef hi2c1;
extern unsigned int send_i;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
//  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_SET);
//  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_RESET);
  __HAL_TIM_DISABLE_IT(&Set_IIC_Tim,TIM_IT_UPDATE);
  __HAL_TIM_DISABLE(&Set_IIC_Tim);
  //HAL_I2C_Master_Sequential_Transmit_IT(&hi2c1,iic_addr,TxBuf,1,I2C_NEXT_FRAME);
  send_i++;
  __HAL_TIM_SET_COUNTER(&Set_IIC_Tim,0);
}



void Atom_Tim_IRQHandler(TIM_HandleTypeDef *htim)
{
  /* TIM Update event */
  
  if(__HAL_TIM_GET_FLAG(htim, TIM_FLAG_UPDATE) != RESET)
  {
    if(__HAL_TIM_GET_IT_SOURCE(htim, TIM_IT_UPDATE) !=RESET)
    {
      __HAL_TIM_CLEAR_IT(htim, TIM_IT_UPDATE);
      HAL_TIM_PeriodElapsedCallback(htim);
    }
  }
}

void ATOM_TIM_INIT(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig;
  GPIO_InitTypeDef GPIO_InitStruct;

  /**TIM2 GPIO Configuration    
  PA1     ------> TIM2_CH2 
  */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
  Set_IIC_Tim.Instance = TIM2;
  Set_IIC_Tim.Init.Prescaler = 1;
  Set_IIC_Tim.Init.CounterMode = TIM_COUNTERMODE_UP;
  Set_IIC_Tim.Init.Period = 2000;
  Set_IIC_Tim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  Set_IIC_Tim.Init.RepetitionCounter = 0;
  
  HAL_TIM_Base_Init(&Set_IIC_Tim);
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&Set_IIC_Tim, &sClockSourceConfig);
  /* Peripheral clock enable */
  __HAL_RCC_TIM2_CLK_ENABLE();
  /* TIM1 interrupt Init */
  HAL_NVIC_SetPriority(TIM2_IRQn, TIM2_INT_PRIORITY, 0);
  HAL_NVIC_EnableIRQ(TIM2_IRQn);
    
  __HAL_TIM_DISABLE_IT(&Set_IIC_Tim,TIM_IT_UPDATE);
  __HAL_TIM_DISABLE(&Set_IIC_Tim);
  __HAL_TIM_SET_COUNTER(&Set_IIC_Tim,0);
  __HAL_TIM_CLEAR_IT(&Set_IIC_Tim, TIM_IT_UPDATE);
}