
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPDash.h>
#include <WiFiClient.h>

AsyncWebServer server(80);

const char* ssid = ""; // Your WiFi SSID
const char* password = ""; // Your WiFi Password
const int x_axis_size = 20; //размер графика 
int x_axis[x_axis_size];
int y_axis[x_axis_size];
unsigned long sum =0;
int state_card; // статус прибора для карты состояния
const int analogInPin = A0;
const int window_avarage=200;
unsigned long previousMillis = 0;      
const long interval = 3000;// интервал в секунду 
int vi=0;
float average;
int pin[] = {12,4,5,13,15};
const String ledtip[7] = {"but2","but3","but4","but5","but6","but7","but1"}; // 
const int allled[7][5] = {
  {0,2,2,1,2},
  {0,2,2,2,1},
  {2,0,2,1,2},
  {2,0,2,2,1},
  {2,2,0,1,2},  
  {2,2,0,2,1},
  {0,0,0,0,0}
  };

float average_opt () {
  for( int i=0; i<window_avarage; i++){
   sum+= analogRead(analogInPin);
   delay(2);
   }
     int average =int(0.01775*pow(2.71828,(0.004593*(1024-(sum/window_avarage)))));
     Serial.print("среднее значение:");
     Serial.println(average);
     sum=0;
     return average;
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
void btnCallback (const char* id){// функция отвечающая за управление кнопками
Serial.println("Button pressed on Dash, ID - "+String(id));  
 ledstatus(String(id));
 if (String(id)=="but8" && state_card==0){
    state_card=1; 
  }
  else if(String(id)=="but1" && state_card==1){
    state_card=0;
  }
}


void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  Serial.print( "password:");
  Serial.println(password);
  
    ESPDash.init(server);   // Initiate ESPDash and attach your Async webserver instance
    // Add Respective Cards
    ESPDash.addNumberCard("num1", "Optical density",average );
    ESPDash.addLineChart("chart1", "Dependence of optical density on time",  x_axis,  vi, "the value of optical density",  y_axis,  vi);
    ESPDash.addStatusCard("card1", "Condition", state_card);
    ESPDash.addButtonCard("but1", "off"); 
    ESPDash.addButtonCard("but8", "start"); 
    ESPDash.addButtonCard("but2", "470 нм high");
    ESPDash.addButtonCard("but3", "470 нм low"); 
    ESPDash.addButtonCard("but4", "590 нм high");
    ESPDash.addButtonCard("but5", "590 нм low"); 
    ESPDash.addButtonCard("but6", "630 нм high");
    ESPDash.addButtonCard("but7", "630 нм low");
    ESPDash.attachButtonClick(btnCallback);
    server.begin();
    for (int i=0; i<x_axis_size;i++){
     x_axis[vi]= i;
    }
}

void loop() {
 unsigned long currentMillis = millis();
if (currentMillis - previousMillis >= interval) {
  previousMillis = currentMillis;
  average= int(average_opt());
  ESPDash.updateNumberCard("num1", average); 
  ESPDash.updateStatusCard("card1", state_card); 
    if (state_card==1){
      x_axis[vi]=vi;
      y_axis[vi]= average;
      ESPDash.updateLineChart("chart1", x_axis, vi, y_axis, vi);
      vi++;
      if (vi==20){
        state_card=0;
       }}
    else if (state_card==0){
    vi=0;
    y_axis[vi]= 0;
    }
}
}
