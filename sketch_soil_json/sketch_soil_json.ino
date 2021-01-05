#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

#define led 13

// Wifi defenitions
const char* ssid = "";
const char* password = "";

const int AirValue = 857;
const int WaterValue = 440;
int intervals = (AirValue - WaterValue) / 3;

int soilMoistureValue = 0;
String condition = "";

WiFiServer server(80);

WiFiClient client;



void setup() {
 Serial.begin(115200);
  delay(1000);
  pinMode(A0, INPUT);
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);
  
  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }

  Serial.println("");
  Serial.println("WiFi connected");

  //start server
  server.begin();
  Serial.println("Server started");

  //print the IP address
  Serial.println(WiFi.localIP());
 

 }

void loop() {

   //check of client has connected
  client = server.available();
  if(!client){
    return;
  }
  
  soilMoistureValue = analogRead(A0);

  readSoilSensor();

  //read the request line
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  //match request
  if(request.indexOf("/ledOn") != -1) { //led on
    digitalWrite(led, HIGH);
  
  } else if(request.indexOf("/ledOff") != -1) { //led off
    digitalWrite(led, LOW);
   
  }
  client.flush();
StaticJsonDocument<300> doc;

  doc["sensor"] = soilMoistureValue;
  doc["condition"] = condition;

  serializeJson(doc, Serial);

  Serial.println();

  serializeJsonPretty(doc, Serial);

  // Write response headers
  client.println(F("HTTP/1.0 200 OK"));
  client.println(F("Content-Type: application/json"));
  client.println(F("Connection: close"));
  client.print(F("Content-Length: "));
  client.println(measureJsonPretty(doc));
  client.println();

  // Write JSON document
  serializeJsonPretty(doc, client);

  // Disconnect
  client.stop();
 }



  

 

  void readSoilSensor() {

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
  }
