#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <EEPROM.h>
#include "WifiChecker.h"
#include <hcsr04.h>
#include "AWSIoTConnect.h"
#include "ArduinoJson.h"

#define TRIG_PIN 5
#define ECHO_PIN 4

HCSR04 hcsr04first(TRIG_PIN, ECHO_PIN, 20, 9000);
String convertToJSON ();
long lastMsg = 0;

//JSON object
StaticJsonDocument<200> doc;


void setup()
{
  DefaultWifiSetup();
  handleCertificates();
  setupClient();
}

void loop()
{
  long now = setCurrentTime();
  if (now - lastMsg >= 60)
  {
    lastMsg = now;
    WifiConnectionCheckLoop();
    String msg = convertToJSON();
    sendMessageToAWS(msg);
  }
  //delay(1000);
}

String convertToJSON ()
{
  //Serial.println("Ultrasonic read start");
  int distInCm = hcsr04first.distanceInMillimeters();
  
  doc["Door 1"] = distInCm;
  doc["Door 2"] = 0.0;
  doc["time"] = setCurrentTime();

	// Add an array
  char buffer[200];

  // Generate the minified JSON and send it to buffer array
  //
  serializeJson (doc, buffer);
  // The above line prints:
  // {"Door 1":"value 1","Door 2":"value 2","time":1351824120}

  // Generate the prettified JSON and send it to STDOUT
  //
  serializeJsonPretty(doc, buffer);
  // The above line prints:
  // {
  //   [
  //   "Door 1": "Value 1",
  //   "Door 2": "Value 2",
  //   "time": 1351824120
  //   ]
  // }

  return buffer;
}

