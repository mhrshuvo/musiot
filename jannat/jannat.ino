#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#include "DHT.h"  
#define DHTTYPE DHT11 

#define         MQ2PIN   (A0)
float           Ro = 0; 

#define         RL_VALUE_MQ2                 (1)   
#define         RO_CLEAN_AIR_FACTOR_MQ2      (9.577) 
                                                    


#define         CALIBARAION_SAMPLE_TIMES     (50)    
#define         CALIBRATION_SAMPLE_INTERVAL  (500)  
                                                     
#define         READ_SAMPLE_INTERVAL         (50)    
#define         READ_SAMPLE_TIMES            (5)   
                                                    
#define         GAS_HYDROGEN                  (0)
#define         GAS_LPG                       (1)
#define         GAS_METHANE                   (2)
#define         GAS_CARBON_MONOXIDE           (3)
#define         GAS_ALCOHOL                   (4)
#define         GAS_SMOKE                     (5)
#define         GAS_PROPANE                   (6)
#define         accuracy                      (0) 

ESP8266WebServer server(80);  

const char* ssid = "Thesis";
const char* password = "10101010";

const int DHTPin = 2;
DHT dht(DHTPin, DHTTYPE);


void handleRoot() {
  
String Value = "";
  Value += "{";
  Value += "\"TempC\":";
  Value += dht.readTemperature();

  Value += ",";
  
  Value += "\"UnitId\":\"F1 - L1\"";
  

  Value += ",";  

  Value += "\"Humidity\":";
  Value +=  dht.readHumidity();

  Value += ",";

  Value += "\"Smoke\":";
  Value +=  MQGetGasPercentage(MQRead(MQ2PIN)/Ro,GAS_SMOKE);

  Value += ",";

  Value += "\"Carbon_Monoxide\":";
  Value += MQGetGasPercentage(MQRead(MQ2PIN)/Ro,GAS_CARBON_MONOXIDE);
  Value += "}";
  

 server.send(200, "application/json", Value); //--> Send web page
}

void handleDHT11Temperature() {
  String Temperature_Value = "";
  Temperature_Value += "{";
  Temperature_Value += "\"TempC\":";
  Temperature_Value += dht.readTemperature();
  Temperature_Value += "}";
  server.send(200, "application/json", Temperature_Value);
}

void handleDHT11Humidity() {
  
  String Humidity_Value = "";
  Humidity_Value += "{";
  Humidity_Value += "\"Humidity\":";
  Humidity_Value +=  dht.readHumidity();
  Humidity_Value += "}";

  server.send(200, "application/json", Humidity_Value); 
}

void handleCO() {
  String Gas_Value = "";
  Gas_Value += "{";
  Gas_Value += "\"Carbon_Monoxide\":";
  Gas_Value += MQGetGasPercentage(MQRead(MQ2PIN)/Ro,GAS_CARBON_MONOXIDE);
  Gas_Value += "}";
  
  server.send(200, "application/json", Gas_Value); 
  
}

void handleSmoke() {
  String s_Value = "";
  s_Value += "{";
  s_Value += "\"Smoke\":";
  s_Value += MQGetGasPercentage(MQRead(MQ2PIN)/Ro,GAS_SMOKE);
  s_Value += "}";
  
  server.send(200, "application/json", s_Value); 
  
}

void handleUnitId() {
  const String UnitId =  "{\"UnitId\":\"F1 - L1\"}";
  server.send(200, "application/json", UnitId);
 
}

void setup(void){
  Serial.begin(9600);

  Ro = MQCalibration(MQ2PIN);                       //Calibrating the sensor. Please make sure the sensor is in clean air 
                     
  Serial.print("Calibration is done...\n"); 
  Serial.print("Ro=");
  Serial.print(Ro);
  Serial.print("kohm");
  Serial.print("\n");
  dht.begin();  
 
  WiFi.softAP(ssid, password);  

  IPAddress apip = WiFi.softAPIP(); 
  Serial.print("Connect your wifi laptop/mobile phone to this NodeMCU Access Point : ");
  Serial.println(ssid);
  Serial.print("Visit this IP : ");
  Serial.print(apip); //--> Prints the IP address of the server to be visited
  Serial.println(" in your browser.");
  
  server.on("/", handleRoot); 
  server.on("/readTemperature", handleDHT11Temperature);  
  server.on("/readHumidity", handleDHT11Humidity);
  server.on("/readCO", handleCO);  
  server.on("/readUnitId", handleUnitId);  

  server.begin(); //--> Start server
  Serial.println("HTTP server started");
}

void loop(){
  
  Serial.print("Temperature:");
  Serial.print(dht.readTemperature());
  Serial.print(",");
  Serial.print("Humidity:");
  Serial.print(dht.readHumidity());
  Serial.print(",");
  Serial.print("CARBON MONOXIDE:");
  Serial.println(MQGetGasPercentage(MQRead(MQ2PIN)/Ro,GAS_CARBON_MONOXIDE));
  
  server.handleClient();
}
