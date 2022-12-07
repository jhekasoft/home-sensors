#include "config.h"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <DHT_U.h>
#include <DHT.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASSWORD;

BlynkTimer timer;

Adafruit_BME280 bme; // I2C BME
DHT dht(DHT_PIN, DHT11); // DHT

void setup() {
  Serial.begin(115200);
  delay(500);

  // Wi-Fi connection
  Serial.println();
  Serial.println();
  Serial.println("Connecting to Blynk...");
  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

  // BME sensor setup
  bool bmeStatus = bme.begin(BME280_ADDRESS_ALTERNATE);
  if (!bmeStatus) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
  }

  // DHT sensor setup
  dht.begin();

  // Setup a function to be called every minute
  timer.setInterval(60000L, publishTimerEvent);
}

void loop() {
  Serial.println("Loop...");
  Blynk.run(); // Initiates Blynk
  timer.run();
}

void publishTimerEvent() {
  // Values
  float temperature = bme.readTemperature();
  float pressure = bme.readPressure() / 100.0F;
  float humidity = bme.readHumidity();
  float temperature2 = dht.readTemperature();
  float humidity2 = dht.readHumidity();

  // Publish to Blynk
  Serial.println("Publishing to Blynk...");
  Blynk.virtualWrite(V1, temperature); // For Temperature
  Blynk.virtualWrite(V2, pressure); // For Pressure
  Blynk.virtualWrite(V3, humidity); // For Humidity
  Blynk.virtualWrite(V4, temperature2); //For Temperature 2
  Blynk.virtualWrite(V5, humidity2); // For Humidity 2

  Serial.println();
}
