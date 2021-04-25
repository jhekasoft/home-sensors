#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "config.h"

WiFiClient espClient;
PubSubClient mqttClient(espClient);

Adafruit_BME280 bme; // I2C

void setup() {
  Serial.begin(9600);
  delay(500);

  // Wi-Fi connection
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Wi-Fi connected!");
  Serial.print("Got IP: ");
  Serial.println(WiFi.localIP());

  // MQTT setup
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  // mqttClient.setCallback(callback);
  bool mqttStatus;
  mqttStatus = bme.begin(BME280_ADDRESS_ALTERNATE);
  if (!mqttStatus) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
  }
}

void loop() {
  publishValues();
  delay(60000); // Wait 1 minute
}

void publishValues() {
  // MQTT connection
  while (!mqttClient.connected()) {
    Serial.println("Connecting to MQTT...");
    if (mqttClient.connect("ESP8266Client")) {
      Serial.println("Connected");  
    } else {
      Serial.print("Failed with state ");
      Serial.print(mqttClient.state());
      delay(2000);
    }
  }
  mqttClient.loop();

  // Values
  float temperature = bme.readTemperature();
  float pressure = bme.readPressure() / 100.0F;
  float humidity = bme.readHumidity();

  // MQTT publish
  Serial.println("Publishing to MQTT...");
  // Publish retained MQTT message
  char topic[255];
  char payload[255];
  sprintf(
    topic,
    "%s/%s/sensor/%s",
    MQTT_TOPIC_LOCATION, MQTT_TOPIC_SUBLOCATION, MQTT_TOPIC_SENSOR_NAME
  );
  sprintf(
    payload,
    "{\"temperature\":%d.%02d,\"pressure\":%d.%02d,\"humidity\":%d.%02d%}",
    (int)temperature, (int)(temperature*100)%100,
    (int)pressure, (int)(pressure*100)%100,
    (int)humidity, (int)(humidity*100)%100
  );
  Serial.println(topic);
  Serial.println(payload);
  bool publishSuccess = mqttClient.publish(topic, payload, true);
  if (publishSuccess) {
    Serial.println("Published");
  } else {
    Serial.println("Didn't publish!");
  }
  Serial.println();
}
