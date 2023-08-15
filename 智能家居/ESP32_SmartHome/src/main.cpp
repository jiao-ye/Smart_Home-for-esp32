#include <Arduino.h>
#include "ESP_Net.h"
#include "ir_Coolix.h"
#define BLINKER_PRINT Serial
#define BLINKER_WIFI
#include "Blinker.h"
#include "ESP_Net.h"
#include "DHT22.h"
#include <WiFi.h>
#include <WiFiClient.h>

int receive;
uint8_t flag=0;

WiFiClient client1;
float clientFloatValue; //存储客户端发送的浮点型测试数据
int clientIntValue=0;     //存储客户端发送的整数型测试数据
const char* host1 = "192.168.0.123";    // 即将连接服务器网址/IP顶灯
const char* host2 = "192.168.0.124";    // 即将连接服务器网址/IP床头灯
const int httpPort = 80;               // 即将连接服务器端口

int Servial2_Read;

int client_Read;


/**************************************顶灯********************************************/
uint8_t flag1=0,b=0;//床头灯
uint8_t flag2=0,a=0;//顶灯

/******************************************IR***控制空调********************************************/
IRCoolixAC AC(12);
//空调开启
void IR_On(){
    AC.on();
    AC.setTemp(26);
}

//空调关闭
void IR_Off(){
    AC.off();
}

//空调模式选择
void IR_Mode(uint8_t mode){

    AC.setMode(mode);
    AC.setTemp(26);
}

//空调温度调节
void IR_Temp(uint8_t temp){

    AC.setTemp(temp);
}

//空调风速调节
void IR_Fan(uint8_t speed){

    AC.setFan(speed);
}

//扫风
void IR_Swing(){

    AC.setSwing();

}

//屏显
void IR_Led(){

    AC.setLed();

}

//强劲
void IR_Turb(){

    AC.setTurbo();

}



void IR_Send(){

    AC.send();
}

/********************************Blinker****手机app******************************/
char auth[] = "8b0300d703dd";//密钥
char ssid_blinker[] = "";//wifi名称
char pswd_blinker[] = "";//wifi密码


uint8_t Fan_mode=0;
uint8_t Fen=0;
int16_t temp=0;
uint16_t humi=0;
uint8_t heart_flag=0;

BlinkerButton Button1("btn-q8l");//自动
BlinkerButton Button2("btn-873");//制冷
BlinkerButton Button3("btn-mh8");//制热
BlinkerButton Button4("btn-phh");//送风
BlinkerButton Button5("btn-jpr");//开启
BlinkerButton Button6("btn-1sv");//关闭
BlinkerButton Button7("btn-k5n");//风速
BlinkerButton Button8("btn-tsl");//扫风
BlinkerButton Button9("btn-rx0");//床头灯
BlinkerButton Button10("btn-hmm");//顶灯
BlinkerButton Button11("btn-ijl");//屏显
BlinkerButton Button12("btn-gl0");//强劲
BlinkerSlider Slider1("ran-o26");//温度调节
BlinkerNumber Number1("num-9jn");//房间温度
BlinkerNumber Number2("num-sud");//房间湿度


void change(String change_value,char value[])
{
  
  for (int i = 0; i < change_value.length(); i++)
  {
    value[i] = change_value[i];
  }
  
}

// 空调自动
void Button1_back(const String & state){

    IR_Mode(kCoolixAuto);
    IR_Send();

}
//空调制冷
void Button2_back(const String & state){

    IR_Mode(kCoolixCool);
    IR_Send();

}
//空调制热
void Button3_back(const String & state){

    IR_Mode(kCoolixHeat);
    IR_Send();

}
//空调送风
void Button4_back(const String & state){

    IR_Mode(kCoolixFan);
    IR_Send();

}
//开空调
void Button5_back(const String & state){

    IR_On();
    
    IR_Send();

}
//关空调
void Button6_back(const String & state){

    IR_Off();
    IR_Send();

}
 //空调风速选择
