#include <ESP8266WiFi.h>
#include <WiFiManager.h>

WiFiManager wm;

void setup() {
  Serial.begin(115200);

  
  wm.resetSettings();//force config portal

  wm.autoConnect("NodeMCU_Config");

  Serial.println("WiFi Connected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
}
