#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h> 
#include <ArduinoJson.h>

// Wifi defenitions
const char* ssid = "";
const char* password = "";

// Initialize Telegram BOT
#define BOTtoken ""
#define CHAT_ID ""

WiFiClientSecure net_ssl; 
UniversalTelegramBot bot(BOTtoken, net_ssl);

// Checks for new messages every 1 second.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

const int AirValue = 857;
const int WaterValue = 440;
int intervals = (AirValue - WaterValue) / 3;


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


void setup() {
   Serial.begin(115200);

  #ifdef ESP8266
    net_ssl.setInsecure();
  #endif
   
  pinMode(A0, INPUT);

  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP()); 

}

void loop() {
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }   

}

// Handle what happens when you receive new messages
  void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);
    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String welcome = "Welcome, " + from_name + ".\n";
      welcome += "Use the following commands to get current readings.\n\n";
      welcome += "/readings \n";
    
      bot.sendMessage(chat_id, welcome, "");
    }

    if (text == "/readings") {
     String readings = getReadings();
     bot.sendMessage(chat_id, readings, "");
    }
  }
}
