#include <Arduino_JSON.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Wire.h>
#include <ESP8266WiFiMulti.h>
#include <SoftwareSerial.h>

//sim800l TXD to esp8266 D7
//sim800l RXD to esp8266 D8
SoftwareSerial GSMSerial(D7,D8);
ESP8266WiFiMulti WiFiMulti;

const char* ssid = "Thesis";
const char* password = "10101010";

const char* SROOT = "http://192.168.4.1";
const char* SUID = "http://192.168.4.1/readUnitId";
const char* STemp = "http://192.168.4.1/readTemperature";
const char* SHumi = "http://192.168.4.1/readHumidity";
const char* SGas = "http://192.168.4.1/readGas";

String Root;
const char *Uid;

double temperature, humidity, CO, smk;

unsigned long previousMillis = 0;
const long interval = 5000;
int led = 0;
const int buzz = D0; 
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  GSMSerial.begin(9600);
  pinMode(led, OUTPUT);
  const int buzz = D1; 
  SIMInitialize();
  Serial.print("Connecting to ... ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    lcd.setCursor(0, 0);
    lcd.print("Connecting + + +");
    delay(500);
     lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print(" * * * * ");
    delay(500);
     lcd.clear();
  }
  Serial.println("");
  Serial.println("Connected to WiFi");
  lcd.setCursor(0, 0);
  lcd.print("Connected");
  delay(500);
  lcd.clear();

}

void loop() {
   updateSerial();
   digitalWrite(led, LOW);
  unsigned long currentMillis = millis();
  
  if(currentMillis - previousMillis >= interval) {
     // Check WiFi connection status
    if ((WiFiMulti.run() == WL_CONNECTED)) {
      Root =  httpGETRequest(SROOT);
      Serial.println(Root);
      JSONVar myObject = JSON.parse(Root);
  
      // JSON.typeof(jsonVar) can be used to get the type of the var
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
        return;      
  }    
      Serial.print("JSON object = ");
      Serial.println(myObject);
      JSONVar keys = myObject.keys();

JSONVar valueT =  myObject[keys[0]];
JSONVar valueU =  myObject[keys[1]];
JSONVar valueH =  myObject[keys[2]];
JSONVar valuesmk =  myObject[keys[3]];
JSONVar valueCO =  myObject[keys[4]];

temperature =double(valueT);
Uid = valueU;
humidity = double(valueH);
smk = double(valuesmk);
CO = double(valueCO);

  if (temperature > 45 && humidity < 25){
   if(smk > 110){
      alarm();
      SendSMS("Fire Fire");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" Fire Fire  ");
      delay(500);
      lcd.clear();
    }
  }
  if(temperature > 11){
      alarm();
     digitalWrite(led, HIGH);
     SendSMS("Temperature high "  +  String(Uid));
     lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Temperature high in "  +  String(Uid));
      delay(500);
      lcd.clear();
    }

    if(smk > 110){
        alarm();
     digitalWrite(led, HIGH);
     SendSMS("Too much Smoke in "  +  String(Uid));
     lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Too much Smoke in "  +  String(Uid));
      delay(500);
      lcd.clear();
    }
    if(CO > 150){
        alarm();
      SendSMS("Too much CO in "  +  String(Uid));
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Too much CO in "  +  String(Uid));
      delay(500);
      lcd.clear();
    }

lcd.setCursor(0, 0);
  lcd.print("Uint ID" +  String(Uid));
  lcd.setCursor(0,1);
  lcd.print("CO : " + String(CO) + " ppm ");
  delay(3000);
  lcd.clear();
 
  lcd.setCursor(0, 0);
  lcd.print("Temp : " + String(temperature) + " *C ");
  lcd.setCursor(0,1);
  lcd.print("Hum : " + String(humidity) + " %");
  delay(5000);
  lcd.clear();
  
    }
  }
 
}


String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;
    
  // Your IP address with path or Domain name with URL path 
  http.begin(client, serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "--"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}


void updateSerial() {
  delay(500);
  while (Serial.available()) {
   GSMSerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }

  while (GSMSerial.available()) {
    Serial.write(GSMSerial.read());//Forward what Software Serial received to Serial Port
  }
}
