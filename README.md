# Arduino home sensors

This is project for ESP8266 Wi-Fi module. Working with meteo sensor BME280 +
DHT11 as second.

## Circuit board

### ESP8266 + BME280

![Sketch ESP8266 + BME280](./assets/images/sketch_esp8266_bme280.jpg)

### ESP8266 + DHT11

![Sketch ESP8266 + DHT11](./assets/images/sketch_esp8266_dht11.jpg)

## Run

1. Open sketch (one of `esp8266_*`) in Arduino IDE.
2. Install missing libraries, including `tzapu/WiFiManager`.
3. Copy `config.h.example` to the `config.h` and write your constants.
4. Upload sketch. On first startup, connect to the `WIFIMANAGER_AP_NAME`
   access point and enter your Wi-Fi credentials in its configuration portal.
   WiFiManager saves them to ESP8266 flash and reconnects with them after reboot.
