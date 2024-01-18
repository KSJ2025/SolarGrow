#include <DHT.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ThingSpeak.h>

#define MOISTURE_PIN A0    // Analog pin for moisture sensor
#define PUMP_PIN 14        // Digital pin for water pump

const int moistureThreshold = 500;  // Moisture threshold value

#define DHTPIN 2   // Pin where the DHT11 is connected
#define DHTTYPE DHT11   // DHT sensor type

DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "Ahxmedxx";               // Replace with your network credentials
const char* password = "00000000";

const char* server = "api.thingspeak.com";
const unsigned int httpPort = 80;
const String apiKey = "LU2U7F9VUEWBAVUN";    // Replace with your ThingSpeak API Key

WiFiClient client;

const int AirValue = 790;   // You need to replace this value with Value_1
const int WaterValue = 390; // You need to replace this value with Value_2
const int SensorPin = A0;
int soilMoistureValue = 0;
int soilMoisturePercent = 0;

void setup() {
  Serial.begin(9600);
  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, LOW);  // Initialize the pump as off
  dht.begin();
  
  connectWiFi();
}

void loop() {
  int moistureValue = analogRead(MOISTURE_PIN);
  Serial.print("Moisture level: ");
  Serial.println(moistureValue);


// dry soil  = maximum moistureValue 

  if (moistureValue > moistureThreshold) {
    digitalWrite(PUMP_PIN, LOW);  // Turn on the water pump
    Serial.println("Water Pump ON");
  } else {
    digitalWrite(PUMP_PIN, HIGH);   // Turn off the water pump
    Serial.println("Water Pump OFF");
  }

  delay(1000);

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  soilMoistureValue = analogRead(SensorPin);
  soilMoisturePercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);
  
  Serial.print("Soil Moisture: ");
  Serial.println(soilMoistureValue);
  
  sendToThingSpeak(temperature, humidity, soilMoistureValue, soilMoisturePercent);
}

void connectWiFi() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Connected to WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void sendToThingSpeak(float temperature, float humidity, int soilMoistureValue, int soilMoisturePercent) {
  Serial.println("Sending data to ThingSpeak...");

  if (client.connect(server, httpPort)) {
    String url = "/update?api_key=" + apiKey +
                 "&field1=" + String(temperature) +
                 "&field2=" + String(humidity) +
                 "&field3=" + String(soilMoistureValue) +
                 "&field4=" + String(soilMoisturePercent);

    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + server + "\r\n" +
                 "Connection: close\r\n\r\n");
                 
    Serial.println("Data sent!");
  } else {
    Serial.println("Failed to connect to ThingSpeak");
  }

  delay(1000);
  client.stop();
}
