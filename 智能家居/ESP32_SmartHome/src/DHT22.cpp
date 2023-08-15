#include "DHT22.h"
#define SDA 21
#define SCL 22
#define V4_I2C_ADDR 0x44


static uint16_t HumA,HumB;

int SDA_Flag;
#define ACK 0
#define NACK 1
static u8 ACK_ret;

const u16 POLYNOMIAL=0x131;//P(x)=x^8+x^5+x^4+1=100110001

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


void HDCSDA_SET(){
  digitalWrite(SDA,HIGH);
}
void HDCSCL_SET(){
  digitalWrite(SCL,HIGH);
}
void HDCSDA_CLR(){
  digitalWrite(SDA,LOW);
}
void HDCSCL_CLR(){
  digitalWrite(SCL,LOW);
}
int HDCGet_SDA(){
  SDA_Flag= digitalRead(SDA);
  return SDA_Flag;
}
void HDCSDA_Output(){
  pinMode(SDA,OUTPUT);
}
void HDCSDA_Input(){
  pinMode(SDA,INPUT);
}
void DHT22Init(void)
{
  pinMode(SDA,OUTPUT);
  pinMode(SCL,OUTPUT);
  ResetMD();
  delay(10);//10us或者不用
  HumA=Read_Rg(8);
  HumA=(HumA<<8)|Read_Rg(9);
  HumB=Read_Rg(10);
  HumB=(HumB<<8)|Read_Rg(11);
}

//单次触发温湿度测量
uint8_t ReadDHT22(int16_t*tem,uint16_t*Hum)
{
  static u8 initFlag=0;
  u8 i,errRe,ReadDatSH[6],CalCRC[3],errorflag;
  s16 TemBuf;
  s32 CapBuf;//s32
  start();
  write_byte(V4_I2C_ADDR<<1);//Add+W
  delay(1);
  respons();
  write_byte(0x2c);//单字节读取寄存器指令D2xx
  respons();
  write_byte(0x10);
  respons();
  stop();
  delay(5);//2msSCL空闲最小1ms
  for(i=0;i<50;i++)//查询5次看测完结果
  {
  delay(1);//1ms
  delay(1);//1ms
  delay(1);//1ms
  start();
  write_byte(V4_I2C_ADDR<<1|0x01);//Add+R
  errRe=respons();
  if(errRe==0)
  {  
    break;//测量完成
  }

  else if(errRe == 1)
    stop();
  }
if(errRe==0)
{
    ReadDatSH[0]=read_byte();
    SendACK(ACK);
    ReadDatSH[1]=read_byte();
    SendACK(ACK);
    ReadDatSH[2]=read_byte();
    SendACK(ACK);
    ReadDatSH[3]=read_byte();
    SendACK(ACK);
    ReadDatSH[4]=read_byte();
    SendACK(ACK);
    ReadDatSH[5]=read_byte();
    SendACK(NACK);
    stop();
}
CalCRC[0]=ReadDatSH[0];
CalCRC[1]=ReadDatSH[1];
CalCRC[2]=ReadDatSH[2];
errorflag=SHT3x_CheckCrc(CalCRC,2);
if(errorflag==0)
{
  TemBuf=(u16)ReadDatSH[0]<<8|(ReadDatSH[1]);
  TemBuf=400+TemBuf/25.6;//*10结果*10倍286即28.6℃
  *tem=TemBuf;
}
CalCRC[0]=ReadDatSH[3];
CalCRC[1]=ReadDatSH[4];
CalCRC[2]=ReadDatSH[5];
errorflag|=SHT3x_CheckCrc(CalCRC,2);
if(errorflag==0)
{
CapBuf=(u16)ReadDatSH[3]<<8|(ReadDatSH[4]);
CapBuf=(CapBuf-HumB)*600/(HumA-HumB)+300;
//20℃为5个湿度点即1℃为0.25个湿度点0.1℃为0.025
CapBuf=CapBuf+25*(TemBuf-250)/100;
if(CapBuf>1000)
CapBuf=999;
else if(CapBuf<0)
  CapBuf=0;
*Hum=(u16)CapBuf;//同样结果*10
}
return errorflag;
}