void Button7_back(const String & state){

    if (Fan_mode == 4)
    {
        Fan_mode=0;
    }
    
    switch (Fan_mode) 
    {
        case 0:IR_Fan(4); break;
        case 1:IR_Fan(2); break;
        case 2:IR_Fan(1); break;
        case 3:IR_Fan(5); break;
    default:
        break;
    }
    Fan_mode++;
    IR_Send();

}
//扫风
void Button8_back(const String & state){
    IR_Swing();
    IR_Send();

}


//床头灯
void Button9_back(const String & state){
    b=1;
    if (flag2 == 2)
    {
        flag2=0; 
    }
    flag2++;    

}
//顶灯
void Button10_back(const String & state){
    a=1;
    if (flag1 == 2)
    {
        flag1=0; 
    }    
    flag1++;

}

//屏显
void Button11_back(const String & state){
    IR_Led();
    IR_Send();

}
//强劲
void Button12_back(const String & state){

    IR_Turb();
    IR_Send();
}

void slider1_callback(int32_t value) {
  uint8_t value_change=(uint8_t)value;
  IR_Temp(value_change);
  IR_Send();

}



void blinker_heartbeat() {
    //返回温湿度数据
    heart_flag=1;
    Number1.print(temp/10.0);
    Number2.print(humi/10.0);
}


void Blinker_Begin(){
    change(wifi_ssid,ssid_blinker);
    change(wifi_pass,pswd_blinker);
    Blinker.begin(auth,ssid_blinker,pswd_blinker);
    /*****************************组件的初始化*********************************/
    Button1.attach(Button1_back);
    Button2.attach(Button2_back);
    Button3.attach(Button3_back);
    Button4.attach(Button4_back);
    Button5.attach(Button5_back);
    Button6.attach(Button6_back);
    Button7.attach(Button7_back);
    Button8.attach(Button8_back);
    Button9.attach(Button9_back);
    Button10.attach(Button10_back);
    Button11.attach(Button11_back);
    Button12.attach(Button12_back);
    Slider1.attach(slider1_callback);
     /**********************************注册心跳包*********************************/
    Blinker.attachHeartbeat(blinker_heartbeat);
    
    
}

void Blinker_Run(){
    Blinker.run();
}

