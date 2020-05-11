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
  WifiConnectionCheckLoop();
  //Serial.println(hcsr04first.ToString());
  String msg = convertToJSON();
  sendMessageToAWS(msg);
  delay(1000);
}

String convertToJSON ()
{
  //Serial.println("Ultrasonic read start");
  int distInCm = hcsr04first.distanceInMillimeters();
  //String Dist = hcsr04first.ToString();
  //Serial.println("Ultrasonic read end");
  //String Door1Distance = Dist.substring(((Dist.indexOf("Distance")) - 2), (Dist.length()) - 1);
  //String Door1Distance = Dist.substring(((Dist.indexOf("Distance")) + 10), (Dist.length()) - 2);
  //Serial.println (Door1Distance);

  doc["Door 1"] = distInCm;
  doc["Door 2"] = "TBD";
  doc["time"] = 1351824120;
	 // Add an array.
  //
  //JsonArray data = doc.createNestedArray("data");
  //data.add(48.756080);
  //data.add(Door1Distance);

  char buffer[200];

  // Generate the minified JSON and send it to STDOUT
  //
  serializeJson (doc, buffer);
  // The above line prints:
  // {"sensor":"gps","time":1351824120,"data":[48.756080,2.302038]}

  // Start a new line
  //Serial << std::endl;

  // Generate the prettified JSON and send it to STDOUT
  //
  serializeJsonPretty(doc, buffer);
  // The above line prints:
  // {
  //   "sensor": "gps",
  //   "time": 1351824120,
  //   "data": [
  //     48.756080,
  //     2.302038
  //   ]
  // }

  return buffer;
}

