#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// WiFi and API
const char* ssid = "Wokwi-GUEST";
const char* password = "";
String apiKey = "4e3ff9f85087a3196c469c725af5f9f3";       
String city = "Malang";              
String units = "metric";              
String server = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "&units=" + units + "&appid=" + apiKey;

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Buttons
const int nextButtonPin = 18;
const int prevButtonPin = 19;
int currentPage = 0;

// Data dari server
String temp = "-";
String feelsLike = "-";
String desc = "-";
String humidity = "-";
String clouds = "-";
String pressure = "-";
String precipitation = "-";
String windSpeed = "-";
String windDeg = "-";
String sunrise = "-";
String sunset = "-";

// Timer
unsigned long lastUpdateTime = 0;
const long updateInterval = 60000; // 1 menit

// Fungsi Forward
void getWeatherData();
void displayPage();
String convertUnixTime(long unixTime);

void setup() {
  Serial.begin(115200);

  pinMode(nextButtonPin, INPUT_PULLUP);
  pinMode(prevButtonPin, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Weather Info:");

  WiFi.begin(ssid, password);
  lcd.setCursor(0, 1);
  lcd.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connected!");
  delay(2000);
  lcd.clear();

  getWeatherData(); // Fetch pertama
  displayPage();
}

void loop() {
  if (digitalRead(nextButtonPin) == LOW) {
    currentPage++;
    if (currentPage > 4) currentPage = 0; // Update sesuai jumlah page
    displayPage();
    delay(300);
  }

  if (digitalRead(prevButtonPin) == LOW) {
    currentPage--;
    if (currentPage < 0) currentPage = 4; // Update sesuai jumlah page
    displayPage();
    delay(300);
  }

  if (millis() - lastUpdateTime >= updateInterval) {
    getWeatherData();
    lastUpdateTime = millis();
  }

  delay(100);
}

void getWeatherData() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(server);
    int httpCode = http.GET();

    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println(payload);

      const size_t capacity = 2048;
      DynamicJsonDocument doc(capacity);
      DeserializationError error = deserializeJson(doc, payload);

      if (!error) {
        temp = String(doc["main"]["temp"].as<float>(), 1);
        feelsLike = String(doc["main"]["feels_like"].as<float>(), 1);
        desc = doc["weather"][0]["description"].as<String>();
        humidity = String(doc["main"]["humidity"].as<int>());
        clouds = String(doc["clouds"]["all"].as<int>()) + "%";
        pressure = String(doc["main"]["pressure"].as<int>()) + "hPa";
        windSpeed = String(doc["wind"]["speed"].as<float>(), 1) + "m/s";
        windDeg = String(doc["wind"]["deg"].as<int>()) + "°";
        sunrise = convertUnixTime(doc["sys"]["sunrise"].as<long>());
        sunset = convertUnixTime(doc["sys"]["sunset"].as<long>());

        // Periksa apakah ada hujan
        if (doc.containsKey("rain") && doc["rain"].containsKey("1h")) {
          precipitation = String(doc["rain"]["1h"].as<float>()) + "mm";
        } else {
          precipitation = "0mm";
        }
      } else {
        Serial.println("Failed to parse JSON");
      }

      displayPage();
    } else {
      Serial.println("HTTP Request error");
    }
    http.end();
  } else {
    Serial.println("WiFi disconnected");
  }
}

void displayPage() {
  lcd.clear();
  switch (currentPage) {
    case 0:
      lcd.setCursor(0, 0);
      lcd.print("Temp:" + temp + "C");
      lcd.setCursor(0, 1);
      lcd.print("Feels:" + feelsLike + "C");
      break;
    case 1:
      lcd.setCursor(0, 0);
      lcd.print("Weather:");
      lcd.setCursor(0, 1);
      lcd.print(desc);
      break;
    case 2:
      lcd.setCursor(0, 0);
      lcd.print("Hum:" + humidity + " ");
      lcd.print("Cloud:" + clouds);
      lcd.setCursor(0, 1);
      lcd.print("Press:" + pressure);
      break;
    case 3:
      lcd.setCursor(0, 0);
      lcd.print("Wind:" + windSpeed);
      lcd.setCursor(0, 1);
      lcd.print("Dir:" + windDeg);
      break;
    case 4:
      lcd.setCursor(0, 0);
      lcd.print("Sunrise:" + sunrise);
      lcd.setCursor(0, 1);
      lcd.print("Sunset:" + sunset);
      break;
  }
}

String convertUnixTime(long unixTime) {
  const int timeOffset = 7 * 3600; // WIB (UTC+7)
  unixTime += timeOffset;

  time_t rawtime = unixTime;
  struct tm * ti;
  ti = gmtime(&rawtime);
  char buffer[9];
  sprintf(buffer, "%02d:%02d:%02d", ti->tm_hour, ti->tm_min, ti->tm_sec);
  return String(buffer);
}
