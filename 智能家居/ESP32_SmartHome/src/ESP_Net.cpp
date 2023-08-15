#include "ESP_Net.h"


const byte DNS_PORT = 53;                  //设置DNS端口号    
 
const char* ssid="ESP32"; //wifi名称
const char* pswd="12345678";//wifi密码
//Ap
IPAddress local_IP(192, 168, 4, 1); //IP地址
IPAddress gateway(192, 168, 4, 1);  //网关地址
IPAddress subnet(255, 255, 255, 0); //子网掩码
 
//STA
IPAddress staticIP(192, 168, 0, 104);
IPAddress gateway_STA(192, 168, 0, 1);
IPAddress subnet_STA(255, 255, 255, 0);
IPAddress dns(192, 168, 0,1);

String scanNetworksID = "";                //用于储存扫描到的WiFi ID
 
String wifi_ssid = "";                     //暂时存储wifi账号密码
String wifi_pass = "";                     //暂时存储wifi账号密码

DNSServer dnsServer;                       //创建dnsServer实例
WebServer server(80);                 //开启web服务, 创建TCP SERVER,参数: 端口号,最大连接数




//处理网站访问根目录请求
void handleRoot() 
{
  if (server.hasArg("selectSSID")) {//是否有selectSSID的参数
    server.send(200, "text/html", ROOT_HTML + scanNetworksID + "</body></html>");   //scanNetWprksID是扫描到的wifi
  } else {
    server.send(200, "text/html", ROOT_HTML + scanNetworksID + "</body></html>");   
  }

}



//处理404
void handleNotFound()           // 当浏览器请求的网络资源无法在服务器找到时通过此自定义函数处理
{           
  handleRoot();                 //访问不存在目录则返回配置页面

}


void WiFi_APInit()
{
  WiFi.mode(WIFI_AP);//设置wifi的模式为ap模式
  WiFi.softAPConfig(local_IP, gateway, subnet);//设置wifi信息
  WiFi.softAP(ssid,pswd);//开启热点
   if (WiFi.softAP(ssid,pswd))   //如果AP热点开启成功                       
  {                           
    //打印相关信息
    Serial.println("ESP32 热点已经开启。");
    Serial.print("热点 address = ");
    Serial.println(WiFi.softAPIP());                                                //接入点ip
    Serial.println(String("MAC address = ")  + WiFi.softAPmacAddress().c_str());    //接入点mac
  } 
  else                                                  //开启AP热点失败
  { 
    Serial.println("热点开启失败");
    delay(1000);
    Serial.println("正在重启");
    ESP.restart();                                      //重启复位esp32
  }
}

//初始化DNS
void DNS_Init() 
{
  if (dnsServer.start(DNS_PORT, "*", local_IP))   //判断将所有地址映射到esp32的ip上是否成功
  {
    Serial.println("地址映射成功");
  } else {
    Serial.println("地址映射失败");
  }
}




//提交数据后显示的界面

void handleConfigWifi()               //返回http状态
{
  if (server.hasArg("ssid"))          //判断是否有账号参数
  {
    Serial.print("获取名称:");
    wifi_ssid = server.arg("ssid");   //获取html表单输入框name名为"ssid"的内容
    Serial.println(wifi_ssid);
  } 
  else                                //没有参数
  { 
    Serial.println("错误, 没有找到WiFi");
    server.send(200, "text/html", "<meta charset='UTF-8'>error, not found ssid"); //返回错误页面
    return;
  }
  //密码与账号同理
  if (server.hasArg("pass")) 
  {
    Serial.print("获取密码:");
    wifi_pass = server.arg("pass");  //获取html表单输入框name名为"pass"的内容
    Serial.println(wifi_pass);
  } 
  else 
  {
    Serial.println("错误, 没有找到密码");
    server.send(200, "text/html", "<meta charset='UTF-8'>error, not found password");
    return;
  }
  server.send(200, "text/html", "<meta charset='UTF-8'>SSID:" + wifi_ssid + "<br />password:" + wifi_pass + "<br />已取得WiFi信息,正在尝试连接,请手动关闭此页面。"); //返回保存成功页面
  delay(2000);
  WiFi.softAPdisconnect(true);     //参数设置为true，设备将直接关闭接入点模式，即关闭设备所建立的WiFi网络。
  server.close();                  //关闭web服务
  WiFi.softAPdisconnect();         //在不输入参数的情况下调用该函数,将关闭接入点模式,并将当前配置的AP热点网络名和密码设置为空值.
  Serial.println("WiFi 连接 名称:" + wifi_ssid + "  密码:" + wifi_pass);
 
  if (WiFi.status() != WL_CONNECTED)    //wifi没有连接成功
  {
    Serial.println("开始调用连接函数connectToWiFi()..");
    connectToWiFi();
  } 
  else {
    Serial.println("提交的配置信息自动连接成功..");
  }
}
 
