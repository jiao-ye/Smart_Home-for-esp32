#ifndef _ESP_Net_H
#define _ESP_Net_H


#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <DNSServer.h>
#include <esp_wifi.h>

//网页代码
#define ROOT_HTML  "<!DOCTYPE html><html><head><meta charset='utf-8''><title>WIFI</title><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head><style type=\"text/css\">.input{display: block; margin-top: 10px;}.input span{width: 100px; float: left; float: left; height: 36px; line-height: 36px;}.input input{height: 30px;width: 200px;}.btn{width: 120px; height: 35px; background-color: #000000; border:0px; color:#ffffff; margin-top:15px; margin-left:100px;}</style><body><h1 align='center'>ESP32配网</h1><p align='right'>末页书角</p><form method=\"POST\" action=\"configwifi\"><label class=\"input\"><span>WiFi 名称</span><input type=\"text\" name=\"ssid\" value=\"\"></label><label class=\"input\"><span>WiFi 密码</span> <input type=\"text\"  name=\"pass\"></label><input class=\"btn\" type=\"submit\" name=\"submit\" value=\"连接\"> <p><span> 附近 wifi:</P></form>"
#define RestPin 13
extern String wifi_ssid ;                     //暂时存储wifi账号密码
extern String wifi_pass ;  

void WiFi_Reseat();
void connectToWiFi();


#endif 