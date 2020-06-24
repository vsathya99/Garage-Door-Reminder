
#define AWSIoTConnect
#include "AWSIoTConnect.h"

const char *AWS_endpoint = "a25b0bd2aiywyd-ats.iot.us-east-2.amazonaws.com";

//Variables
WiFiClientSecure espClient;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
PubSubClient client(AWS_endpoint, 8883, callback, espClient);
long epochCurrentTime;

void SetupTimeClient()
{
    espClient.setBufferSizes(512, 512);

    timeClient.begin();
    while (!timeClient.update())
    {
        timeClient.forceUpdate();
    }
    Serial.println("Connected to timed server");
    espClient.setX509Time(timeClient.getEpochTime());
    Serial.println("Connected to AWS");
}

long setCurrentTime()
{
    return epochCurrentTime = timeClient.getEpochTime();
}

void callback(char *topic, byte *payload, int length)
{
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++)
    {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}
void sendMessageToAWS(String msg)
{
    if (!client.connected())
    {
        if (client.connect("ESPthing"))
        {
            //Serial.println("Reconnect success");
            client.subscribe("inTopic");
        }
        else
        {
            Serial.println("Failed to reconnect");
        }
    }
    if (client.connected())
    {
        //Serial.println("Sending to AWS");
        bool sendResult = client.publish("GDRTopic", msg.c_str());
        Serial.println(sendResult ? "" : "Failed to send");
    }
    client.loop();
}

void HandleCertificates()
{
    if (!SPIFFS.begin())
    {
        Serial.println("Failed to mount FS");
        return;
    }
    Serial.print("Heap: ");
    Serial.println(ESP.getFreeHeap());

    // Load certificate file
    File cert = SPIFFS.open("/cert.der", "r"); //replace cert.crt eith your uploaded file name
    if (!cert)
    {
        Serial.println("Failed to open cert file");
    }
    else
        Serial.println("Success to open cert file");

    delay(1000);

    if (espClient.loadCertificate(cert))
        Serial.println("cert loaded");
    else
        Serial.println("cert not loaded");

    // Load private key file
    File private_key = SPIFFS.open("/private.der", "r"); //replace private eith your uploaded file name
    if (!private_key)
    {
        Serial.println("Failed to open private cert file");
    }
    else
        Serial.println("Success to open private cert file");

    delay(1000);

    if (espClient.loadPrivateKey(private_key))
        Serial.println("private key loaded");
    else
        Serial.println("private key not loaded");

    // Load CA file
    File ca = SPIFFS.open("/ca.der", "r"); //replace ca eith your uploaded file name
    if (!ca)
    {
        Serial.println("Failed to open ca ");
    }
    else
        Serial.println("Success to open ca");

    delay(1000);

    if (espClient.loadCACert(ca))
        Serial.println("ca loaded");
    else
        Serial.println("ca failed");

    Serial.print("Heap: ");
    Serial.println(ESP.getFreeHeap());
}