void start(void){
  HDCSDA_SET();
  delay(10);//IIC时钟频率可以0~100Hz
  HDCSCL_SET();
  delay(10);
  HDCSDA_CLR();
  delay(10);
  HDCSCL_CLR();
}
void stop(void){
  HDCSDA_CLR();
  delay(10);
  HDCSCL_SET();
  delay(10);
  HDCSDA_SET();
  delay(10);
}

u8 respons(void)
{
  u8 i=0,error=0;//写完后紧跟着WaitACK，所有可以改变SDA
  HDCSDA_Input();
  delay(10);
  HDCSCL_SET();
  delay(10);
  ACK_ret=0;
  while((HDCGet_SDA()!=0)&&i<50)i++;
  if(i>45)
  {
  error=1;
  ACK_ret=1;
  }
  HDCSCL_CLR();//SCK:0可以改变SDA
  delay(10);
  HDCSDA_Output();
  return error;
}
void SendACK(u8 ack)
{
  if(ack==0)
  {
  HDCSDA_CLR();
  }
  else
  {
  HDCSDA_SET();
  }
  delay(10);
  HDCSCL_SET();
  delay(10);
  HDCSCL_CLR();
  delay(10);
  HDCSDA_SET();
}
void write_byte(u8 date){
  u8 i,temp;
  temp=date;
  for(i=0;i<8;i++)
  {
  HDCSCL_CLR();
  //delay(10);
  if((temp&0x80)==0x80)
  {HDCSDA_SET();}else{HDCSDA_CLR();}
  delay(10);
  HDCSCL_SET();
  delay(10);
  temp=temp<<1;
  }
  HDCSCL_CLR();
  delay(10);
  HDCSDA_SET();
  delay(10);
}
u8 read_byte(void){
  u8 i,k;
  HDCSDA_SET();//让P口准备读数
  HDCSDA_Input();
  for(i=0;i<8;i++)
  {
  HDCSCL_SET();
  delay(10);
  k<<=1;
  if(HDCGet_SDA())//==0x04
  k|=0x01;
  HDCSCL_CLR();//SCK:0可以改变SDA
  delay(10);
  }
  HDCSDA_Output();//
  return k;
}

//==============================================================================
u8 SHT3x_CheckCrc(u8 data[],u8 nbrOfBytes)//跟SHT30一样只是将CRC值到会到data后返

//==============================================================================
{
  u8 crc=0xff;//0
  u8 byteCtr,bit;
  //calculates8-Bitchecksumwithgivenpolynomial
  for(byteCtr=0;byteCtr<nbrOfBytes;++byteCtr)
  {crc^=(data[byteCtr]);
  for(bit=8;bit>0;--bit)
  {if(crc&0x80)crc=(crc<<1)^POLYNOMIAL;
  else crc=(crc<<1);
  }
  }
  if(crc!=data[nbrOfBytes])
  {
  data[nbrOfBytes]=crc;
  return 0x01;
  }
  else return 0;
}

//软复位
void ResetMD(void)
{
  start();
  write_byte(V4_I2C_ADDR<<1);//Add+W
  ets_delay_us(1);
  respons();
  write_byte(0x30);
  respons();
  write_byte(0xA2);
  respons();
  stop();
}
//读取寄存器数据
u16 Read_Rg(u8 AddRg)
{
  u8 errRe,ReadDatSH[3];
  u16 ReDat;
  start();
  write_byte(V4_I2C_ADDR<<1);//Add+W
  ets_delay_us(1);
  respons();
  write_byte(0xD2);//单字节读取寄存器指令D2xx
  respons();
  write_byte(AddRg);
  respons();
  ets_delay_us(10);
  start();
  write_byte(V4_I2C_ADDR<<1|0x01);//Add+R
  errRe=respons();
  //if(ACK_ret==0)
  //{
  ReadDatSH[0]=read_byte();
  SendACK(ACK);
  ReadDatSH[1]=read_byte();
  SendACK(ACK);
  ReadDatSH[2]=read_byte();
  SendACK(NACK);
  //}
  stop();
  errRe=SHT3x_CheckCrc(ReadDatSH,2);
  if(errRe==0)
  {
  ReDat=ReadDatSH[0];
  }else
  {
  ReDat=0xff;
  }
  return ReDat;
}
