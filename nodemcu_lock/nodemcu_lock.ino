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
    * A0 = LM35
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



// WiFi Config Portal Settings
const char wifi_config_ap_name[10] = "SmartLock";
const char wifi_config_ap_password[9] = "12345678";
IPAddress wifi_config_ap_ip = IPAddress(192, 168, 1, 1);

//  MQTT Broker & Auth
#error "Change auth"
const char* mqtt_broker_url = "";
const int mqtt_broker_port = 0;
const char* mqtt_broker_username = "";
const char* mqtt_broker_password = "";

//  Pub & Sub topics
const char topic_panel[6] = "panel";  //  Sub - Remote
const char topic_lock[5] = "lock";    //  Pub - Me
const char topic_lock_temp[10] = "lock_temp";    //  Pub - Me  (Temperature)
const char topic_lock_online[12] = "lock_online";    //  Pub - Me  (Online state indicator)

//  Lock state
bool stateLocked = true;
const char sLocked[2] = "1"; 
const char sUnlocked[2] = "0"; 

//  Lock online  state
const char sOnline[2] = "1"; 
const char sOffline[2] = "0"; 


// Interfaces
//  Pin layout
int pinLM35 = A0;
int pinServoLock = D1;
int pinLedWiFi = D6;
int pinLedMqtt = D7;

//  Hardware
Servo servoLock;

WiFiClient espClient; //  WiFi Client
PubSubClient mqttClient(espClient); //  MQTT Client


void setup() {
    Serial.begin(115200);

    servoLock.attach(pinServoLock);
    pinMode(pinLedWiFi, OUTPUT);
    pinMode(pinLedMqtt, OUTPUT);

    digitalWrite(pinLedWiFi, LOW);
    digitalWrite(pinLedMqtt, LOW);

    setLock(true);
    
    WiFiManager wifiManager;
    WiFiManagerParameter title("<h2>SmartLock Configuration Portal</h2>");
    wifiManager.addParameter(&title);
    wifiManager.setConfigPortalTimeout(60);  // 60 sec
    wifiManager.setAPStaticIPConfig(wifi_config_ap_ip, wifi_config_ap_ip, IPAddress(255,255,255,0));
    if (!wifiManager.autoConnect(wifi_config_ap_name, wifi_config_ap_password)) {
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
  sendTemperature();
  mqttClient.loop();
}

void connect() {
  setLock(true);
  
  // Loop until connected
  while (!mqttClient.connected()) {
    Serial.println(F("Attempting MQTT connection..."));
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

void sendTemperature() {
    float temp = 3.5 * 100 * (analogRead(pinLM35)/1024.0); 
    Serial.print(F("Temperature (C) = "));
    Serial.println(temp);
    delay(100);
    
    char buffer[10];
    dtostrf(temp, 0, 0, buffer);
    mqttClient.publish(topic_lock_temp, buffer);
    blinkLedMqtt(true);
    delay(1000);
}

void blinkLedMqtt(bool shortDelay) {
    digitalWrite(pinLedMqtt, LOW);
    delay(shortDelay ? 100 : 300);
    digitalWrite(pinLedMqtt, HIGH);
}
