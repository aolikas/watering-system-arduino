#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <FirebaseArduino.h>
#include <ESP8266HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define FIREBASE_HOST "soilmoisturetemperatureproject.firebaseio.com"
#define FIREBASE_AUTH "lK90UlpKwR0MlEc6Hqj4XFS3TcPi1IMv6yzlGkTm"

// Wifi defenitions
const char* ssid = "TrollAthenaeum";
const char* password = "PendeL100kg";

int soilMoistureValue;

const int AirValue = 857;
const int WaterValue = 440;
int intervals = (AirValue - WaterValue) / 3;

String DB_NAME = "Users";
String USER_ID = "exdxLIjgqCQVQMOBfbLTLUPkb2U2";
String SENSOR_SUB_DB = "userSensors";
String SENSOR_ID = "-MQI10eju5ibtJRjanSO";
String SENSOR_CONDITION = "userSensorMoistureCondition";

void setup() {
   Serial.begin(115200);
  
  pinMode(A0, INPUT);
  soilMoistureValue = analogRead(A0);
  
  connectToWiFi();

  sentToRealTime();

  Serial.println("Going to sleep");

  ESP.deepSleep(20e6);

}

void loop() {
  // put your main code here, to run repeatedly:

}

void sentToRealTime() {


  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  String condition = getSoilSensorReadings(soilMoistureValue);
 
  Serial.print("Humidity: ");
  Serial.println(soilMoistureValue);
  Serial.print("Condition: ");
  Serial.println(condition);

  String currentSoilConditionToDB = DB_NAME + "/" + USER_ID + "/" + SENSOR_SUB_DB + "/" + SENSOR_ID + "/" + SENSOR_CONDITION;
  
  Firebase.setString(currentSoilConditionToDB,condition);
    if(Firebase.failed()) {
    Serial.println("Setting Soil Condition failed");
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

String getSoilSensorReadings(int sensorData) {

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
