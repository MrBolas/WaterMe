#include <SPI.h>
#include <Wire.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DisplayManager/DisplayManager.h>
#include <SoilMoistureSensor/SoilMoistureSensor.h>

#define DHTTYPE DHT22   // DHT 11 
#define DHTPIN 13    

#define SMSPOWERPIN 1
#define SMSREADPIN 0
#define SMSVOLTAGE 3.3

#define mqtt_server "192.168.8.102"
#define mqtt_user "your_username"
#define mqtt_password "your_password"

#define humidity_topic "sensor/humidity"
#define temperature_topic "sensor/temperature"

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);
DHT dht(DHTPIN, DHTTYPE);
SoilMoistureSensor SMS1(SMSPOWERPIN, SMSREADPIN, SMSVOLTAGE);

const char* ssid = "Copacabana_Garden";
const char* password = "pw_copaGarden";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  display.print("Connecting to ");
  display.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");

  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
 
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("WiFi connected");
  display.println("IP address: ");
  display.println(WiFi.localIP());
  display.display();

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    // If you do not want to use a username and password, change next line to
     if (client.connect("ESP8266Client")) {
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

bool checkBound(float newValue, float prevValue, float maxDiff) {
  return !isnan(newValue) &&
         (newValue < prevValue - maxDiff || newValue > prevValue + maxDiff);
}

long lastMsg = 0;
float temp = 0.0;
float hum = 0.0;
float diff = 1.0;

void setup() {
  //Setup Serial
  Serial.begin(115200);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
  display.setCursor(0,0);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.clearDisplay();
  display.display();

  //Setup Wifi
  setup_wifi();
  client.setServer(mqtt_server, 1883);

  // Begin DHT readings
  dht.begin();

  delay(1000);
}

void loop() {
  
  if (!client.connected()) {
    //reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 1000) {
    lastMsg = now;

    float newTemp = dht.readTemperature();
    float newHum = dht.readHumidity();

    if (checkBound(newTemp, temp, diff)) {
      temp = newTemp;
      Serial.print("New temperature:");
      Serial.println(String(temp).c_str());
      client.publish(temperature_topic, String(temp).c_str(), true);
    }

    if (checkBound(newHum, hum, diff)) {
      hum = newHum;
      Serial.print("New humidity:");
      Serial.println(String(hum).c_str());
      client.publish(humidity_topic, String(hum).c_str(), true);
    }
  }

  //yield();
}
