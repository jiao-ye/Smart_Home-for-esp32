#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>   

const char* ssid = "TP-LINK_CDBD";
const char* password = "ouyang888888??";

ESP8266WebServer server(80);    // 建立网络服务器对象，该对象用于响应HTTP请求。监听端口（80）
 
IPAddress local_IP(192, 168, 0, 123); // 设置ESP8266-NodeMCU联网后的IP
IPAddress gateway(192, 168, 0, 1);    // 设置网关IP（通常网关IP是WiFI路由IP）
IPAddress subnet(255, 255, 255, 0);   // 设置子网掩码
IPAddress dns(192,168,0,1);           // 设置局域网DNS的IP（通常局域网DNS的IP是WiFI路由IP）
 
float floatValue;
int intValue;
int buttonValue;
void handleUpdate(){

   intValue = server.arg("int").toInt();        // 获取客户端发送HTTP信息中的整数数值

  
  server.send(200, "text/plain", "Received");                 // 发送http响应
 
  // 通过串口监视器输出获取到的变量数值

  Serial.print("intValue = ");  Serial.println(intValue); 

  Serial.println("=================");    
}

void setup(void){
  Serial.begin(115200);          // 启动串口通讯
  Serial.println("");
  pinMode(2,OUTPUT);
  pinMode(0,OUTPUT);
  // 设置开发板网络环境
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("Failed to Config ESP8266 IP"); 
  } 

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("等待连接wifi");
    delay(500);
  }
  // WiFi连接成功后将通过串口监视器输出连接成功信息 
  Serial.println('\n');
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());              // 通过串口监视器输出连接的WiFi名称
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());           // 通过串口监视器输出ESP8266-NodeMCU的IP
 
  server.on("/update", handleUpdate);        // 处理服务器更新函数
  
  server.begin();                            // 启动网站服务
  Serial.println("HTTP server started");
}
 
void loop(void){
  server.handleClient();                    // 检查http服务器访问
  if (intValue == 1)
  {
    digitalWrite(0,HIGH);
    digitalWrite(2,HIGH);
  }
  else  if (intValue == 2)
  {
    digitalWrite(0,LOW);
    digitalWrite(2,LOW);
  }
  

}
 

