#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <DHT_U.h>
#include <DHT.h>
#include "config.h"

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASSWORD;

Adafruit_BME280 bme; // I2C BME
DHT dht(DHT_PIN, DHT11); // DHT

void setup() {
  Serial.begin(115200);
  delay(500);

  // Wi-Fi connection
  Serial.println();
  Serial.println();
  Serial.print("Connecting to Blynk...");
  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

  Serial.println("Setup 1");

  // Wi-Fi connection
  // Serial.println();
  // Serial.println();
  // Serial.print("Connecting to ");
  // Serial.print(WIFI_SSID);
  // WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(1000);
  //   Serial.print(".");
  // }
  // Serial.println();
  // Serial.println("Wi-Fi connected!");
  // Serial.print("Got IP: ");
  // Serial.println(WiFi.localIP());

  // MQTT setup
  // mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  // mqttClient.setCallback(callback);

  // BME sensor setup
  bool bmeStatus = bme.begin(BME280_ADDRESS_ALTERNATE);
  if (!bmeStatus) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
  }

  Serial.println("Setup 1");

  // DHT sensor setup
  dht.begin();

  Serial.println("Setup 1");
}

void loop() {
  Serial.println("Loop...");
  Blynk.run(); // Initiates Blynk
  publishValues();
  delay(60000); // Wait 1 minute
}

void publishValues() {
  // MQTT connection
  // while (!mqttClient.connected()) {
  //   Serial.println("Connecting to MQTT...");
  //   if (mqttClient.connect("ESP8266Client")) {
  //     Serial.println("Connected");  
  //   } else {
  //     Serial.print("Failed with state ");
  //     Serial.print(mqttClient.state());
  //     delay(2000);
  //   }
  // }
  // mqttClient.loop();

  // Values
  float temperature = bme.readTemperature();
  float pressure = bme.readPressure() / 100.0F;
  float humidity = bme.readHumidity();
  float temperature2 = dht.readTemperature();
  float humidity2 = dht.readHumidity();

  // MQTT publish
  // Serial.println("Publishing to MQTT...");
  // // Publish retained MQTT message
  // char topic[255];
  // char payload[255];
  // sprintf(
  //   topic,
  //   "%s/%s/sensor/%s",
  //   MQTT_TOPIC_LOCATION, MQTT_TOPIC_SUBLOCATION, MQTT_TOPIC_SENSOR_NAME
  // );
  // sprintf(
  //   payload,
  //   "{\"temperature\":%d.%02d,\"pressure\":%d.%02d,\"humidity\":%d.%02d%,\"temperature2\":%d.%02d,\"humidity2\":%d.%02d%}",
  //   (int)temperature, (int)(temperature*100)%100,
  //   (int)pressure, (int)(pressure*100)%100,
  //   (int)humidity, (int)(humidity*100)%100,
  //   (int)temperature2, (int)(temperature2*100)%100,
  //   (int)humidity2, (int)(humidity2*100)%100
  // );
  // Serial.println(topic);
  // Serial.println(payload);
  // bool publishSuccess = mqttClient.publish(topic, payload, true);
  // if (publishSuccess) {
  //   Serial.println("Published");
  // } else {
  //   Serial.println("Didn't publish!");
  // }

  // Publish to Blynk
  Serial.println("Publishing to Blynk...");
//  Blynk.virtualWrite(V1, temperature); // For Temperature
//  Blynk.virtualWrite(V2, pressure); // For Pressure
//  Blynk.virtualWrite(V3, humidity); // For Humidity
//  Blynk.virtualWrite(V4, temperature2); //For Temperature 2
//  Blynk.virtualWrite(V5, humidity2); // For Humidity 2

  Serial.println();
}
