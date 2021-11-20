#ifndef GLOBAL_H
#define GLOBAL_H


#include "stm32f4xx_hal.h"
#include "core_cm4.h"
#include "cmsis_os.h"
#include <string.h>

typedef uint8_t   u8;
typedef uint16_t  u16;
typedef uint32_t  u32;

extern SemaphoreHandle_t HostRxSemaphore;
extern SemaphoreHandle_t SendSemaphore;


//#include "stm32f4xx_hal_tim.h"
#include "main.h"
#include "usart.h"
#include "gpio.h"
#include "fifo.h"
#include "timer.h"

#include "atom_macro.h"
#include "atom_macro_debug.h"
#include "init.h"
//#include "atom_fifo.h"
#include "atomprotocol.h"

#define NONE_MODE               0x00
#define MEASURE_MODE            0x01
#define MESSAGE_MODE            0x02
#define FIFO_STATUS_MODE        0x03
#define RESPONSE_MODE           0x04


#endif
