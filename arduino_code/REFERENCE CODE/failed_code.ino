//Title: Adafruit_TCS34725.h
//Author: Kevin (KTOWN) Townsend for Adafruit Industries
//Date: 2013
//Availability: https://github.com/adafruit/Adafruit_TCS34725 

//Title: ESP8266Wifi.h
//Author: Ivan Grokhotkov 
//Date: 2014
//Availability: https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/ESP8266WiFi.h 

/* Pin 1 is PB
Pin 3,4,5,6,7... etc are LEDs. */

#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

const char* ssid = "MySpectrumWiFi50-5G"; 
const char* password = "vastbike697";

const char* host = "maker.ifttt.com";
const int httpsPort = 443;
const int API_TIMEOUT = 10000;

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);

void setup() 
{
  //LED Setup 
  pinMode(0, OUTPUT); //LED1 RGB Sensor
  pinMode(2, OUTPUT); //LED2 Wifi
  pinMode(14, OUTPUT); //LED3 IFTTT Host
  pinMode(12 OUTPUT); //LED4 Processing
  //TEST
  digitalWrite(0, HIGH);
  digitalWrite(2, HIGH);
  digitalWrite(14, HIGH);
  digitalWrite(12, HIGH);
  delay(1500);
  digitalWrite(0, LOW);
  digitalWrite(2, LOW);
  digitalWrite(14, LOW);
  digitalWrite(12, LOW);
  
  //Input Setup
  pinMode(13, INPUT); 
  
  Serial.begin(115200);
  delay(100);
  Serial.println("");
  Serial.println("INITALIZED SERIAL CONNECTION");
  
  if (tcs.begin()) 
  {
    Serial.println("Found sensor");
    digitalWrite(0, HIGH);
  } 
  else 
  {
    Serial.println("No TCS34725 found ... check your connections");
    while (1);
  }
  Serial.print("Connecting To: ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println(".");
  }
  Serial.println("");
  digitalWrite(2, HIGH);
  Serial.println("WiFi Connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  
  
  BearSSL::WiFiClientSecure client;
  Serial.print("connecting to ");
  Serial.println(host);
  client.setInsecure();
  client.setTimeout(API_TIMEOUT);
  if (!client.connect(host, httpsPort)) 
  {
    Serial.println("connection failed");
    return;
  }
  
  digitalWrite(14, HIGH);
}

void loop() 
{
  uint16_t colorTemp;
  BearSSL::WiFiClientSecure client;
  digitalWrite(12, HIGH);
  while (digitalRead(HIGH))
  {
    digitalWrite(12, LOW);
    Serial.println("Button Pressed, Reading Data");
    //*colorTemp = collectData();
    Serial.println("Data: " + colorTemp);
   //* Storedata(colorTemp);
    if(colorTemp > 4500 && colorTemp < 6800)
    {
      String url = "/trigger/Hydrated/with/key/mj32bdgJ0-Z_EfDX6J61XMNfzAsioruCbrurkiP8iKm";
      Serial.print("requesting URL: ");
      Serial.println(url);

      client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");

      Serial.println("request sent");
      while (client.connected()) 
      {
        String line = client.readStringUntil('\n');
        if (line == "\r") 
        {
        Serial.println("headers received");
        break;
        }
      }
      String line = client.readStringUntil('\n');

      Serial.println("reply was:");
      Serial.println("==========");
      Serial.println(line);
      Serial.println("==========");
      Serial.println("closing connection");
    }
    if(colorTemp <= 4500)
    {
      String url = "/trigger/Dehydrated/with/key/mj32bdgJ0-Z_EfDX6J61XMNfzAsioruCbrurkiP8iKm";
      Serial.print("requesting URL: ");
      Serial.println(url);

      client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");

      Serial.println("request sent");
      while (client.connected()) 
      {
        String line = client.readStringUntil('\n');
        if (line == "\r") 
        {
        Serial.println("headers received");
        break;
        }
      }
      String line = client.readStringUntil('\n');

      Serial.println("reply was:");
      Serial.println("==========");
      Serial.println(line);
      Serial.println("==========");
      Serial.println("closing connection");
    }
    if(colorTemp >= 6800)
    {
      String url = "/trigger/Invalid/with/key/mj32bdgJ0-Z_EfDX6J61XMNfzAsioruCbrurkiP8iKm";
      Serial.print("requesting URL: ");
      Serial.println(url);

      client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");

      Serial.println("request sent");
      while (client.connected()) 
      {
        String line = client.readStringUntil('\n');
        if (line == "\r") 
        {
        Serial.println("headers received");
        break;
        }
      }
      String line = client.readStringUntil('\n');

      Serial.println("reply was:");
      Serial.println("==========");
      Serial.println(line);
      Serial.println("==========");
      Serial.println("closing connection");
    }
    delay(5000);
 }
 uint16_t collectData(void)
{
	uint16_t r, g, b, c, colorTemp;
	tcs.getRawData(&r, &g, &b, &c);
	colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
	return colorTemp;
}

 void conversiontoPH(void)
{
	uint16_t r, g, b, c, colorTemp;
	tcs.getRawData(&r, &g, &b, &c);
	colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
	if ((colorTemp > 0) && (colorTemp < 8000))
	{
		PH=2.7;
		int httpCode = ThingSpeak.writeField(channel1, 1, PH, apikey);

		if (httpCode == 200) 
		{
			Serial.println("Channel write successful.");
		}
		else 
		{
		Serial.println("Problem writing to channel. HTTP error code " + String(httpCode));
		}  
	stopping=1; 
	}
	if ((colorTemp < 0) || (colorTemp > 8000))
	{
		Serial.println("Value not found");
		loop(); 
	}
}