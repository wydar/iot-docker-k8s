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
const char* mqttServer = "192.168.1.50";                          // IP adress Raspberry Pi
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

  if(length==1){
    int x = (int)payload[0];
    Serial.print("Payload: ");
    Serial.println(x);
    changeLedAndFreq(x);
  }else{
     Serial.print("Message arrived in topic: ");
    Serial.println(topic);
  
    Serial.print("Message:");
    for (int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
      
    }

    Serial.println();
    Serial.println("-----------------------");
  }

}

void changeLedAndFreq(int estate){
  if(estate==48){ //48 en bytes equivale a recibir un 0
    ledState=LOW;
    del=7000;
    digitalWrite(ledPin, ledState);
    Serial.println("Apagando led, bajando la frecuencia de muestreo");
  }else if(estate==49){ //48¡9 en bytes equivale a recibir un 1
    ledState=HIGH;
    del=2000;
    digitalWrite(ledPin, ledState);
    Serial.println("Encendiendo led, subiendo la frecuencia de muestreo");
  }else{
    Serial.print("***ERROR: estado no reconocido: ");
    Serial.println(estate);
  }
  Serial.println("-----------------------");
}

float readVoltage(){
    int val = analogRead(A0);
    if(isnan(val)){
      Serial.println("***ERROR: lectura del voltaje incorrecta");
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
    Serial.println("***ERROR: lectura de la humedad incorrecta");
    return -999;
  }
  else {
    Serial.print("Humedad: ");
    Serial.println(h);
    return h;
  }
}

float readDHTTemperature() {

  float t = dht.readTemperature();
  if (isnan(t)) {    
    Serial.println("***ERROR: lectura de la temperatura incorrecta");
    return -9999;
  }
  else {
    Serial.print("Temperatura: ");
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
