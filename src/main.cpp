#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <EEPROM.h>
#include "WiFiChecker.h"
#include "AWSIoTConnect.h"
#include "ArduinoJson.h"
#include "Ultrasonic.h"

#define TRIG_PIN_1 5
#define ECHO_PIN_1 4

#define TRIG_PIN_2 12
#define ECHO_PIN_2 13
#define DoorOpenLength 10
#define WaitForNotSendMsg 1800
#define NoOfSecondsInMin 60

Ultrasonic door1Sensor(TRIG_PIN_1, ECHO_PIN_1, 17760);
Ultrasonic door2Sensor(TRIG_PIN_2, ECHO_PIN_2, 17760);

String convertToJSON(int, int);
void PerformTasks(long);
void BlinkLED();

long lastMsg = 0;
int firstSensor = 0;
int secondSensor = 0;
bool BlinkFlag = false;
bool LEDStatus = false;
bool justRebooted = true;

//JSON object
StaticJsonDocument<200> doc;

bool CheckIfBothDoorsAreClosed(int firstSensor, int secondSensor)
{
  if (firstSensor < DoorOpenLength || secondSensor < DoorOpenLength)
  {
    Serial.println ("Check Doors return false");
    return false;
  }
  Serial.println ("Check Doors return true");
  return true;
}
bool CheckIfTimeToSendClosedMsg(long checkTime)
{
  //Send every half an hour 
  Serial.println ("Check time to send : ");
  Serial.println (checkTime % WaitForNotSendMsg);
  return (checkTime % WaitForNotSendMsg < NoOfSecondsInMin);
}

void setup()
{
  Serial.begin(115200); //Initialising if(DEBUG)Serial Monitor
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  DefaultWifiConnect();
  SetupTimeClient();

  Serial.println("Reading cetificates");
  HandleCertificates();

  //Set up Ultrasonic sensors.

  //Set Last message time to current time.
  lastMsg = setCurrentTime();
  digitalWrite(LED_BUILTIN, HIGH);
  justRebooted= true;
}

void loop()
{
  long now = setCurrentTime();
  //Check for sensor 1
  //int distInCm = hcsr04first.distanceInMillimeters();
  PerformTasks(now);

  if (now - lastMsg >= 60)
  {
    
    lastMsg = now;
    // WifiConnectionCheckLoop();
    String msg = convertToJSON(firstSensor, secondSensor);
    if (justRebooted)
    {
      sendMessageToAWS(msg);
      Serial.println("Sent first message after Reboot");
      justRebooted = false;
    }

    if (CheckIfBothDoorsAreClosed(firstSensor, secondSensor))
    {
      // Serial.println ("Inside CheckIfBothDoorsAreClosed");
      if (CheckIfTimeToSendClosedMsg(now))
      {
        Serial.print("Sending the following to AWS (Closed Door): ");
        Serial.println(msg);
        sendMessageToAWS(msg);
      }
    }
    else
    {
      Serial.print("Sending the following to AWS: ");
      Serial.println(msg);
      sendMessageToAWS(msg);
    }

    firstSensor = 0;
    secondSensor = 0;
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
void PerformTasks(long now)
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
      Serial.print("secondSensor :");
      Serial.println(secondSensor);
    }
  }
  if (!(now % 5))
  {
    if (BlinkFlag == false)
    {
      BlinkFlag = true;
      BlinkLED();
    }
  }
  else
  {
    BlinkFlag = false;
  }
}

void BlinkLED()
{
  Serial.println("Blinking");
  if (LEDStatus == true)
  {
    LEDStatus = false;
  }
  else
  {
    LEDStatus = true;
  }
  digitalWrite(LED_BUILTIN, LEDStatus);
}
