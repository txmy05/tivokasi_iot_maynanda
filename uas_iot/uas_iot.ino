#define BLYNK_TEMPLATE_ID "TMPL6VnDP64ZE"
#define BLYNK_TEMPLATE_NAME "uas iot"
#define BLYNK_AUTH_TOKEN "LQ8DzxhiHyVvPPTuAky24WW0m24fOFdO"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Mayyy";
char pass[] = "cantikkk";

#define TRIG_PIN 13
#define ECHO_PIN 27
#define LED_AIR_HABIS 25  // LED fisik

void setup() {
  Serial.begin(115200);
  delay(1000); // Pastikan serial siap

  Serial.println("Memulai program...");

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_AIR_HABIS, OUTPUT);
  digitalWrite(LED_AIR_HABIS, LOW); // LED mati di awal

  Blynk.begin(auth, ssid, pass);

  Serial.println("Terhubung ke WiFi dan Blynk.");
  Serial.println("Mulai membaca jarak air...");
}

void loop() {
  Blynk.run();

  long duration;
  float distance;

  // Mengukur jarak air
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration * 0.034 / 2;

  Serial.print("Jarak air: ");
  Serial.print(distance);
  Serial.println(" cm");

  String statusAir;
  int ledState = 0;

  if (distance < 5) {
    statusAir = "✅ Air Cukup";
    digitalWrite(LED_AIR_HABIS, LOW);
    ledState = 0;
  } else if (distance <= 7) {
    statusAir = "⚠ Hampir Habis";
    digitalWrite(LED_AIR_HABIS, LOW);
    ledState = 0;
  } else {
    statusAir = "🚨 Isi Air!";
    digitalWrite(LED_AIR_HABIS, HIGH);
    ledState = 255;
  }

  Serial.print("Status air: ");
  Serial.println(statusAir);
  Serial.print("LED Virtual (V5): ");
  Serial.println(ledState);

  // Kirim data ke Blynk
  Blynk.virtualWrite(V0, distance);     // Gauge atau Display Jarak
  Blynk.virtualWrite(V1, statusAir);    // Label Status Air
  Blynk.virtualWrite(V5, ledState);     // LED Virtual

  delay(2000);
}
