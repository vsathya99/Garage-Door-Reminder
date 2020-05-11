#ifdef AWSIoTConnect
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "FS.h"
#endif

//Func Declare
void setupClient();
void callback(char *topic, byte *payload, int length);
void sendMessageToAWS(String msg);
void handleCertificates();