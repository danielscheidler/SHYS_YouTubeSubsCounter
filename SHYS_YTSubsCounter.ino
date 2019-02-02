#include <ArduinoJson.h> // !!! Version 5.13.4 !!!
#include "DigitLedDisplay.h"
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <YoutubeApi.h>

// --------------------------------
//  CONFIGURATION START
// --------------------------------
#define DISPLAY_DIN_PIN 16 // ESP PIN D0
#define DISPLAY_CS_PIN 4   // ESP PIN D1
#define DISPLAY_CLK_PIN 5  // ESP PIN D2

// WiFi SSID
char ssid[] = "YOUR_WIFI_SSID";
// WiFi Password
char password[] = "YOUR_WIFI_PASSWORD";

// Google-API key 
#define API_KEY "YOUR_API_KEY"

// YouTube Channel ID
#define CHANNEL_ID "YOUR_YT_CHANNEL_ID"

// Delay between Counter-Update-Requests
unsigned long requestInterval = 60000;
// --------------------------------
//  CONFIGURATION END
// --------------------------------


unsigned long lastRequest;

int subsCount; 
int viewCount; 
int commentCount; 
int videoCount; 

WiFiClientSecure client;
YoutubeApi api(API_KEY, client);

DigitLedDisplay ld =  DigitLedDisplay(DISPLAY_DIN_PIN, DISPLAY_CLK_PIN, DISPLAY_CS_PIN);

void setup() {
  ld.setBright(10);
  ld.setDigitLimit(8);

  Serial.begin(115200);
  reconnectWiFi();
}

void loop() {
  if (millis() > lastRequest + requestInterval)  {
      // refresh Values from YouTube API
      refreshSubscriber();

      // Show new Subscriber Count on Display
      ld.clear();
      ld.printDigit(subsCount);
      delay(100);
      // remember last refresh time
      lastRequest = millis();
  }
}

/**
 * reconnect to WiFi
 */
void reconnectWiFi(){
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  boolean zeroVisible = false;
  while (WiFi.status() != WL_CONNECTED) {
    ld.clear();
    if(zeroVisible){
      ld.printDigit(0);
    }
    zeroVisible = !zeroVisible;
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

  ld.clear();
  ld.printDigit(ip[0]);
  delay(1000);

  ld.clear();
  ld.printDigit(ip[1]);
  delay(1000);

  ld.clear();
  ld.printDigit(ip[2]);
  delay(1000);

  ld.clear();
  ld.printDigit(ip[3]);
  delay(1000);

  ld.clear();
}


/**
 * get current values from YouTube API into local variables
 */
void refreshSubscriber(){
  if(api.getChannelStatistics(CHANNEL_ID)) {
      commentCount = api.channelStats.commentCount;
      subsCount = api.channelStats.subscriberCount;
      videoCount = api.channelStats.videoCount;
      viewCount = api.channelStats.viewCount;

      Serial.print("Comments: ");
      Serial.println(commentCount);
      Serial.print("Subscriber: ");
      Serial.println(subsCount);
      Serial.print("Videos: ");
      Serial.println(videoCount);
      Serial.print("Views: ");
      Serial.println(viewCount);
   }
}
