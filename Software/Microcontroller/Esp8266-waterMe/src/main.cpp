#include <SPI.h>
#include <Wire.h>
#include <DHT.h>
#include <Arduino.h>
#include <vector>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoilMoistureSensor/SoilMoistureSensor.h>

#define DHTTYPE DHT22   // DHT 22

#define DHT1PIN 0    
#define DHT2PIN 3    

#define SMS1POWERPIN 15
#define SMS2POWERPIN 12
#define SMSREADPIN A0
#define SMSVOLTAGE 3.3

#define mqtt_server "broker.hivemq.com"
//#define mqtt_user "your_username"
//#define mqtt_password "your_password"

#define MicroControllerID WiFi.macAddress()
#define DEEP_SLEEP_TIMER 3e9

#define humidity_topic "sensor/humidity"
#define temperature_topic "sensor/temperature"  

Adafruit_SSD1306 OLED = Adafruit_SSD1306(128, 32, &Wire);

DHT dht1(DHT1PIN, DHTTYPE);
DHT dht2(DHT2PIN, DHTTYPE);

SoilMoistureSensor SMS1(SMS1POWERPIN, SMSREADPIN, SMSVOLTAGE);
SoilMoistureSensor SMS2(SMS2POWERPIN, SMSREADPIN, SMSVOLTAGE);

const char* ssid = "Copacabana_Garden";
const char* password = "pw_copaGarden";

WiFiClient espClient;
PubSubClient client(espClient);

float dht_temp_1 = NAN;
float dht_hum_1 = NAN;

float dht_temp_2 = NAN;
float dht_hum_2 = NAN;

float SMS1_voltage = NAN;
float SMS2_voltage = NAN;

void updateOLED()
{
  OLED.clearDisplay();
  OLED.setCursor(0,0);
  
  //IP / connection Status
  OLED.println(WiFi.isConnected() ? WiFi.localIP().toString() : "Disconnected");
  
  //Mac Adress
  OLED.println(MicroControllerID);

  // Temperature and Humidity Sensor 1
  OLED.print("T1: ");
  OLED.print(String(dht_temp_1).c_str());
  OLED.print(" - H1: ");
  OLED.println(String(dht_hum_1).c_str());

  // Temperature and Humidity Sensor 2
  OLED.print("T2: ");
  OLED.print(String(dht_temp_2).c_str());
  OLED.print(" - H2: ");
  OLED.println(String(dht_hum_2).c_str());

  // Vertical Moisture Bars
  OLED.drawRect(112,0,8,32,1);
  OLED.fillRect(112,32-32*(SMS1_voltage/SMSVOLTAGE),8,32*(SMS1_voltage/SMSVOLTAGE),1);

  OLED.drawRect(120,0,8,32,1);
  OLED.fillRect(120,32-32*(SMS2_voltage/SMSVOLTAGE),8,32*(SMS2_voltage/SMSVOLTAGE),1);

  OLED.display();
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

  // Acquire Sensor information
  SMS1.turnPowerOn();
  delay(5000);
  SMS1_voltage = SMS1.readSensorVoltage();
  SMS1.turnPowerOff();

  SMS2.turnPowerOn();
  delay(5000);
  SMS2_voltage = SMS2.readSensorVoltage();
  SMS2.turnPowerOff();

  dht_temp_1 = dht1.readTemperature();
  dht_hum_1 = dht1.readHumidity();

  dht_temp_2 = dht2.readTemperature();
  dht_hum_2 = dht2.readHumidity();

  //Update OLED
  updateOLED();

  // Update Serial and MQTT Broker
  Serial.print("New temperature DHT_1 -> ");
  Serial.println((MicroControllerID+"/"+String(dht_temp_1)).c_str());
  client.publish(temperature_topic, (MicroControllerID+"/Sensor1/"+String(dht_temp_1)).c_str(), true);
  
  Serial.print("New humidity DHT_1 -> ");
  Serial.println((MicroControllerID+"/"+String(dht_hum_1)).c_str());
  client.publish(humidity_topic, (MicroControllerID+"/Sensor1/"+String(dht_hum_1)).c_str(), true);

  Serial.print("New temperature DHT_2 -> ");
  Serial.println((MicroControllerID+"/"+String(dht_temp_2)).c_str());
  client.publish(temperature_topic, (MicroControllerID+"/Sensor2/"+String(dht_temp_2)).c_str(), true);
  
  Serial.print("New humidity DHT_2 -> ");
  Serial.println((MicroControllerID+"/"+String(dht_hum_2)).c_str());
  client.publish(humidity_topic, (MicroControllerID+"/Sensor2/"+String(dht_hum_2)).c_str(), true);

  Serial.print("New SMS1 value -> ");
  Serial.println((MicroControllerID+"/"+String(SMS1_voltage)).c_str());
  client.publish(temperature_topic, (MicroControllerID+"/"+String(SMS1_voltage)).c_str(), true);
  
  Serial.print("New SMS2 value -> ");
  Serial.println((MicroControllerID+"/"+String(SMS2_voltage)).c_str());
  client.publish(temperature_topic, (MicroControllerID+"/"+String(SMS2_voltage)).c_str(), true);

  //goToSleep();
}
