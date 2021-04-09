//Title: ThingSpeak.h
//Author: The MathWorks, Inc 
//Date: 2018
//Availability: https://github.com/mathworks/thingspeak-arduino/blob/master/src/ThingSpeak.h  

//Title: Adafruit_TCS34725.h
//Author: Kevin (KTOWN) Townsend for Adafruit Industries
//Date: 2013
//Availability: https://github.com/adafruit/Adafruit_TCS34725 

//Title: ESP8266Wifi.h
//Author: Ivan Grokhotkov 
//Date: 2014
//Availability: https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/ESP8266WiFi.h 

//Title: RSSI_to_ThingSpeak.ino
//Author: Hans Scharler
//Date: 2019
//Availability: https://github.com/nothans/thingspeak-esp-examples/blob/master/examples/RSSI_to_ThingSpeak.ino

/* Pin 1 is PB
Pin 3,4,5,6,7... etc are LEDs. */


#include "ThingSpeak.h"
#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

const char* ssid = "degroot"; 
const char* password = "degroot143!";

#define channelid 1038676                                 
#define apikey_write "GYYOAQPPZ8HGU5LM"               

unsigned long channel1 = channelid;
const char * apikey = apikey_write;

const char* host = "maker.ifttt.com";
const int httpsPort = 443;

float PH;
int stopping=0;
WiFiClient  espclient;

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);

const int API_TIMEOUT=10000;

void setup() 
{
  //LED Setup 
  pinMode(0, OUTPUT); //LED1 RGB Sensor
  pinMode(2, OUTPUT); //LED2 Wifi
  pinMode(14, OUTPUT); //LED3 IFTTT Host
  pinMode(12, OUTPUT); //LED4 Processing
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
	uint16_t red, blue, green, clear, colorTemp;
	digitalWrite(12, HIGH);
	while (digitalRead(13)==HIGH)
	{
		digitalWrite(12, LOW);
		Serial.println("Button Pressed, Reading Data");
		red = collectDataRed();
		green = collectDataGreen();
		blue = collectDataBlue();
		clear = collectDataClear();
		colorTemp = collectDataColorTemp();
		Serial.println("Data: " + red);
		Serial.println("Data: " + green);
		Serial.println("Data: " + blue);
		Serial.println("Data: " + clear);
		Serial.println("Data: " + colorTemp);
		Storedata(colorTemp, red, green);
		if(red < 820 && blue > 700 && clear < 800 && colorTemp <= 6000)
		{
			if(colorTemp > 3200 )
			{
				Serial.println("Hydrated");
				Hydrated();
			}
			if(colorTemp <= 3200 && colorTemp > 2600)
			{
				Serial.println("Lightly DeHydrated");
				DeHydrated1();
			}
			if(colorTemp <= 2600)
			{
				Serial.println("Very Dehydrated");
				DeHydrated2();
			}
		}
		else
		{
			Serial.println("Value Not Recognized as Urine");
			ReadError();
		}
		delay(5000);
	}
	/* if(stopping ==1) 
	{
		
	}
	else 
	{
		delay(1000);
		Serial.println("Align Strip For Reading");
		delay(8000);
		Serial.println("Reading in Progress");
		conversiontoPH();
	} */
}

uint16_t collectDataColorTemp(void)
{
	uint16_t r, g, b, c, colorTemp;
	tcs.getRawData(&r, &g, &b, &c);
	colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
	return colorTemp;
}
uint16_t collectDataClear(void)
{
	uint16_t r, g, b, c;
	tcs.getRawData(&r, &g, &b, &c);
	return c;
}
uint16_t collectDataRed(void)
{
	uint16_t r, g, b, c;
	tcs.getRawData(&r, &g, &b, &c);
	return r;
}
uint16_t collectDataGreen(void)
{
	uint16_t r, g, b, c;
	tcs.getRawData(&r, &g, &b, &c);
	return g;
}
uint16_t collectDataBlue(void)
{
	uint16_t r, g, b, c;
	tcs.getRawData(&r, &g, &b, &c);
	return b;
}

