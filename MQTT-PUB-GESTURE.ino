//Librerie ESP
#include <ESP8266WiFi.h> //!!PROBLEMA!!!! libreria in conflitto con SparkFun (RISOLTO)
//#include <WiFiManager.h>

//#include "WConfig.h"
//#include <DNSServer.h>
//#include <ESP8266WebServer.h>

#include <PubSubClient.h>

//Lbrerie Sensore movimento
#include <Wire.h>
#include <SparkFun_APDS9960.h>

/*************  IMPOSTAZIONI PIN e VARIABILI **************************/
//PIN
#define APDS9960_SDA    4  //GPIO4 (D2)
#define APDS9960_SCL    5  //GPIO5 (D1)
const byte APDS9960_INT  = 12; //GPIO12 (D6)

// Variabili globali
SparkFun_APDS9960 apds = SparkFun_APDS9960();
volatile bool isr_flag = 0;
//unsigned long previousMillis = 0;
//unsigned long interval = 10000;

//Interrupt callback in IRAM
void ICACHE_RAM_ATTR interruptRoutine ();

//WiFi
const char* ssid     = "TIM-24156322";
const char* password = "cgRKZqFsGn6rXgTcRMd7fphH";
//const char* ssid     = "EX-TpLink";
//const char* password = "78174023";
//ESP8266WebServer server(80);
IPAddress local_IP(192, 168, 1, 60);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);

//MQTT
const char* mqttServer = "192.168.1.57";
const char* mqttUser = "mqttb";
const char* mqttPassword = "soloIOT";
const int mqttPort = 1883;
WiFiClient espClient;
PubSubClient client(espClient);
//#define Topic "Casa/LEDW2812B"
const char* Topic ="Casa/LEDW2812B";

/*******************SETUP sensore *************************/
void setup() {
 Wire.begin(APDS9960_SDA,APDS9960_SCL);

  // Set interrupt pin as input
  pinMode(digitalPinToInterrupt(APDS9960_INT), INPUT);

  // Initialize Serial port
  Serial.begin(115200);
  Serial.println();
  Serial.println(F("--------------------------------"));
  Serial.println(F("SparkFun APDS-9960 - GestureTest"));
  Serial.println(F("--------------------------------"));
  
  // Initialize interrupt service routine
  attachInterrupt(digitalPinToInterrupt(APDS9960_INT), interruptRoutine, FALLING);

  // Initialize APDS-9960 (configure I2C and initial values)
  if ( apds.init() ) {
    Serial.println(F("APDS-9960 initialization complete"));
  } else {
    Serial.println(F("Something went wrong during APDS-9960 init!"));
  }
  
  // Start running the APDS-9960 gesture sensor engine
  if ( apds.enableGestureSensor(true) ) {
    Serial.println(F("Gesture sensor is now running"));
  } else {
    Serial.println(F("Something went wrong during gesture sensor init!"));
  }
  
/****************SETUP WI-Fi e MQTT ***********************/

// Configures static IP address
  /*if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }*/
  
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
// Print local IP address
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  //server.begin();

//MQTT
  client.setServer(mqttServer, mqttPort);
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP8266ClientPUB", mqttUser, mqttPassword )) {
      Serial.println("connected");
    } else {

      Serial.println("failed with state ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}
/***************************MAIN LOOP *********************/
void loop() {
  client.loop();
  if( isr_flag == 1 ) {
    detachInterrupt(digitalPinToInterrupt(APDS9960_INT));
    handleGesture();
    isr_flag = 0;
    attachInterrupt(digitalPinToInterrupt(APDS9960_INT), interruptRoutine, FALLING);
  }

}

/****************************GESTIONE-INVIO DATI SENSORE *************************/

void interruptRoutine() {
  isr_flag = 1;
}


void handleGesture() {
    if ( apds.isGestureAvailable() ) {
    switch ( apds.readGesture() ) {
      case DIR_UP:
        Serial.println("UP");
        client.publish(Topic,"UP");
        break;
      case DIR_DOWN:
        Serial.println("DOWN");
        client.publish(Topic,"DOWN");
        break;
      case DIR_LEFT:
        Serial.println("LEFT");
        client.publish(Topic,"LEFT");
        break;
      case DIR_RIGHT:
        Serial.println("RIGHT");
        client.publish(Topic,"RIGHT");
        break;
      case DIR_NEAR:
        Serial.println("NEAR");
        client.publish(Topic,"NEAR");
        break;
      case DIR_FAR:
        Serial.println("FAR");
        client.publish(Topic,"FAR");
        break;
      default:
        Serial.println("NONE");
        client.publish(Topic,"NONE");
    }
  }
}
