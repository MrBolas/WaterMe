#include <SPI.h>
#include <Wire.h>
#include <DHT.h>
#include <time.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoilMoistureSensor/SoilMoistureSensor.h>

#define DHTTYPE DHT22   // DHT 22

#define DHT1PIN 13    
#define DHT2PIN 3    

#define SMS1POWERPIN 15
#define SMS2POWERPIN 12
#define SMS3POWERPIN 0
#define SMS4POWERPIN 14
#define SMSREADPIN A0
#define SMSVOLTAGE 3.3

#define mqtt_server "94.62.211.190"
//#define mqtt_user "your_username"
//#define mqtt_password "your_password"

#define MicroControllerID WiFi.macAddress()

#define DEEP_SLEEP_TIMER 3e8  //microseconds 5 minutes
#define WAKE_UP_TIME 5e3     //milliseconds

#define main_topic "WaterMe"

Adafruit_SSD1306 OLED = Adafruit_SSD1306(128, 32, &Wire);

//Setup JSON
DynamicJsonDocument doc(2048);
char serialized_json[1024];

DHT dht1(DHT1PIN, DHTTYPE);
DHT dht2(DHT2PIN, DHTTYPE);

//Flower Pot1
SoilMoistureSensor SMS1(SMS1POWERPIN, SMSREADPIN, SMSVOLTAGE);
SoilMoistureSensor SMS2(SMS2POWERPIN, SMSREADPIN, SMSVOLTAGE);

//Flower Pot 2
SoilMoistureSensor SMS3(SMS3POWERPIN, SMSREADPIN, SMSVOLTAGE);
SoilMoistureSensor SMS4(SMS4POWERPIN, SMSREADPIN, SMSVOLTAGE);

const char* ssid = "The Internet";
const char* password = "Papaya0721";

WiFiClient espClient;
PubSubClient client(espClient);
const long utcOffsetInSeconds = 3600;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

int dht_temp_1 = 0;
int dht_hum_1 = 0;

int dht_temp_2 = 0;
int dht_hum_2 = 0;

float SMS1_value = NAN;
float SMS2_value = NAN;
float SMS3_value = NAN;
float SMS4_value = NAN;

void updateOLED()
{
  OLED.clearDisplay();
  OLED.setCursor(0,0);
  
  //IP / connection Status
  OLED.print(WiFi.isConnected() ? WiFi.localIP().toString() : "Disconnected");

  //Temperature and Humidity
  OLED.setCursor(80,0);
  OLED.print("T:");
  int reported_temp = dht_temp_1 != 0 ? dht_temp_2 != 0 ? (dht_temp_1+dht_temp_2)/2 : dht_temp_1 : 0;
  int reported_hum = dht_hum_1 != 0 ? dht_hum_2 != 0 ? (dht_hum_1+dht_hum_2)/2 : dht_hum_1 : 0;

  if (reported_temp == 0)
  {
    OLED.print("- ");
  }else{
    OLED.print(reported_temp);
  }

  OLED.print("H:");
  
  if (reported_hum == 0)
  {
    OLED.println("- ");
  }else{
    OLED.println(reported_hum);
  }

  //Mac Adress
  OLED.println(MicroControllerID);

  // Horizontal Moisture Bars for Pot 1
  OLED.drawRect(0,16,50,8,1);
  OLED.fillRect(0,16,50*(SMS1_value/SMSVOLTAGE),8,1);
  OLED.setCursor(52,16);
  OLED.print("Pot1");
  OLED.drawRect(78,16,50,8,1);
  OLED.fillRect(128-78*(SMS2_value/SMSVOLTAGE),16,128-78*(SMS2_value/SMSVOLTAGE),8,1);
  
  // Horizontal Moisture Bars for Pot 2
  OLED.drawRect(0,24,50,8,1);
  OLED.fillRect(0,24,50*(SMS3_value/SMSVOLTAGE),8,1);
  OLED.setCursor(52,24);
  OLED.print("Pot2");
  OLED.drawRect(78,24,50,8,1);
  OLED.fillRect(128-78*(SMS4_value/SMSVOLTAGE),24,128-78*(SMS4_value/SMSVOLTAGE),8,1);

  OLED.display();
}

