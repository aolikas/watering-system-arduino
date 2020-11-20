#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <FirebaseArduino.h>
#include <ESP8266HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define FIREBASE_HOST ""
#define FIREBASE_AUTH ""

// Wifi defenitions
const char* ssid = "";
const char* password = "";

int soilMoistureValue;

const int AirValue = 857;
const int WaterValue = 440;
int intervals = (AirValue - WaterValue) / 3;

//UTC +1 -> 1 * 60 *60
const long utcOffsetInSeconds = 3600;

//define NTP client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);


String DB_NAME = "SensorDataWithDateTime";
String HUMIDITY = "Humidity";
String CONDITION = "Condition";

void setup() {
  Serial.begin(115200);
  
  pinMode(A0, INPUT);
  soilMoistureValue = analogRead(A0);
  
  connectToWiFi();

  sentToRealTime();

  Serial.println("Going to sleep");

  ESP.deepSleep(600e6);

}

void loop() {
}

String getDate() {

  unsigned long epochTime = timeClient.getEpochTime();
  Serial.print("Epoch Time: ");
  Serial.println(epochTime);

  //Get a time structure
  struct tm *ptm = gmtime ((time_t *)&epochTime); 

  int monthDay = ptm->tm_mday;
  Serial.print("Month day: ");
  Serial.println(monthDay);

  int currentMonth = ptm->tm_mon+1;
  Serial.print("Month: ");
  Serial.println(currentMonth);

  int currentYear = ptm->tm_year+1900;
  Serial.print("Year: ");
  Serial.println(currentYear);

  //Print complete date:
  String currentDate = String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay);
  Serial.print("Current date: ");
  Serial.println(currentDate);

  return currentDate;
}

String getTime() {

  String hr, mn, sc;
  
  if(timeClient.getHours() < 10) {
    hr = "0" + String(timeClient.getHours());
  } else {
    hr = String(timeClient.getHours());
  }

  if(timeClient.getMinutes() < 10) {
    mn = "0" + String(timeClient.getMinutes());
  } else {
    mn = String(timeClient.getMinutes());
  }

   if(timeClient.getSeconds() < 10) {
    sc = "0" + String(timeClient.getSeconds());
  } else {
    sc = String(timeClient.getSeconds());
  }

  String timeNow = hr + ":" + mn + ":" + sc;
  Serial.println(timeNow);
  return timeNow;
  }

void sentToRealTime() {

  
  timeClient.begin();

  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  
  String currentDate = getDate();
  String currentTime = getTime();
  String condition = getReadings(soilMoistureValue);
  

  Serial.print("Humidity: ");
  Serial.println(soilMoistureValue);
  Serial.print("Condition: ");
  Serial.println(condition);

  String humidityDataInt = DB_NAME + "/" + currentDate + "/" + currentTime + "/" + HUMIDITY;
  Firebase.setInt(humidityDataInt,soilMoistureValue);

  if(Firebase.failed()) {
    Serial.println("Setting Humidity failed");
    Serial.println(Firebase.error());
    delay(500);
    return;
  }

  String conditionData = DB_NAME + "/" + currentDate + "/" + currentTime + "/" + CONDITION;

   Firebase.setString(conditionData,condition);

  if(Firebase.failed()) {
    Serial.println("Setting Humidity string failed");
    Serial.println(Firebase.error());
    delay(500);
    return;
  }

  Serial.println("Setting successful");
  Serial.println();
  
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

String getReadings(int sensorData) {

  String condition = "";

  if(sensorData > WaterValue && 
  sensorData < (WaterValue + intervals)) {
    condition = "Very Wet";
  } else if(sensorData > (WaterValue + intervals) &&
  sensorData < (AirValue - intervals)) {
    condition = "Wet"; 
  } else if(sensorData < AirValue && 
  sensorData > (AirValue - intervals)) {
    condition = "Dry";
  }

  return condition;
}

  

 // ------------------Get Data from RealTime
 /*
  Serial.print("Value: ");
  Serial.println(Firebase.getString("RandomVal/Val"));

  if(Firebase.failed()) {
    Serial.println("Setting / Value is failed");
    Serial.println(Firebase.error());
    delay(500);
    return;
  }

  delay(5000);
}

*/
