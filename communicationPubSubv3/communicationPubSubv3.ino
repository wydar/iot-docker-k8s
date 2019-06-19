#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

#define DHTPIN 2  
#define DHTTYPE DHT11

int ledPin = 13;
int ledState = LOW;

int del = 5000;

DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "MOVISTAR_7728";                               // wifi ssid
const char* password =  "supercalifragilisticoespialidoso27";     // wifi password
const char* mqttServer = "192.168.1.55";                          // IP adress Raspberry Pi
const int mqttPort = 1883;
const char* mqttUser = "mqttusr";                                 // if you don't have MQTT Username, no need input
const char* mqttPassword = "mqttrpi";                             // if you don't have MQTT Password, no need input

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {

  Serial.begin(115200);

    //Init LED output
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);
  
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");

    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {

      Serial.println("connected");
      client.subscribe("esp8266");
    } else {

      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);

    }
  }

//  client.publish("esp8266", "Hello Raspberry Pi");
//  client.subscribe("esp8266");

}

void callback(char* topic, byte* payload, unsigned int length) {
  bool error = false;
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println(length);
  if((char)payload[0]=='5'){
    Serial.println("yuju");
    }
  
  if(length==1){
    int x = (int)payload[0];
    changeFreq(x);
  }else if(length == 2){
    char arr [2];
    arr[0] = payload[0];
    arr[1] = payload[1];
    String str = arr;
    Serial.println(arr);
    if(str == "on"){
      ledState = HIGH;
      digitalWrite(ledPin, ledState);
      Serial.println("Turn on LED");
    }else{
        error = true;
        Serial.println("error1");
    }
  }else if(length == 3){
    char arr [3];
    arr[0] = payload[0];
    arr[1] = payload[1];
    arr[2] = payload[2];
    String str = arr;
    if(str == "off"){
      ledState = LOW;
      digitalWrite(ledPin, ledState);
      Serial.println("Turn off LED");
    }else{
      error = true;  
      Serial.println("error2");
    }
   
  }
  
  if(error || length > 3){
     Serial.print("ERROR: Message do not recognize ");
    Serial.print("Message:");
    for (int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
      
    }
    Serial.println();
    Serial.println("-----------------------");
  }

}

void changeFreq(int state){
  if(state==48){ //48 en bytes equivale a recibir un 0
    ledState=LOW;
    digitalWrite(ledPin, ledState);
    char led [1]={'0'};
    client.publish("led-micro",led);    
  }else if(state==49){ //49 en bytes equivale a recibir un 1
    ledState=HIGH;    
    digitalWrite(ledPin, ledState);
    char led [1]={'1'};
    client.publish("led-micro",led);    
  }else if(state==50){
    //Se detecta movimiento por tanto se aumenta la frecuencia de muestreo
    del=2000;
    Serial.println("Change Fregquncy Hight");
  }else if(state==51){
    //Se detecta ausencia de movimiento por tanto se baja la frecuencia de muestreo
    del=7000;
    Serial.println("Change Fregquncy Low");
  }else{
    Serial.print("***ERROR state do not recognize ");
    Serial.println(state);
  }
  Serial.println("-----------------------");
}

float readVoltage(){
    int val = analogRead(A0);
    if(isnan(val)){
      Serial.println("***ERROR in voltage");
      return -999;
    }else{
      float voltage = val * (5.0 / 1023.0);
      Serial.print("voltage: ");
      Serial.println(voltage);
      return voltage;
    }     
}

float readDHTHumidity() {
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println("***ERROR in humidity sensor");
    return -999;
  }
  else {
    Serial.print("Humidity: ");
    Serial.println(h);
    return h;
  }
}

float readDHTTemperature() {

  float t = dht.readTemperature();
  if (isnan(t)) {    
    Serial.println("***ERROR in temperature sensor");
    return -9999;
  }
  else {
    Serial.print("Temperature: ");
    Serial.println(t);
    return t;
  }
}

void loop() {
  //  char str[15];
  //  sprintf(str, "%g", voltage);

    char volt [10];
    char temp [10];
    char humd [10];

    sprintf(volt,"%g",readVoltage());
    sprintf(temp,"%g",readDHTTemperature());
    sprintf(humd,"%g",readDHTHumidity());

    Serial.println("-----------------------");
    
    client.publish("voltage-micro", volt);
    client.publish("temperature-micro", temp);
    client.publish("humidity-micro",humd);
    

    delay(del);
    client.loop();
}
