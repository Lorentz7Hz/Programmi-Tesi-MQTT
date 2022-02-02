//Librerie ESP
#include <ESP8266WiFi.h> 
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>

/*************  IMPOSTAZIONI PIN e VARIABILI **************************/
const uint16_t NUM_LEDS = 20;
const uint8_t PixelPin = 12; //GPIO12 (D6)
Adafruit_NeoPixel neopixel(NUM_LEDS, PixelPin, NEO_GRB + NEO_KHZ800); 
int MODE =0;
int g=0;
int r=0;
int b=0;

//WiFi
const char* ssid     = "TIM-24156322";
const char* password = "cgRKZqFsGn6rXgTcRMd7fphH";
//const char* ssid     = "EX-TpLink";
//const char* password = "78174023";
//ESP8266WebServer server(80);
IPAddress local_IP(192, 168, 1, 61);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);

//MQTT
const char* mqttServer = "192.168.1.57";
const char* mqttUser = "mqttb";
const char* mqttPassword = "soloIOT";
const int mqttPort = 1883;
WiFiClient espClient;
PubSubClient client(espClient);
const char* TOPIC ="Casa/LEDW2812B";

/********************** SETUP WS2812B *************************/
void setup() {
  Serial.begin(115200);
  while (!Serial); // wait for serial 

   Serial.println();
   Serial.println("Sto inizializando...");
   Serial.flush();
   neopixel.begin();
   neopixel.clear();

   for (int i = 0; i <= NUM_LEDS-1; i++) {
    neopixel.setPixelColor(i, neopixel.Color(100, 255, 255));
    neopixel.show();
    delay(100);
   }
/****************SETUP WI-Fi e MQTT ***********************/
// Configures static IP address
  /*if (!WiFi.config(local_IP, gateway, subnet)) { //non va!
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
  client.setCallback(callback);
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP8266ClientSUB", mqttUser, mqttPassword )) {
      Serial.println("connected");
      client.subscribe("Casa/LEDW2812B"); 
    } else {

      Serial.println("failed with state ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}
/**********************MAIN LOOP *********************/
void loop() {
  client.loop();
}

/*********************FUNZIONI LED ********************/

void destra(){
   for (int i = 0; i <= NUM_LEDS-1; i++) {
      neopixel.setPixelColor(i, neopixel.Color(0, 0, 0));
      neopixel.show();
      delay(10);
   }
   
   if(MODE == 0 ){
    r=160;
    g=0;
    b=0;
   }else if(MODE==1) {
    r=0;
    g=160;
    b=0;
   }else if(MODE==2) {
    r=0;
    g=0;
    b=160;
   }
    for (int i = 0; i <= NUM_LEDS-1; i++) {
      neopixel.setPixelColor(i, neopixel.Color(r, g, b));
      neopixel.show();
      delay(200);
    }
    if(MODE<2){
      MODE++;
    }else{
      MODE=0;
    }
}

void sinistra(){
    for (int i = NUM_LEDS-1; i >= 0; i--) {
      neopixel.setPixelColor(i, neopixel.Color(0, 0, 0));
      neopixel.show();
      delay(10);
   }
   for (int i = NUM_LEDS-1; i >= 0; i--) {
      neopixel.setPixelColor(i, neopixel.Color(0, 160, 0));
      neopixel.show();
      delay(200);
   }
}

void su(){
   for (int i = NUM_LEDS-1; i >= 0; i--) {
      neopixel.setPixelColor(i, neopixel.Color(0, 0, 0));
      neopixel.show();
      delay(10);
   }
   int u=0;
   int d=NUM_LEDS-1;
   while(u <= ((NUM_LEDS)/2)-1 && d >= (NUM_LEDS-1)/2){
      neopixel.setPixelColor(u, neopixel.Color(0, 0, 160));
      neopixel.setPixelColor(d, neopixel.Color(0, 0, 160));
      neopixel.show();
      delay(200);
      u++;
      d--;
   }
}

void giù(){
   for (int i = NUM_LEDS-1; i >= 0; i--) {
      neopixel.setPixelColor(i, neopixel.Color(0, 0, 0));
      neopixel.show();
      delay(10);
   }
   int u=(NUM_LEDS/2)-1;
   int d=((NUM_LEDS-1)/2)+1;
   while(u >= 0 && d <= NUM_LEDS-1){
      neopixel.setPixelColor(u, neopixel.Color(0, 0, 160));
      neopixel.setPixelColor(d, neopixel.Color(0, 0, 160));
      neopixel.show();
      delay(200);
      u--;
      d++;
   }
   
}

void niente(){
  /*for (int i = NUM_LEDS-1; i >= 0; i--) {
      neopixel.setPixelColor(i, neopixel.Color(0, 0, 0));
      neopixel.show();
      delay(10);
   }
  RedRandom = random(0, 255);
  GreenRandom = random(0, 255);
  BlueRandom = random(0, 255);
  RedPosR = random(0, NUM_LEDS-1);
  GreenPosR = random(0, NUM_LEDS-1);
  BluePosR = random(0, NUM_LEDS-1);  
  neopixel.setPixelColor(RedPosR, neopixel.Color(RedRandom, 0, 0));
  neopixel.setPixelColor(GreenPosR, neopixel.Color(0, GreenRandom, 0));
  neopixel.setPixelColor(BluePosR, neopixel.Color(0, 0, BlueRandom));
  neopixel.show();
  delay(100);*/
}

void lontano(){
  
}
/************************FUNZIONE CALLBACK*****************************/ 
void callback(char* topic, byte* payload, unsigned int length) {

  
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  
  String message;
  for (int i =0; i<length; i++){
    message += (char)payload[i];
  }
  if(strcmp(topic, "Casa/LEDW2812B") == 0){
    if(message == "UP"){
      //client.publish("TOPIC", "RICEVUTO");
      su();
    }
    if(message == "DOWN"){
      //client.publish("TOPIC", "RICEVUTO"); 
      giù();
    }
    if(message == "LEFT"){
      //client.publish("TOPIC", "RICEVUTO");
      //Serial.println("RICEVUTO");
      sinistra();
    }
    if(message == "RIGHT"){
      //client.publish("TOPIC", "RICEVUTO");
      destra();
    }
  }
}
