#include "global.h"

#define FILL_LEN        6

FIFO_HandleType Input_FIFO_Usart;
FIFO_HandleType Output_FIFO_Usart;
FIFO_HandleType Input_FIFO2Module;
FIFO_HandleType Output_FIFO2Module;
Buffer_Type HOST_Buffer;
MidProtocol_StructType UartRxProtocolHandle;
MidProtocol_StructType HostRxProtocolHandle;
unsigned char temp_data = 0;
static unsigned int send_data_count = 0;
u8 errorCode_Ack;

u8 receiveAck = 0 ;
u8 receiveCid ;
u8 receiveMid ;
extern u8 ReDataFlag;
//u8 hostInterfaceType;
//u8 clear_bit[PAYLOAD_MAX_SIZE] = {0x00};
//int payload_length;

void Buffer_Init(Buffer_Type* Buffer)
{
  Buffer->send_count = 0;
  Buffer->write_count = 0;
  Buffer->send_pt = Buffer->buffer;
  Buffer->send_pt_count = 0;
}

void Buffer_Send_Data(Buffer_Type* Buffer)
{
  Buffer->send_pt ++;
  Buffer->send_pt_count++;
}

void Buffer_Send_N_Data(Buffer_Type* Buffer,unsigned int n)
{
  unsigned int num = 0;
  while(num < n)
  {
    Buffer_Send_Data(Buffer);
    num++;
    if(Buffer->send_pt_count >= 1024)
    {
      Buffer->send_pt = Buffer->buffer;
      Buffer->send_pt_count = 0;
    }
  }
}

/*unsigned char* Buffer_Get_Send_Pt(Buffer_Type* Buffer)
{
  return Buffer->send_pt;
}*/

unsigned char check_cid(unsigned char cid)
{
  unsigned char ret = 1;
  
  return ret;
}

unsigned char check_mid(unsigned char mid)
{
  unsigned char ret = 1;
  
  return ret;
}

void UartRxMacControler(MidProtocol_StructType *UartHandle)
{
  unsigned char temp_input = 0;
  temp_input = temp_data;
  send_data_count ++;
  switch(UartHandle->protocolState)
    {
      case DEVICE_MAC_INIT:
        UartHandle->protocolState = DEVICE_MAC_HEADER1;

      case DEVICE_MAC_HEADER1:
        UartHandle->header_1 = temp_input;
        if(UartHandle->header_1 == 0x41)
        {
          UartHandle->protocolState = DEVICE_MAC_HEADER2;
          
        }
        else
        {
          send_data_count = 0;
          Buffer_Send_N_Data(&HOST_Buffer,1);
          UartHandle->protocolState = DEVICE_MAC_INIT;
        }
        break;
      case DEVICE_MAC_HEADER2:
        UartHandle->header_2 = temp_input;
        if(UartHandle->header_2 == 0x78)
        {
          UartHandle->protocolState = DEVICE_MAC_MADDR;
           
        }
        else
        {
          send_data_count = 0;
          Buffer_Send_N_Data(&HOST_Buffer,1);
          UartHandle->protocolState = DEVICE_MAC_INIT;
        }        
        break;
      case DEVICE_MAC_MADDR:
        UartHandle->maddr = temp_input;
				if(UartHandle->maddr == 0xFF)
				{
					UartHandle->protocolState = DEVICE_MAC_CID;
				}
				else
				{
					send_data_count = 0;
          Buffer_Send_N_Data(&HOST_Buffer,1);
					UartHandle->protocolState = DEVICE_MAC_INIT;
				}
                       
        break;
      case DEVICE_MAC_CID:
        UartHandle->cid = temp_input;
        
            
        if(check_cid(temp_input))
        {
          UartHandle->protocolState = DEVICE_MAC_MID;
          
        }
        else
        {
          send_data_count = 0;
          Buffer_Send_N_Data(&HOST_Buffer,1);
          UartHandle->protocolState = DEVICE_MAC_INIT;
        } 
        break;
        
      case DEVICE_MAC_MID:
        UartHandle->mid = temp_input;
        
        if(check_mid(temp_input))
        {
          UartHandle->protocolState = DEVICE_MAC_PAYLOADLENGTH;
          
        }
        else
        {
          send_data_count = 0;
          Buffer_Send_N_Data(&HOST_Buffer,1);
          UartHandle->protocolState = DEVICE_MAC_INIT;
        }
        break;
        
      case DEVICE_MAC_PAYLOADLENGTH:
        UartHandle->payload_length = temp_input;
        if(UartHandle->payload_length > 0)
        {
          UartHandle->protocolState = DEVICE_MAC_PAYLOAD;
          
        }
        else if(UartHandle->payload_length == 0)
        {
          UartHandle->protocolState = DEVICE_MAC_CRC;
      
        }
        else
        {
          send_data_count = 0;
          Buffer_Send_N_Data(&HOST_Buffer,1);
          UartHandle->protocolState = DEVICE_MAC_INIT;
        }
      break;
        
      case DEVICE_MAC_PAYLOAD:
          UartHandle->payload_counter++;
          if(UartHandle->payload_counter == UartHandle->payload_length)
          {
            UartHandle->protocolState = DEVICE_MAC_CRC;
         
          }
          else if(UartHandle->payload_counter > UartHandle->payload_length)
          {
            UartHandle->payload_counter = 0;
            UartHandle->protocolState = DEVICE_MAC_INIT;
          }
      break;

      case DEVICE_MAC_CRC:
         UartHandle->crc = temp_input;
       
         UartHandle->protocolState = DEVICE_MAC_FOOTER;
      break; 
          
      case DEVICE_MAC_FOOTER:
        UartHandle->tail = temp_input;
        if( UartHandle->tail == 0x6d )
        {
          HOST_Buffer.send_count = send_data_count;
           
          if((UartHandle->cid == 0x06 ) && (UartHandle->mid == 0x81 )) 
          {
            while(1) 
            { 
              if((*(HOST_Buffer.send_pt) == 0x41)&&(*(HOST_Buffer.send_pt+1) == 0x78)&&(*(HOST_Buffer.send_pt+2) == 0xFF))
              { 
                  DataPacketParser(&HOST_Buffer,UartHandle->payload_length+8);
                  Buffer_Send_N_Data(&HOST_Buffer,UartHandle->payload_length + 9);
                  break;
              }
              else
              {
                  HOST_Buffer.send_pt++;
									HOST_Buffer.send_pt_count++;
              }
            }
          }
          else
          {
            receiveAck = 1 ;                            //Received ack flag
            receiveCid = (UartHandle->cid & 0x0F);       //The high four bit is errorCode
            errorCode_Ack = ((UartHandle->cid &0xF0) >>4 );  
            receiveMid = UartHandle->mid ;     
//            memset(&HOST_Buffer,0,UartHandle->payload_length + 8);
            Buffer_Init(&HOST_Buffer);                  
          }
          send_data_count = 0;
          UartHandle->header_1 = 0;
          UartHandle->header_2 = 0;
          UartHandle->maddr = 0;
          UartHandle->cid = 0;
          UartHandle->mid = 0;
          UartHandle->payload_length = 0;
          UartHandle->payload_counter = 0;
          UartHandle->crc = 0;
          UartHandle->tail = 0;
          UartHandle->protocolState = DEVICE_MAC_INIT;
        }
       break;
    }
  
}
