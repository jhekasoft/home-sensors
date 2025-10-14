#include "config.h"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <PubSubClient.h>
#include <Wire.h>

#include <TZ.h>

#ifdef BME280_USE
  #include <Adafruit_Sensor.h>
  #include <Adafruit_BME280.h>
#elif DHT11_USE
  #include <DHT_U.h>
  #include <DHT.h>
#endif

#ifdef I2C1602_USE
  #include <LiquidCrystal_I2C.h>
  
  // Celsius degree char
  byte charCelsius[8] = {B01110, B01010, B01110, B00000, B00000, B00000, B00000, B00000};
#endif

#ifdef TM1637_USE
  #include <TM1637Display.h>
  #define TM1637_CLK 2
  #define TM1637_DIO 0
#endif

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASSWORD;

BlynkTimer timer;

// NTP servers
const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";

#ifdef BME280_USE
  Adafruit_BME280 bme; // I2C BME
#elif DHT11_USE
  DHT dht(DHT_PIN, DHT11); // DHT
#endif

#ifdef I2C1602_USE
  LiquidCrystal_I2C lcd(0x27, 16, 2);
#endif

#ifdef TM1637_USE
  TM1637Display display = TM1637Display(TM1637_CLK, TM1637_DIO);
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
  
  // NTP
  #define TIME_ZONE PSTR("EET-2EEST,M3.5.0/3,M10.5.0/4")
  configTime(TIME_ZONE, ntpServer1, ntpServer2);
  Serial.print("Waiting for NTP time synchronization");
  time_t now = time(nullptr);
  while (now < 100000) { // Check if time is valid (epoch time should be much larger than this)
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("\nTime synchronized");

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

  #ifdef I2C1602_USE
    lcd.init();
    lcd.backlight();
    lcd.createChar(0, charCelsius);
  #endif

  #ifdef TM1637_USE
    display.clear();
    display.setBrightness(7, true); // Turn on
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

  // NTP
  configTime(TIME_ZONE, ntpServer1, ntpServer2);

  time_t now;
  struct tm* timeinfo;

  time(&now); // Get current epoch time
  timeinfo = localtime(&now); // Convert to local time structure
  int timeDec = 0;
  timeDec = timeinfo->tm_hour * 100 + timeinfo->tm_min;
  Serial.println("TIME!");
  Serial.println(timeDec);

  // Publish to Blynk
  Serial.println("Publishing to Blynk...");
  Blynk.virtualWrite(V1, temperature); // For Temperature
  Blynk.virtualWrite(V2, pressure); // For Pressure
  Blynk.virtualWrite(V3, humidity); // For Humidity

  #ifdef I2C1602_USE
    // Print to I2 16x02 display
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print((float)temperature);
    lcd.write(0);
    lcd.print("C ");
    lcd.print((float)humidity);
    lcd.print("%");
    lcd.setCursor(0, 1);
    lcd.print((float)pressure);
    lcd.print(" hPa");
  #endif

  #ifdef TM1637_USE
    // display.showNumberDec(12);
    int temperatureDec = (int)(temperature*100);
    // display.showNumberDecEx(temperatureDec, 0b01000000, false, 4);
    display.showNumberDecEx(timeDec, 0b01000000, false, 4, 0);
  #endif

  Serial.println();
}
