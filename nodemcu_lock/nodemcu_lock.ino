/*
  SmartLock
  * Made for NodeMCU (ESP8266)
  * Configures WiFi authentication using a provisional configuration portal using WiFiManager library.
  * Uses a 3D printed prototype with a servo as a lock
  * MQTT protocol for communication
  * Static web control panel using jQuery and Bootstrap
  
  Created by Harsha Alva, August 25, 2017.
  Released into the public domain.

  Components:
  * NodeMCU V3 (Chinese clone)
  * Breadboard
  * 9g RC servo
  * 2x LED
  * 2x 330 ohm resistor
  * Jumper cables

  Pin Connections:
  + NodeMCU V3
    * D1 = Servo input
    * D6 = WiFi status LED
    * D7 = MQTT status LED
    * G  = Common GND
*/

//  Arduino IDE for ESP
//  https://github.com/esp8266/Arduino
#include <ESP8266WiFi.h>      

//  Captive config portal
//  https://github.com/tzapu/WiFiManager
#include <DNSServer.h>              
#include <ESP8266WebServer.h>
#include <WiFiManager.h>      

//  Arduino MQTT Client'
//  https://pubsubclient.knolleary.net/index.html
#include <PubSubClient.h>

//  Servo control
#include <Servo.h>


//  MQTT Broker & Auth
#error "Change auth"
const char* mqtt_broker_url = "";
const int mqtt_broker_port = 0;
const char* mqtt_broker_username = "";
const char* mqtt_broker_password = "";

//  MQTT Client
WiFiClient espClient;
PubSubClient mqttClient(espClient); //  MQTT Client

//  Pub & Sub topics
const char* topic_panel = "panel";  //  Sub - Remote
const char* topic_lock = "lock";    //  Pub - Me
const char* topic_lock_online = "lock_online";    //  Pub - Me  (Online state indicator)

//  Lock state
bool stateLocked = true;
const char* sLocked = "1"; 
const char* sUnlocked = "0"; 

//  Lock online  state
const char* sOnline = "1"; 
const char* sOffline = "0"; 

//  Hardware
Servo servoLock;

//  Pin layout
int pinServoLock = D1;
int pinLedWiFi = D6;
int pinLedMqtt = D7;

void setup() {
    Serial.begin(115200);

    servoLock.attach(pinServoLock);
    pinMode(pinLedWiFi, OUTPUT);
    pinMode(pinLedMqtt, OUTPUT);

    digitalWrite(pinLedWiFi, LOW);
    digitalWrite(pinLedMqtt, LOW);
    
    WiFiManager wifiManager;
    WiFiManagerParameter title("<h2>SmartLock Configuration Portal</h2>");
    wifiManager.addParameter(&title);
    wifiManager.setConfigPortalTimeout(300);  // 300 sec = 5 min
    if (!wifiManager.autoConnect("SmartLock", "12345678")) {
        Serial.println(F("Failed to connect and hit timeout"));
        delay(3000);
        ESP.reset();  //reset and try again, or maybe put it to deep sleep
        delay(5000);
    }

    Serial.println(F("Connected to WiFi"));
    digitalWrite(pinLedWiFi, HIGH);
    
    mqttClient.setServer(mqtt_broker_url, mqtt_broker_port);
    mqttClient.setCallback(callback);
}

void callback(char* topic, byte* payload, unsigned int length) {
  blinkLedMqtt(false);
  if(strcmp(topic, topic_panel) == 0) {
    char value[length + 1];
    for (int i=0;i<length;i++) {
        value[i] = (char)payload[i];
    }
    value[length] = '\0';

    Serial.print(F("State from panel: "));
    Serial.println(value);
    setLock(strcmp(value, sUnlocked) == 0 ? false : true);
    sendLockStateFeedback();
  }
}



void loop() {
  if (!mqttClient.connected()) {
      connect();
  }
  mqttClient.loop();
}

void connect() {
  setLock(true);
  
  // Loop until connected
  while (!mqttClient.connected()) {
    Serial.print(F("Attempting MQTT connection..."));
    // Attempt to connect
    // connect (clientID, username, password, willTopic, willQoS, willRetain, willMessage)
    if (mqttClient.connect(String(ESP.getChipId()).c_str(), mqtt_broker_username, mqtt_broker_password, topic_lock_online, 1, true, sOffline)) {
        Serial.println(F("MQTT connected"));
        mqttClient.subscribe(topic_panel);
        sendLockOnlineState(true);
        sendLockStateFeedback();
        digitalWrite(pinLedMqtt, HIGH);
    } else {
        Serial.print(F("MQTT failed, rc="));
        Serial.println(mqttClient.state());
        digitalWrite(pinLedMqtt, LOW);
        delay(5000);  
    }
  }
}

void setLock(bool locked) {
    stateLocked = locked;
    servoLock.write(locked == true ? 0 : 60);
    //  digitalWrite(pinLed, locked ? LOW : HIGH);
}

void sendLockOnlineState(bool online) {
    delay(100);
    mqttClient.publish(topic_lock_online, online ? sOnline : sOffline, true);
    blinkLedMqtt(true);
}

void sendLockStateFeedback() {
    delay(100);
    mqttClient.publish(topic_lock, stateLocked ? sLocked : sUnlocked);
    blinkLedMqtt(true);
}

void blinkLedMqtt(bool shortDelay) {
    digitalWrite(pinLedMqtt, LOW);
    delay(shortDelay ? 100 : 300);
    digitalWrite(pinLedMqtt, HIGH);
}
