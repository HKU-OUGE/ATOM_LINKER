#ifndef SERIAL_H
#define SERIAL_H



signed char	SendUARTMessageLength(signed char nFd, const char chrMessage[],const unsigned short usLen);
signed char OpenCOMDevice(const char *Port,const int iBaudrate);
void CloseCOMDevice(signed char nFd);
signed char SetBaudrate(signed char nFd, const int iBaudrate);

signed char OpenCom(const char *Port, const int iBaudrate);
void CloseSerialPort(signed char nFd);


#endif
