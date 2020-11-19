#include <UniversalTelegramBot.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

// Wifi defenitions
const char* ssid = "TrollAthenaeum";
const char* password = "PendeL100kg";

//Telegram config
#define BOTtoken ""
#define CHAT_ID ""

WiFiClientSecure net_ssl; 
UniversalTelegramBot bot(BOTtoken, net_ssl);

const int AirValue = 857;
const int WaterValue = 440;
int intervals = (AirValue - WaterValue) / 3;

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(2000);

  //wait for serial to init
  while(!Serial) {}

  net_ssl.setInsecure();

  //init sensor
  pinMode(A0, INPUT);

  connectToWiFi();

  //bot.sendMessage(CHAT_ID, "Bot is starting", "");

  sendToTelegram();

  Serial.println("Going to sleep");

  ESP.deepSleep(3600e6);

}

void loop() {
 
}

void sendToTelegram() {
  String sensorData = getReadings();
  
  if(sensorData.equals("Dry")) {
    bot.sendMessage(CHAT_ID, "Sensor is " + sensorData + ", watering your plant!", "");
    Serial.println(sensorData);
  }
}

void connectToWiFi() {

  //set WiFi to station mode and disconnect from AP if 
  // it was previously connected
   WiFi.mode(WIFI_STA);
   WiFi.disconnect();
   delay(100);

   // attempt to connect to WIFI network
   Serial.print("Connected WiFi: ");
   Serial.println(ssid);
   WiFi.begin(ssid, password);
   while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
   }
   Serial.println("");
   Serial.println("WiFi connected");
   Serial.println("IP Address: ");
   // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP()); 
 
   }

String getReadings() {
  int soilMoistureValue = analogRead(A0);
  String condition = "";

  if(soilMoistureValue > WaterValue && 
  soilMoistureValue < (WaterValue + intervals)) {
    condition = "Very Wet";
  } else if(soilMoistureValue > (WaterValue + intervals) &&
  soilMoistureValue < (AirValue - intervals)) {
    condition = "Wet"; 
  } else if(soilMoistureValue < AirValue && 
  soilMoistureValue > (AirValue - intervals)) {
    condition = "Dry";
  }

  return condition;
}



    
   
