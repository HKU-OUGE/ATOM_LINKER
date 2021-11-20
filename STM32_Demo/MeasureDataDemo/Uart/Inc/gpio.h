/**
  ******************************************************************************
  * File Name          : gpio.h
  * Description        : This file contains all the functions prototypes for 
  *                      the gpio  
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __gpio_H
#define __gpio_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "main.h"

   typedef enum
{
  LED1 = 0,
  LED2 = 1,
  LED3 = 2,
  LED4 = 3,
  LED5 = 4,
  LED6 = 5,
  LED7 = 6,
  LED8 = 7,
  LED9 = 8,
  LED10 = 9
}Led_TypeDef;

enum
{
  LED1_NORMAL,
  LED1_PWM
};

#define LEDn                             10

#define LED1_PIN                         GPIO_PIN_1                                  
#define LED1_GPIO_PORT                   GPIOA                                        
#define LED1_GPIO_CLK_ENABLE()           __GPIOA_CLK_ENABLE()                         
#define LED1_GPIO_CLK_DISABLE()          __GPIOA_CLK_DISABLE()                        

/* Debug use now *//////////////////////////////////////////////////////////////////////
#define LED2_PIN                         GPIO_PIN_12//Saber PIN 7(SPI2 NSS)           //
#define LED2_GPIO_PORT                   GPIOB                                        //
#define LED2_GPIO_CLK_ENABLE()           __GPIOB_CLK_ENABLE()                         //
#define LED2_GPIO_CLK_DISABLE()          __GPIOB_CLK_DISABLE()                        //
                                                                                      //
#define LED3_PIN                         GPIO_PIN_13//Saber PIN 10(SPI2 SCK)          //
#define LED3_GPIO_PORT                   GPIOB                                        //
#define LED3_GPIO_CLK_ENABLE()           __GPIOB_CLK_ENABLE()                         //
#define LED3_GPIO_CLK_DISABLE()          __GPIOB_CLK_DISABLE()                        //
                                                                                      //
#define LED4_PIN                         GPIO_PIN_14//Saber PIN 9(SPI2 MISO)          //
#define LED4_GPIO_PORT                   GPIOB                                        //
#define LED4_GPIO_CLK_ENABLE()           __GPIOB_CLK_ENABLE()                         //
#define LED4_GPIO_CLK_DISABLE()          __GPIOB_CLK_DISABLE()                        //
                                                                                      //
#define LED5_PIN                         GPIO_PIN_4//Saber PIN 12(DRDY)               //
#define LED5_GPIO_PORT                   GPIOA                                        //
#define LED5_GPIO_CLK_ENABLE()           __GPIOA_CLK_ENABLE()                         //
#define LED5_GPIO_CLK_DISABLE()          __GPIOA_CLK_DISABLE()                        //
                                                                                      //
#define LED6_PIN                         GPIO_PIN_15//Saber PIN 8(SPI2 MOSI)          //
#define LED6_GPIO_PORT                   GPIOB                                        //
#define LED6_GPIO_CLK_ENABLE()           __GPIOB_CLK_ENABLE()                         //
#define LED6_GPIO_CLK_DISABLE()          __GPIOB_CLK_DISABLE()                        //
                                                                                      //
#define LED7_PIN                         GPIO_PIN_0//Saber PIN 16(SYNC IN)            //
#define LED7_GPIO_PORT                   GPIOB                                        //
#define LED7_GPIO_CLK_ENABLE()           __GPIOB_CLK_ENABLE()                         //
#define LED7_GPIO_CLK_DISABLE()          __GPIOB_CLK_DISABLE()                        //
                                                                                      //
#define LED8_PIN                         GPIO_PIN_1//Saber PIN 11(PSEL 0)             //
#define LED8_GPIO_PORT                   GPIOB                                        //
#define LED8_GPIO_CLK_ENABLE()           __GPIOB_CLK_ENABLE()                         //
#define LED8_GPIO_CLK_DISABLE()          __GPIOB_CLK_DISABLE()                        //
                                                                                      //
#define LED9_PIN                         GPIO_PIN_2//Saber PIN 15(PSBL 1)             //
#define LED9_GPIO_PORT                   GPIOB                                        //
#define LED9_GPIO_CLK_ENABLE()           __GPIOB_CLK_ENABLE()                         //
#define LED9_GPIO_CLK_DISABLE()          __GPIOB_CLK_DISABLE()                        //
                                                                                      //
#define LED10_PIN                        GPIO_PIN_11//Saber PIN 19(USART1 CTS)        //
#define LED10_GPIO_PORT                  GPIOA                                        //
#define LED10_GPIO_CLK_ENABLE()          __GPIOA_CLK_ENABLE()                         //
#define LED10_GPIO_CLK_DISABLE()         __GPIOA_CLK_DISABLE()                        //
/* OVER *///////////////////////////////////////////////////////////////////////////////
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_GPIO_Init(void);
extern int BSP_LED_Init(Led_TypeDef Led);
/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ pinoutConfig_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
