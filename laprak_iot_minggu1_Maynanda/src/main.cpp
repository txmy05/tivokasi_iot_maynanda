#include <arduino.h>

void setup() {
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");

  pinMode(23, OUTPUT); // Red LED
  pinMode(22, OUTPUT); // Yellow LED
  pinMode(21, OUTPUT); // Green LED
}

void loop() {
  // Red light ON for 30 seconds
  digitalWrite(23, HIGH);
  digitalWrite(22, LOW);
  digitalWrite(21, LOW);
  delay(30000);

  // Yellow light ON for 5 seconds
  digitalWrite(23, LOW);
  digitalWrite(22, HIGH);
  digitalWrite(21, LOW);
  delay(5000);

  // Green light ON for 20 seconds
  digitalWrite(23, LOW);
  digitalWrite(22, LOW);
  digitalWrite(21, HIGH);
  delay(20000);
}