void Storedata(uint16_t colorTemp, uint16_t r, uint16_t g)
{
  while(!!!client.connect(host, 80) && (5-- > 0)) {
	Serial.print(".");
  }
  String url = "/trigger/Urinalysis/with/key/mj32bdgJ0-Z_EfDX6J61XM15bkwN_5c-5Lx0USVBRoL";
  
  String jsonObject = String("{\"value1\":\"") + String((uint16_t)colorTemp) + "\",\"value2\":\"" + String((uint16_t)r) + "\",\"value3\":\"" + String((uint16_t)g) + "\"}";
  
  BearSSL::WiFiClientSecure client;	
  Serial.println("request sent");
  client.println(String("POST ") + url + " HTTP/1.1");
  client.println(String("Host: ") + host); 
  client.println("Connection: close\r\nContent-Type: application/json");
  client.print("Content-Length: ");
  client.println(jsonObject.length());
  client.println();
  client.println(jsonObject);
        
  int timeout = 10 * 10; // 10 seconds             
  while(!!!client.available() && (timeout-- > 0)){
    delay(100);
  }
  if(!!!client.available()) {
    Serial.println("No response...");
  }
  while(client.available()){
    Serial.write(client.read());
  }
}

void Hydrated()
{
	String url = "/trigger/Hydrated/with/key/mj32bdgJ0-Z_EfDX6J61XMNfzAsioruCbrurkiP8iKm";
	
	BearSSL::WiFiClientSecure client;

	client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "User-Agent: BuildFailureDetectorESP8266\r\n" + "Connection: close\r\n\r\n");
	
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
	String line = client.readStringUntil('/r');

	if (line=="")
	{
		Serial.println("reply was:");
		Serial.println(line);
		Serial.println("Success");
	}
	else
	{
		Serial.println("reply was:");
		Serial.println(line);
		Serial.println("No A Success");
	}
}
void DeHydrated1()
{
	String url = "/trigger/Dehydrated/with/key/mj32bdgJ0-Z_EfDX6J61XMNfzAsioruCbrurkiP8iKm";
	
	BearSSL::WiFiClientSecure client;

	client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "User-Agent: BuildFailureDetectorESP8266\r\n" + "Connection: close\r\n\r\n");
	
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
	String line = client.readStringUntil('/r');

	if (line=="")
	{
		Serial.println("reply was:");
		Serial.println(line);
		Serial.println("Success");
	}
	else
	{
		Serial.println("reply was:");
		Serial.println(line);
		Serial.println("No A Success");
	}
}
void DeHydrated2()
{
	String url = "/trigger/Dehydrated/with/key/mj32bdgJ0-Z_EfDX6J61XMNfzAsioruCbrurkiP8iKm";
	
	BearSSL::WiFiClientSecure client;

	client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "User-Agent: BuildFailureDetectorESP8266\r\n" + "Connection: close\r\n\r\n");
	
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
	String line = client.readStringUntil('/r');

	if (line=="")
	{
		Serial.println("reply was:");
		Serial.println(line);
		Serial.println("Success");
	}
	else
	{
		Serial.println("reply was:");
		Serial.println(line);
		Serial.println("No A Success");
	}
}
void ReadError()
{
	String url = "/trigger/Invalid/with/key/mj32bdgJ0-Z_EfDX6J61XMNfzAsioruCbrurkiP8iKm";
	
	BearSSL::WiFiClientSecure client;

	client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "User-Agent: BuildFailureDetectorESP8266\r\n" + "Connection: close\r\n\r\n");
	
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
	String line = client.readStringUntil('/r');

	if (line=="")
	{
		Serial.println("reply was:");
		Serial.println(line);
		Serial.println("Success");
	}
	else
	{
		Serial.println("reply was:");
		Serial.println(line);
		Serial.println("No A Success");
	}
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
