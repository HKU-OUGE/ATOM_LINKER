#include "init.h"
#include "global.h"

extern u8 temp_data;
extern Buffer_Type HOST_Buffer;
unsigned char reset_flag = 1;
unsigned char ready_flag = 0;
unsigned char measure_flag = 0;
unsigned char switch_measure_flag = 0;
unsigned char recieve_flag = 0;
unsigned int packet_num = 91 - 8;
MidProtocol_StructType UartHandle;
extern UART_HandleTypeDef huart1;

void all_init()
{
  
  MX_GPIO_Init();
  MX_USART6_UART_Init();
  MX_USART1_UART_Init();
  Buffer_Init(&HOST_Buffer);
  HAL_UART_Receive_IT(&huart1,&temp_data,1);
  UartHandle.header_1 = 0;
  UartHandle.header_2 = 0;
  UartHandle.maddr = 0;
  UartHandle.cid = 0;
  UartHandle.mid = 0;
  UartHandle.payload_length = 0;
  UartHandle.payload_counter = 0;
  UartHandle.crc = 0;
  UartHandle.tail = 0;
  UartHandle.protocolState = DEVICE_MAC_INIT;
 }

