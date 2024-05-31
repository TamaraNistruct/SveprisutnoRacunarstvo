#include <WiFi.h>
#include <Wire.h>
#include <DHT.h>
#include <PubSubClient.h>

#define DHTPIN 15
#define DHTTYPE DHT22
#define POTENTIOMETER_PIN 34

DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;

PubSubClient client(espClient);
const char* mqtt_user = "user1";
const char* mqtt_password = "pass1";
const char* mqtt_server = "https://d4ea-178-222-230-116.ngrok-free.app"; // IP adresa MQTT broker-a

void initWiFi() {
 WiFi.mode(WIFI_STA);
  WiFi.begin("Wokwi-GUEST", "");
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println(WiFi.status());
    Serial.print('.');
    delay(1000);
  }
  Serial.println("Connected");
  Serial.println(WiFi.status());
  Serial.println(WiFi.localIP());
  Serial.print("RRSI: ");
  Serial.println(WiFi.RSSI());
}

void setup() {
  
  Serial.begin(9600);
  // WiFi
  initWiFi();
  client.setServer(mqtt_server, 5555);
  dht.begin();
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(mqtt_user)) {
      Serial.println("connected");
      // Subscribe to a topic
      client.subscribe("hello/topic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {

 if (!client.connected()) {
    reconnect();
  }
  client.loop();

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  int pressure = analogRead(POTENTIOMETER_PIN);

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Error reading sensor DHT22.");
  } else {
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.print("%\n");
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println("°C");
  }

  Serial.print("Atmospheric pressure: ");
  Serial.print(potValue);

  String payload = "{\"temperature\":";
  payload += temperature;
  payload += ", \"humidity\":";
  payload += humidity;
  payload += ", \"pressure\":";
  payload += pressure;
  payload += "}";

  if (client.publish("hello/topic", payload.c_str())) {
    Serial.println("Message published successfully");
  } else {
    Serial.println("Message publishing failed");
  }

  delay(2000); 
}
