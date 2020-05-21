#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <EEPROM.h>
#include "WifiChecker.h"
#include "AWSIoTConnect.h"
#include "ArduinoJson.h"
#include "Ultrasonic.h"

#define TRIG_PIN_1 5
#define ECHO_PIN_1 4

#define TRIG_PIN_2 5
#define ECHO_PIN_2 4

Ultrasonic door1Sensor(TRIG_PIN_1, ECHO_PIN_1, 17760);
Ultrasonic door2Sensor(TRIG_PIN_2, ECHO_PIN_2, 17760);


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

  //Set up Ultrasonic sensors.


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
      firstSensor = door1Sensor.Ranging(1);
      Serial.print("firstSensor ");
      Serial.println(firstSensor);
    }
  }
  if (now - lastMsg > 45 && now - lastMsg < 55)
  {
    if (secondSensor == 0)
    {
      secondSensor = door2Sensor.Ranging(1);
      Serial.print("secondSensor ");
      Serial.println(secondSensor);
    }
  }
}