//初始化Web 
void WebServer_Init() 
{
  if (MDNS.begin("esp32"))      //给设备设定域名esp32,完整的域名是esp32.local
  {
    Serial.println("MDNS responder started");
  }
  //必须添加第二个参数HTTP_GET，以下面这种格式去写，否则无法强制门户
  server.on("/", HTTP_GET, handleRoot);                      //  当浏览器请求服务器根目录(网站首页)时调用自定义函数handleRoot处理，设置主页回调函数，必须添加第二个参数HTTP_GET，否则无法强制门户
  server.on("/configwifi", HTTP_POST, handleConfigWifi);     //  当浏览器请求服务器/configwifi(表单字段)目录时调用自定义函数handleConfigWifi处理
                                                            
  server.onNotFound(handleNotFound);                         //当浏览器请求的网络资源无法在服务器找到时调用自定义函数handleNotFound处理
 
  server.begin();                                           //启动TCP SERVER
 
  Serial.println("WebServer started!");
}


bool WiFi_Scan() {
  Serial.println("扫描开始");
  Serial.println("--------->");
  // 扫描附近WiFi
  int n = WiFi.scanNetworks();
  Serial.println("扫描结束");
  if (n == 0) {
    Serial.println("没有找到");
    scanNetworksID = "没有找到";
    return false;
  } else {
    Serial.print(n);
    Serial.println(" 找到WiFi");
    for (int i = 0; i < n; ++i) {
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
      scanNetworksID += "<P>" + WiFi.SSID(i) + "</P>";
      delay(10);
    }
    return true;
  }
}


//配网
void wifiConfig() 
{
  WiFi_APInit();  
  DNS_Init();        
  WebServer_Init();  
  WiFi_Scan();       
}


//连接WiFi
void connectToWiFi() {
  WiFi.hostname("ESP32");             //设置设备名
  Serial.println("进入connectToWiFi()函数");
  WiFi.config(staticIP, gateway_STA, subnet_STA, dns, dns);
  WiFi.mode(WIFI_STA);                        //设置为STA模式并连接WIFI
  WiFi.setAutoConnect(true);                  //设置自动连接    
  
  if (wifi_ssid != "")                        //wifi_ssid不为空，意味着从网页读取到wifi
  {
    Serial.println("用web配置信息连接.");
    WiFi.begin(wifi_ssid.c_str(), wifi_pass.c_str()); //c_str(),获取该字符串的指针
    wifi_ssid = "";
    wifi_pass = "";
  } 
  else                                        //未从网页读取到wifi
  {
    Serial.println("用nvs保存的信息连接.");
    WiFi.begin();                             //begin()不传入参数，默认连接上一次连接成功的wifi
  }
 
  int Connect_time = 0;                       //用于连接计时，如果长时间连接不成功，复位设备
  while (WiFi.status() != WL_CONNECTED)       //等待WIFI连接成功
  {  
    Serial.print(".");                        //一共打印30个点点    
    delay(500);
    Connect_time ++;
                                       
    if (Connect_time > 2 * 15)         //长时间连接不上，重新进入配网页面
    { 

      Serial.println("");                     //主要目的是为了换行符
      Serial.println("启动配网模式");
      wifiConfig();                           //开始配网功能
      return;                                 //跳出 防止无限初始化
    }
  }
  
  if (WiFi.status() == WL_CONNECTED)          //如果连接成功
  {
    Serial.println("WIFI连接成功");
    Serial.printf("名称:%s", WiFi.SSID().c_str());
    Serial.printf(", 密码:%s\r\n", WiFi.psk().c_str());
    Serial.print("LocalIP:");
    Serial.print(WiFi.localIP());
    Serial.print(" ,GateIP:");
    Serial.println(WiFi.gatewayIP());
    Serial.print("WIFI status is:");
    Serial.print(WiFi.status());
    server.stop();                            //停止开发板所建立的网络服务器。
  }
}




//删除配网信息
void restoreWiFi() {
  delay(500);
  esp_wifi_restore();  //删除保存的wifi信息
  Serial.println("连接信息已清空,准备重启设备..");
  delay(10);

}

//检查WiFi是否连接
void Connect_Cheak(bool reConnect) 
{
  if (WiFi.status() != WL_CONNECTED)           //wifi连接失败
  {
    if (reConnect == true && WiFi.getMode() != WIFI_AP && WiFi.getMode() != WIFI_AP_STA ) 
    {
      Serial.println("WIFI未连接.");
      Serial.println("WiFi 模式:");
      Serial.println(WiFi.getMode());
      Serial.println("正在连接WiFi...");
      connectToWiFi();          //连接wifi函数 
    }
  } 
}


/*
 * 检测客户端DNS&HTTP请求
 */
void checkDNS_HTTP()
{
  dnsServer.processNextRequest();   //检查客户端DNS请求
  server.handleClient();            //检查客户端(浏览器)http请求
}



//如果RestPin接地5秒则重置已经保存的WiFi信息
void WiFi_Reseat()
{
     if (digitalRead(RestPin)==0) //5秒io13接地清除网络配置信息
  {
    delay(5000);             
    if (!digitalRead(RestPin)==0) 
    {
      Serial.println("\n正在清空网络连保存接信息.");
      restoreWiFi();     //删除保存的wifi信息
      ESP.restart();              //重启复位esp32
      Serial.println("已重启设备.");
    }
  }
    checkDNS_HTTP();                  //检测客户端DNS&HTTP请求
    Connect_Cheak(true);               //检测网络连接状态，参数true表示如果断开重新连接
 
}