void readingVerification()
{
  if (dht_temp_1 > 120)
  {
    dht_temp_1 = 0;
  }
  
  if (dht_temp_2 > 120)
  {
    dht_temp_2 = 0;
  }

  if (dht_hum_1 > 120)
  {
    dht_hum_1 = 0;
  }

  if (dht_hum_2 > 120)
  {
    dht_hum_2 = 0;
  }
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
 
  updateOLED();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    // If you do not want to use a username and password, change next line to
     if (client.connect("client")) {
    //if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void goToSleep()
{
  OLED.clearDisplay();
  OLED.display();

  //enter deep sleep in microseconds
  ESP.deepSleep(DEEP_SLEEP_TIMER); 
}

void publish_MQTT()
{
  serializeJson(doc, serialized_json);
  Serial.println(serialized_json);
  client.publish(MicroControllerID.c_str(), serialized_json);
  delay(250); // create delay so other clients can cope with data flow
}

bool validReading(int reading)
{
  if (reading > 120 || reading < -10)
  {
    return false;
  }
  return true;
}

void setup() {
  //Setup Serial
  Serial.begin(115200);

  OLED.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
  OLED.setCursor(0,0);
  OLED.setTextSize(1);
  OLED.setTextColor(SSD1306_WHITE);
  updateOLED();

  //Setup Wifi
  setup_wifi();
  client.setServer(mqtt_server, 1883);

  // Turn ON both SMS
  pinMode(SMS1POWERPIN, OUTPUT);
  digitalWrite(SMS1POWERPIN, LOW);
  
  pinMode(SMS2POWERPIN, OUTPUT);
  digitalWrite(SMS2POWERPIN, LOW);

  pinMode(SMS3POWERPIN, OUTPUT);
  digitalWrite(SMS3POWERPIN, LOW);
  
  pinMode(SMS4POWERPIN, OUTPUT);
  digitalWrite(SMS4POWERPIN, LOW);

  //initializa DHT22 sensors
  dht1.begin();
  dht2.begin();

  // Delay to initialize DHT22 sensors
  delay(1000);
}


void loop() {
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  //Sends wake notification to main topic
  client.publish(main_topic, MicroControllerID.c_str());

  unsigned long time = millis();

  // Acquire Sensor information
  SMS1.turnPowerOn();
  delay(250);
  SMS1_value = SMS1.getWaterVolume();
  SMS1.turnPowerOff();

  SMS2.turnPowerOn();
  delay(250);
  SMS2_value = SMS2.getWaterVolume();
  SMS2.turnPowerOff();

  SMS3.turnPowerOn();
  delay(250);
  SMS3_value = SMS3.getWaterVolume();
  SMS3.turnPowerOff();

  SMS4.turnPowerOn();
  delay(250);
  SMS4_value = SMS4.getWaterVolume();
  SMS4.turnPowerOff();

  dht_temp_1 = dht1.readTemperature();
  dht_hum_1 = dht1.readHumidity();

  dht_temp_2 = dht2.readTemperature();
  dht_hum_2 = dht2.readHumidity();

  //Reading Verification
  readingVerification();

  //Update OLED
  updateOLED();

  // Update Serial and MQTT Broker
  Serial.print(timeClient.getFormattedTime());
  Serial.println((" -> DHT_TEMP_1/"+String(dht_temp_1)).c_str());
  
  Serial.print(timeClient.getFormattedTime());
  Serial.println((" -> DHT_HUM_1/"+String(dht_hum_1)).c_str());

  Serial.print(timeClient.getFormattedTime());
  Serial.println((" -> DHT_TEMP_2/"+String(dht_temp_2)).c_str());
  
  Serial.print(timeClient.getFormattedTime());
  Serial.println((" -> DHT_HUM_2/"+String(dht_hum_2)).c_str());

  Serial.print(timeClient.getFormattedTime());
  Serial.println((" -> SMS1/"+String(SMS1_value)).c_str());
  
  Serial.print(timeClient.getFormattedTime());
  Serial.println((" -> SMS2/"+String(SMS2_value)).c_str());

  Serial.print(timeClient.getFormattedTime());
  Serial.println((" -> SMS3/"+String(SMS3_value)).c_str());
  
  Serial.print(timeClient.getFormattedTime());
  Serial.println((" -> SMS4/"+String(SMS4_value)).c_str());

  // Build JSON
  // Serializes and punlishes JSON over MQTT
  timeClient.update();
  doc["mac_address"] = MicroControllerID;
  doc["time"] = timeClient.getEpochTime();

  if (validReading(dht_temp_1))
  {
    doc["type"] = "DHT1_temp";
    doc["value"] = dht_temp_1;
    publish_MQTT();
  }

  if (validReading(dht_hum_1))
  {
    doc["type"] = "DHT1_hum";
    doc["value"] = dht_hum_1;
    publish_MQTT();
  }

  if (validReading(dht_temp_2))
  {
    doc["type"] = "DHT2_temp";
    doc["value"] = dht_temp_2;
    publish_MQTT();
  }

  if (validReading(dht_hum_2))
  {
    doc["type"] = "DHT2_hum";
    doc["value"] = dht_hum_2;
    publish_MQTT();
  }

  doc["type"] = "SMS1";
  doc["value"] = SMS1_value;
  publish_MQTT();

  doc["type"] = "SMS2";
  doc["value"] = SMS2_value;
  publish_MQTT();

  doc["type"] = "SMS3";
  doc["value"] = SMS3_value;
  publish_MQTT();

  doc["type"] = "SMS4";
  doc["value"] = SMS4_value;
  publish_MQTT();

/*
  if ((SMS1.beingWatered() 
  || SMS2.beingWatered() 
  || SMS3.beingWatered() 
  || SMS4.beingWatered()) 
  && time == 0) // testing for now. Should not use deepsleep cycle
  */
  if(false)
  {
    time = millis(); 
  }else if(time > WAKE_UP_TIME){
    time = 0;
    goToSleep();
  }
  
}
