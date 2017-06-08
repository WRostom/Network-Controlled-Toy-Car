#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
/************************* WiFi Access Point & MQTT Stuff *********************************/

#define WLAN_SSID //Wireless SSID here
#define WLAN_PASS //Wifi Password

#define MQTT_SERVER //Server hostname/ip address
#define MQTT_SERVERPORT 1883 // use 8883 for SSL
#define MQTT_USERNAME //MQTT server username (if any)
#define MQTT_KEY //MQTT server password (if any)
#define MQTT_TOPIC //MQTT TOPIC

/******************************************************************************************/

//MQTT G-Variables
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_SERVERPORT, MQTT_USERNAME, MQTT_KEY);

//MQTT Subscribe topic variable for /wrostom/dev/onoff and assign to variable
Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, MQTT_USERNAME MQTT_TOPIC);

//Motor Variables
int in1Pin = 14;
int in2Pin = 12;
int in3Pin = 5;
int in4Pin = 16;
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
char motions[] = {'F', 'L', 'X'};
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
char inByte = 0;

//Other G-variables
char mode[1];
int i = 0;
int j = 0;

//Prototype for Arduino 1.6.6
void MQTT_connect();
void choices();

void setup()
{
  //Setup motor pins as output
  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);
  pinMode(in3Pin, OUTPUT);
  pinMode(in4Pin, OUTPUT);

  Serial.begin(115200);
  delay(10); //Boot up time
  Serial.print("\nConnecting to ");
  Serial.println(WLAN_SSID);
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Subscribe to topic
  mqtt.subscribe(&onoffbutton);
  for (int i = 0; i < sizeof(motions); i++)
  {
      choices(motions[i]);
      delay(1000);
  }
}

void loop()
{
  MQTT_connect();

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000)))
  {
    if (subscription == &onoffbutton)
    {
      Serial.print(F("Got: "));
      Serial.println((char *)onoffbutton.lastread);
      char *mode2 = (char *)onoffbutton.lastread;
      mode[0] = mode2[0];
    }
  }

  if (mode[0] == 'U')
  {
    Serial.println("**********");
    Serial.print("OTA Mode: ");
    Serial.print("Connecting to server (");
    Serial.print(MQTT_SERVER);
    Serial.print(") to update\n");
    Serial.println("**********");
    ESPhttpUpdate.update(MQTT_SERVER, 80, "ADD BIN PATH HERE"); //Replace bin file location at server
  }
  else if (mode[0] == 'V')
  {
    Serial.println("**********");
    Serial.println("Boot Mode");
    Serial.println("**********");
    for (int i = 0; i < sizeof(motions); i++)
    {
      choices(motions[i]);
      delay(1000);
    }
    mode[0] = 'X';
  }
  else
  {
    //For testing motor controls
    choices(mode[0]);
  }
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect()
{
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected())
  {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0)
  { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000); // wait 5 seconds
    retries--;
    if (retries == 0)
    {
      // basically die and wait for WDT to reset me
      while (1)
        ;
    }
  }
  Serial.println("MQTT Connected!");
}

//Move forward and backwards
void moveFB(boolean direction)
{
  digitalWrite(in1Pin, !direction);
  digitalWrite(in2Pin, direction);
}

//Move left and right
void moveLR(boolean direction)
{
  digitalWrite(in3Pin, !direction);
  digitalWrite(in4Pin, direction);
}

//Reset pins
void reset()
{
  digitalWrite(in1Pin, 0);
  digitalWrite(in2Pin, 0);
  digitalWrite(in3Pin, 0);
  digitalWrite(in4Pin, 0);
}

void choices(char inBytes)
{
  switch (inBytes)
  {
  case 'F':
    moveFB(true);
    break;
  case 'B':
    moveFB(false);
    break;
  case 'R':
    moveLR(true);
    break;
  case 'L':
    moveLR(false);
    break;
  default:
    reset();
  }
}