void wifiClientRequest1(){
  WiFiClient client;  
 
  // 将需要发送的数据信息放入客户端请求
  String url = "/update?float=" + String(clientFloatValue) + 
               "&int=" + String(clientIntValue) ;
               
                         
  // 建立字符串，用于HTTP请求
  String httpRequest =  String("GET ") + url + " HTTP/1.1\r\n" +
                        "Host: " + host1 + "\r\n" +
                        "Connection: close\r\n" +
                        "\r\n";
                        
  Serial.print("Connecting to "); 
  Serial.print(host1); 
  
  if (client1.connect(host1, httpPort)) {  //如果连接失败则串口输出信息告知用户然后返回loop
    Serial.println(" Sucess");
    
    client1.print(httpRequest);          // 向服务器发送HTTP请求
    Serial.println("Sending request: ");// 通过串口输出HTTP请求信息内容以便查阅
    Serial.println(httpRequest);        
  } else{
    Serial.println(" failed");
  }
  
  client1.stop();                         
}
void wifiClientRequest2(){
  WiFiClient client;  
 
  // 将需要发送的数据信息放入客户端请求
  String url = "/update?float=" + String(clientFloatValue) + 
               "&int=" + String(clientIntValue) ;
               
                         
  // 建立字符串，用于HTTP请求
  String httpRequest =  String("GET ") + url + " HTTP/1.1\r\n" +
                        "Host: " + host2 + "\r\n" +
                        "Connection: close\r\n" +
                        "\r\n";
                        
  Serial.print("Connecting to "); 
  Serial.print(host2); 
  
  if (client1.connect(host2, httpPort)) {  //如果连接失败则串口输出信息告知用户然后返回loop
    Serial.println(" Sucess");
    client1.print(httpRequest);          // 向服务器发送HTTP请求
    Serial.println("Sending request: ");// 通过串口输出HTTP请求信息内容以便查阅
    Serial.println(httpRequest);        
  } else{
    Serial.println(" failed");
  }
  client1.stop();                         
}
void setup()
{
  Serial.begin(115200);
  connectToWiFi();
  pinMode(13, INPUT_PULLUP);
  DHT22Init();
    if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("连接wifi成功");
    Serial2.begin(9600);
    Blinker_Begin();
    AC.begin(); 
  }  
}
void loop()
{
    /****************************控制灯************************************/
    Blinker_Run();
    while (Serial2.available())
      {
        Servial2_Read=Serial2.read();
        Serial.print(Servial2_Read);       
      }
    switch (Servial2_Read)
    {
      case 0x01:clientIntValue=1;wifiClientRequest1();break;//灯光控制
      case 0x02:clientIntValue=3;wifiClientRequest2();break;
      case 0x03:clientIntValue=2;wifiClientRequest1();break;
      case 0x04:clientIntValue=4;wifiClientRequest2();break;
      case 0x20: IR_On();IR_Send();break;
      case 0x21: IR_Off();IR_Send();break;
      case 0x22: IR_Mode(kCoolixAuto);IR_Send();break;
      case 0x23: IR_Mode(kCoolixCool);IR_Send();break;
      case 0x24: IR_Mode(kCoolixHeat);IR_Send();break;
      case 0x25: IR_Mode(kCoolixFan);IR_Send();break;
      case 0x26: IR_Fan(4);IR_Send();break;
      case 0x27: IR_Fan(2);IR_Send();break;
      case 0x28: IR_Fan(1);IR_Send();break;
      case 0x29: IR_Fan(5);IR_Send();break;
      case 0x2A: IR_Temp(17);IR_Send();break;
      case 0x2B: IR_Temp(18);IR_Send();break;
      case 0x2C: IR_Temp(19);IR_Send();break;
      case 0x2D: IR_Temp(20);IR_Send();break;
      case 0x2E: IR_Temp(21);IR_Send();break;
      case 0x2F: IR_Temp(22);IR_Send();break;
      case 0x30: IR_Temp(23);IR_Send();break;
      case 0x31: IR_Temp(24);IR_Send();break;
      case 0x32: IR_Temp(25);IR_Send();break;
      case 0x33: IR_Temp(26);IR_Send();break;
      case 0x34: IR_Temp(27);IR_Send();break;
      case 0x35: IR_Temp(28);IR_Send();break;
      case 0x36: IR_Temp(29);IR_Send();break;
      case 0x37: IR_Temp(30);IR_Send();break;
      case 0x38: IR_Swing();IR_Send();break;
      case 0x39: IR_Led();IR_Send();break;
      case 0x3A: IR_Turb();IR_Send();break;        
     default:
        break;
    }
    Servial2_Read=0;
          if (heart_flag == 1)
      {
        Serial.println("进入心跳包");
        ReadDHT22(&temp,&humi);
        heart_flag = 0;
      }
      Serial.println("已开始");      
      while (Serial2.available())
      {
        Servial2_Read=Serial2.read();
        Serial.print(Servial2_Read);       
      }

      if (a == 1)
      {
        if (flag1 ==1)
        {
            clientIntValue=1;
            
        }
        else if (flag1 == 2)
        {
            clientIntValue=2;
            
        }
        wifiClientRequest1();
        a=0;
      }  
      if (b == 1)
      {
        if (flag2 ==1)
        {
            clientIntValue=3;
        }
        else if (flag2 == 2)
        {
            clientIntValue=4;
        }
        wifiClientRequest2();
        b=0;
      }
    Servial2_Read=0; 
    Serial.println("接收到的数据");
    WiFi_Reseat();
    delay(100);
}




 
