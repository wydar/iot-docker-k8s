#include <ESP8266WiFi.h>
#include <PubSubClient.h>

int sensor = 13;  // Digital pin D7


const char* ssid = "MOVISTAR_7728";                               // wifi ssid
const char* password =  "supercalifragilisticoespialidoso27";     // wifi password
const char* mqttServer = "192.168.1.55";                          // IP adress Raspberry Pi
const int mqttPort = 1883;
const char* mqttUser = "mqttusr";                                 // if you don't have MQTT Username, no need input
const char* mqttPassword = "mqttrpi";                             // if you don't have MQTT Password, no need input

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  pinMode(sensor, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");

  client.setServer(mqttServer, mqttPort);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");

    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
      Serial.println("connected");
    } else {

      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);

    }  
  }
}

void loop() {
  int state = digitalRead(sensor);
  if (state == LOW) {
    Serial.println("Motion detected!");
    digitalWrite(LED_BUILTIN, LOW);
    char msg [1]={'2'};
    client.publish("pir-micro", msg);
    delay(10000);
  }else {
    Serial.println("No motion detected.");
    digitalWrite(LED_BUILTIN, HIGH);
    char msg [3] ={'3'};
    client.publish("pir-micro", msg);
    delay(1000);
  }
  Serial.println("------------------");
}
