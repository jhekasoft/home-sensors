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

  // Degree symbol
  const uint8_t celsius[] = {
    SEG_A | SEG_B | SEG_F | SEG_G
  };
#endif

// Errors
// const int ERR_NO_WIFI = 101;
// const int ERR_NO_BLYNC = 110;
// const int ERR_NO_BLYNC_AUTH = 111;
// const int ERR_NO_NTC = 112;
const int ERR_NO_BME = 201;

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASSWORD;

BlynkTimer publishTimer;
BlynkTimer displayTimer;
BlynkTimer syncTimeTimer;

float temperature = 0;
float humidity = 0;
float pressure = 0;
int timeDec = 0;

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
  int TM1637Stage = 1;
#endif

void setup() {
  Serial.begin(115200);
  delay(500);

  // Init displays
  #ifdef I2C1602_USE
    lcd.init();
    lcd.backlight();
    lcd.createChar(0, charCelsius);
  #endif

  #ifdef TM1637_USE
    display.clear();
    display.setBrightness(7, true); // Turn on
  #endif

  showLoader(25);

  // Wi-Fi connection
  Serial.println();
  Serial.println();
  Serial.println("Connecting to Blynk...");
  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

  showLoader(50);

  // Sync time
  syncTime();
  showLoader(75);

  #ifdef BME280_USE
    // BME sensor setup
    bool bmeStatus = bme.begin(BME280_ADDRESS_ALTERNATE);
    if (!bmeStatus) {
      Serial.println("Could not find a valid BME280 sensor, check wiring!");
      displayError(ERR_NO_BME);
    }
  #elif DHT11_USE
    // DHT sensor setup
    dht.begin();
  #endif

  // Custom connected event
  Blynk.logEvent("connected");
  showLoader(100);

  // Read and publish data from sensors every minute
  publishTimer.setInterval(60000L, publishTimerEvent);
  publishTimerEvent();

  // Update display every 5 seconds
  displayTimer.setInterval(5000L, displayTimerEvent);
  displayTimerEvent();

  // Sync time every 5 minutes
  syncTimeTimer.setInterval(300000L, syncTimeTimerEvent);
}

void loop() {
  Blynk.run(); // Initiates Blynk
  publishTimer.run();
  displayTimer.run();
  syncTimeTimer.run();
}

void syncTimeTimerEvent() {
  syncTime();
}

void publishTimerEvent() {
  // Values
  #ifdef BME280_USE
    temperature = bme.readTemperature();
    humidity = bme.readHumidity();
    pressure = bme.readPressure() / 100.0F;
  #elif DHT11_USE
    temperature = dht.readTemperature();
    humidity = dht.readHumidity();
    pressure = 0;
  #endif

  readTime();

  // Publish to Blynk
  Serial.println("Publishing to Blynk...");
  Blynk.virtualWrite(V1, temperature); // For Temperature
  Blynk.virtualWrite(V2, pressure); // For Pressure
  Blynk.virtualWrite(V3, humidity); // For Humidity
  Blynk.virtualWrite(V4, timeDec); // For Time at the device
}

void displayTimerEvent() {
  readTime();

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
    if(TM1637Stage == 1) {
      display.showNumberDecEx(timeDec, 0b00000000, true, 4, 0);
      TM1637Stage = 2;
    } else {
      int temperatureDec = (int)(temperature*10);
      display.showNumberDecEx(temperatureDec, 0b01000000, false, 3);
      display.setSegments(celsius, 1, 3);
      TM1637Stage = 1;
    }
  #endif
}

void readTime() {
  time_t now;
  struct tm* timeinfo;

  time(&now); // Get current epoch time
  timeinfo = localtime(&now); // Convert to local time structure
  timeDec = timeinfo->tm_hour * 100 + timeinfo->tm_min;
}

void syncTime() {
  Serial.println("Syncing time...");

  // Sync time via NTP
  configTime(NTP_TIMEZONE, NTP_SERVER1, NTP_SERVER2);

  Serial.print("Waiting for NTP time synchronization");
  time_t now = time(nullptr);
  while (now < 24 * 3600) { // Wait until time is valid
    delay(100);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("\nTime synchronized");
}

void showLoader(int percent) {
  #ifdef I2C1602_USE
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Loading... " + String(percent) + "%");
  #endif

  #ifdef TM1637_USE
    // Loader segments
    const uint8_t SEG_LOAD1[] = {0b10111000, 0b00111111, 0b01110111, 0b01011110};
    const uint8_t SEG_LOAD2[] = {0b10111000, 0b10111111, 0b01110111, 0b01011110};
    const uint8_t SEG_LOAD3[] = {0b10111000, 0b10111111, 0b11110111, 0b01011110};
    const uint8_t SEG_LOAD4[] = {0b10111000, 0b10111111, 0b11110111, 0b11011110};

    if (percent <= 25) {
      display.setSegments(SEG_LOAD1);
    } else if (percent <= 50) {
      display.setSegments(SEG_LOAD2);
    } else if (percent <= 75) {
      display.setSegments(SEG_LOAD3);
    } else if (percent > 75) {
      display.setSegments(SEG_LOAD4);
    }
  #endif

  Serial.println("Loading... " + String(percent) + "%");
}

void displayError(int errorCode) {
  #ifdef I2C1602_USE
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Error:");
    lcd.setCursor(0, 1);
    lcd.print(errorCode);
  #endif

  #ifdef TM1637_USE
    const uint8_t SEG_ERR[] = {0b01111001};
    display.clear();
    display.setSegments(SEG_ERR, 1, 0);
    display.showNumberDec(errorCode, true, 3, 1);
  #endif

  delay(1000);
}
