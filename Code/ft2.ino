#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#ifndef STASSID
#define STASSID ""
#define STAPSK  ""
#endif

#include <LiquidCrystal.h>;

const char* ssid = STASSID;
const char* password = STAPSK;
ESP8266WebServer server(80);
const int led = 13;
String inputString = ""; 
bool stringComplete = false;

unsigned int buf[500];
int i=0;
const int analogInPin = A0;  
unsigned long sum=0 ;

/*blue = 1;
 red = 3;
yellow = 5;
 high = 12;
 low = 15;*/

int pin[] = {5,4,13,12,15};
const String ledtip[7] = {"bh","bl","rh","rl","yh","yl","off"}; // не уверен насчет *
const int allled[7][5] = {
  {1,2,2,0,2},
  {1,2,2,2,0},
  {2,1,2,0,2},
  {2,1,2,2,0},
  {2,2,1,0,2},
  {2,2,1,2,0},
  {0,0,0,0,0}
  };
void serialEvent() {
  while (Serial.available()) { // get the new byte:
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      stringComplete = true;
   }
  else{
  inputString += inChar;
  }}}
  
void sensor () {
buf[i] = analogRead(analogInPin);
if (i== 499){
  for (int l=0; l<500; l++){
   sum+=buf[l];
   }
  float average = sum/500;
  Serial.print("среднее значение:");
  Serial.println(average);
  sum=0;
  i=0;
 }
 delay(5);
 i++;
}
void ledstatus (String state){// state = incomingSerialData
  // проверяем, поступают ли какие-то команды
  for (int row = 0; row < 7; row++) {
    //Serial.println(ledtip[row]);
   if (state.equals(ledtip[row])){
   // Serial.println(state);
    for (int col = 0; col < 5; col++) {
      if (allled[row][col]==2){
        pinMode(pin[col], INPUT);
        delay(5);
      }
      else {
        pinMode(pin[col], OUTPUT);
        delay(5);
        digitalWrite(pin[col],allled[row][col]);
   }}}}}

void setup()  {
 pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  pinMode(analogInPin, INPUT);
  inputString.reserve(200);
}
void loop()  { 
  if (stringComplete) {
    Serial.println(inputString);
    // clear the string:
    ledstatus(inputString);
    inputString = "";
    stringComplete = false;
  }
  serialEvent();
  sensor();
}
