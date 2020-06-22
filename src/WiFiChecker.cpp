#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

#define ConfigPortalTimeout 300

void DefaultWifiConnect()
{
  Serial.println();
  Serial.println("Setting up WiFi");

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //reset saved settings
  //wifiManager.resetSettings();

  //set custom ip for portal
  //wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

  //fetches ssid and pass from eeprom and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  wifiManager.setConfigPortalTimeout(ConfigPortalTimeout);
  wifiManager.autoConnect();
  //or use this for auto generated name ESP + ChipID
  //wifiManager.autoConnect();

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
  Serial.println("SSID");
  Serial.println(WiFi.SSID());
}

void WifiConnectionCheckLoop()
{
  if (WiFi.isConnected())
  {
    return;
  }
  else
  {
    Serial.println("Trying to reconnect");
    ESP.reset();
  }
}