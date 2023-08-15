#ifndef _DHT22_H_
#define _DHT22_H_


#include <Arduino.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef int16_t s16;
typedef int32_t s32;


void DHT22Init(void);
uint8_t ReadDHT22(int16_t*tem,uint16_t*Hum);
void start(void);
void stop(void);
u8 respons(void);
void SendACK(u8 ack);
void write_byte(u8 date);
u8 read_byte(void);
u8 SHT3x_CheckCrc(u8 data[],u8 nbrOfBytes);
void ResetMD(void);
u16 Read_Rg(u8 AddRg);

#endif 