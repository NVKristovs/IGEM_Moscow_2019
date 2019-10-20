#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#ifndef STASSID
#define STASSID "BioEngBF"
#define STAPSK  "0074959395738"
#endif

#include <LiquidCrystal.h>;

const char* ssid = STASSID;
const char* password = STAPSK;
ESP8266WebServer server(80);

int val;
const int led = 13;

/*blue = 1;
 red = 3;
yellow = 5;
 high = 12;
 low = 15;*/

int pin[] = {1,3,5,12,15};
const String ledtip[7] = {"bh","bl","rh","rl","yh","yl","off"}; // не уверен насчет *
const int allled[7][5] = {
  {1,2,2,0,2},
  {1,2,2,2,0},
  {2,1,2,0,2},
  {2,1,2,2,0},
  {2,2,1,0,2},
  {2,2,1,2,0},
  {2,2,2,2,2}
  };

String get(){
 char incomingSerialData[25]; // A character array to store received bytes
 int incomingSerialDataIndex = 0; // Stores the next 'empty space' in the array
 while(Serial.available() > 0){
   incomingSerialData[incomingSerialDataIndex] = Serial.read(); // Add the incoming byte to the array
   incomingSerialDataIndex++; // Ensure the next byte is added in the next position
 }
 return incomingSerialData;
 Serial.flush(); // Clear the serial buffer
}

void ledstatus (String state){
  // проверяем, поступают ли какие-то команды
 if (Serial.available()) {
 state = Serial.read(); // переменная state равна полученной команде
  for (int row = 0; row < 7; row++) {
   if (ledtip[row] == state){
    for (int col = 0; col < 5; row++) {
      if (allled[row][col]==2){
        pinMode(pin[col], INPUT);
      } 
      else {
        pinMode(pin[col], OUTPUT);
        digitalWrite(pin[col],allled[row][col]);
      }
    }
  }
 }
}
}
/*
void blueM () { // горит синий светодиод средне
 pinMode(blue, OUTPUT);//пины с +
  pinMode(Medium, OUTPUT);
  pinMode(red, INPUT); // пины с NC
  pinMode(13, OUTPUT);// пины c -
  pinMode(15, OUTPUT);
  digitalWrite (blue, HIGH);
  digitalWrite (medium, LOW);*/
void setup()  {
 Serial.begin(9600);
}
void loop()  { 
  String command = get(); // Calls the 'get' function
 if(command.length() > 0)  {// If there is a command, print it
   Serial.println(command);
 }
}
