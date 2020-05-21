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
String convertToJSON(int, int);
void ReadValues(long);
long lastMsg = 0;
int firstSensor = 0;
int secondSensor = 0;

//JSON object
StaticJsonDocument<200> doc;



void setup()
{
  Serial.begin(115200); //Initialising if(DEBUG)Serial Monitor
  pinMode(LED_BUILTIN, OUTPUT);
  DefaultWifiSetup();
  SetupTimeClient();

  Serial.println("Reading cetificates");
  HandleCertificates();

  //Set Last message time to current time.
  lastMsg = setCurrentTime();
}

void loop()
{
  long now = setCurrentTime();
  //Check for sensor 1
  //int distInCm = hcsr04first.distanceInMillimeters();
  ReadValues(now);
  
  

  if (now - lastMsg >= 60)
  {
    lastMsg = now;
    WifiConnectionCheckLoop();
    String msg = convertToJSON(firstSensor, secondSensor);
    firstSensor = 0;
    secondSensor = 0;
    Serial.print("Sending the following to AWS: ");
    Serial.println(msg);
    sendMessageToAWS(msg);
  }
  //delay(1000);
}

String convertToJSON(int firstSensor, int secondSensor)
{
  //Serial.println("Ultrasonic read start");
  //int distInCm = hcsr04first.distanceInMillimeters();

  doc["Door 1"] = firstSensor;
  doc["Door 2"] = secondSensor;
  doc["time"] = setCurrentTime();

  // Add an array
  char buffer[200];

  // Generate the minified JSON and send it to buffer array
  //
  serializeJson(doc, buffer);
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
  String returnValue = buffer;
  return returnValue;
}
void ReadValues(long now)
{
  
  if (now - lastMsg > 20 && now - lastMsg < 30)
  {
    if (firstSensor == 0)
    {
      firstSensor = hcsr04first.distanceInMillimeters();
      Serial.print("firstSensor ");
      Serial.println(firstSensor);
    }
  }
  if (now - lastMsg > 45 && now - lastMsg < 55)
  {
    if (secondSensor == 0)
    {
      secondSensor = hcsr04first.distanceInMillimeters();
      Serial.print("secondSensor ");
      Serial.println(secondSensor);
    }
  }
}
