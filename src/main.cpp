#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <EEPROM.h>
#include "WifiChecker.h"
#include <hcsr04.h>

#define TRIG_PIN 5
#define ECHO_PIN 4

HCSR04 hcsr04(TRIG_PIN, ECHO_PIN, 20, 4000);

void setup()
{
  DefaultWifiSetup();
}

void loop() {
  WifiConnectionCheckLoop();
  Serial.println(hcsr04.ToString());

  delay(250);
}


