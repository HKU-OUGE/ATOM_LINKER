#ifndef FIFO_H
#define FIFO_H


#define     FIFO_RX_MAX_SIZE      1024
#define     PAYLOAD_MAX_SIZE      256


typedef struct __Buffer_Type
{
  u8 buffer[1024];
  u8 vbuffer[1024];
  unsigned int write_count;
  unsigned int send_count;
  unsigned int send_pt_count;
  u8 * send_pt;
}Buffer_Type;

typedef struct
{
  uint8_t FIFO[FIFO_RX_MAX_SIZE];               //256 Bytes Fifo buffer,used by both iic/spi and uart.

  u8 *decode_point;                //Decode data pointer.
  
  u8 *dma_point;                   //Data pointer witch dma received and dacode function not used.

  uint8_t temp;                    //Dma received data pointer.

  uint8_t overrun_flag;            //If FIFO overrun, this flag will be setted 1.
  
  uint8_t overrun_count;           //It will be setted when fifo overrun by dma-rx-complete callback and will be reset by decode function.
  
  uint16_t data_count_in_fifo;     //The data that not decode by decode-function in FIFO, the value is from 0 to 255.
  
}FIFO_HandleType;

typedef struct
{
  u8 header_1;
  u8 header_2;
  u8 maddr;
  u8 cid;
  u8 mid;
  u8 payload_length;
  u8 crc;
  u8 tail;
  u8 protocolState;
  u8 payload_counter;
  
}MidProtocol_StructType;


#define USART_MAC_INIT                           0x00
#define USART_MAC_HEADER_1                       0X10///////////
#define USART_MAC_HEADER_2                       0X20/////////
#define USART_MAC_WRCONTROL                      0x30
#define USART_MAC_PAYLOAD_LENGTH_L                 0x40
#define USART_MAC_PAYLOAD_LENGTH_H                 0x50
#define USART_MAC_PAYLOAD_DATA                   0x60
#define USART_MAC_FOOTER                         0x70

#define DEVICE_MAC_INIT                                 0x00
#define DEVICE_MAC_HEADER1                              0x10
#define DEVICE_MAC_HEADER2                              0x20
#define DEVICE_MAC_MADDR                                0x30
#define DEVICE_MAC_CID                                  0x40
#define DEVICE_MAC_MID                                  0x50
#define DEVICE_MAC_PAYLOADLENGTH                        0x60
#define DEVICE_MAC_PAYLOAD                              0x70
#define DEVICE_MAC_CRC                                  0x80
#define DEVICE_MAC_FOOTER                               0x90


enum
{
  HOST_INTERFACE_TYPE_IIC,
  HOST_INTERFACE_TYPE_SPI
};


extern FIFO_HandleType Input_FIFO_Usart;
extern FIFO_HandleType Output_FIFO_Usart;
extern FIFO_HandleType Input_FIFO2Module;
extern FIFO_HandleType Output_FIFO2Module;

extern u8 UsartMacState;
extern u8 DeviceMacState;

extern MidProtocol_StructType UartRxProtocolHandle;
extern MidProtocol_StructType HostRxProtocolHandle;


extern u8 hostInterfaceType;


extern void FIFO_Init(void);
extern void HostRxMacControler(MidProtocol_StructType *HostHandle);
extern void UartRxMacControler(MidProtocol_StructType *UartHandle);
extern void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c);


extern void Buffer_Init(Buffer_Type* Buffer);
extern void Buffer_Send_Data(Buffer_Type* Buffer);
extern void Buffer_Send_N_Data(Buffer_Type* Buffer,unsigned int n);
extern unsigned char* Buffer_Get_Send_Pt(Buffer_Type* Buffer);

#endif
