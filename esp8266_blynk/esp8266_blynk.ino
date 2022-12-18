#include "config.h"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#ifdef BME280_USE
  #include <Adafruit_Sensor.h>
  #include <Adafruit_BME280.h>
#elif DHT11_USE
  #include <DHT_U.h>
  #include <DHT.h>
#endif

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASSWORD;

BlynkTimer timer;

#ifdef BME280_USE
  Adafruit_BME280 bme; // I2C BME
#elif DHT11_USE
  DHT dht(DHT_PIN, DHT11); // DHT
#endif

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

  #ifdef BME280_USE
    // BME sensor setup
    bool bmeStatus = bme.begin(BME280_ADDRESS_ALTERNATE);
    if (!bmeStatus) {
      Serial.println("Could not find a valid BME280 sensor, check wiring!");
    }
  #elif DHT11_USE
    // DHT sensor setup
    dht.begin();
  #endif

  // Custom connected event
  Blynk.logEvent("connected");

  // Setup a function to be called every minute
  timer.setInterval(60000L, publishTimerEvent);
  publishTimerEvent();
}

void loop() {
  Blynk.run(); // Initiates Blynk
  timer.run();
}

void publishTimerEvent() {
  // Values
  #ifdef BME280_USE
    float temperature = bme.readTemperature();
    float humidity = bme.readHumidity();
    float pressure = bme.readPressure() / 100.0F;
  #elif DHT11_USE
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    float pressure = 0;
  #endif

  // Publish to Blynk
  Serial.println("Publishing to Blynk...");
  Blynk.virtualWrite(V1, temperature); // For Temperature
  Blynk.virtualWrite(V2, pressure); // For Pressure
  Blynk.virtualWrite(V3, humidity); // For Humidity

  Serial.println();
}
