#include <Arduino.h>

#define BLYNK_DEVICE_NAME "Esp32IoT"
#define BLYNK_PRINT Serial

#define BLYNK_AUTH_TOKEN "vfOsZBVUg6Iz_N7-Foy0ZsknnaTOcMpR"
#define BLYNK_TEMPLATE_ID "TMPL65IBPXVXd"
#define BLYNK_TEMPLATE_NAME "Template1"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHTesp.h> // Library untuk DHT

char auth[] = BLYNK_AUTH_TOKEN; // Auth Token

char ssid[] = "Wokwi-GUEST"; // Nama hotspot yang digunakan
char pass[] = ""; // Password hotspot yang digunakan

const int DHT_PIN = 15;
byte LED_R = 26; // LED indikator

DHTesp dht;
BlynkTimer timer;

// Fungsi untuk menyinkronkan status terakhir saat ESP32 terhubung ke Blynk
BLYNK_CONNECTED() {
    Blynk.syncVirtual(V2); // Mengambil status terakhir dari switch di V2
}

// Function untuk pengiriman sensor ke Blynk
void sendSensor()
{
    TempAndHumidity data = dht.getTempAndHumidity();

    // Cek apakah data valid
    if (isnan(data.temperature) || isnan(data.humidity)) {
        Serial.println("Error reading from DHT sensor!");
        return; // Keluar dari fungsi jika data tidak valid
    }

    // Menampilkan temperature dan kelembaban pada Serial Monitor
    Serial.print("Temperature: ");
    Serial.print(data.temperature);
    Serial.println("°C");

    Serial.print("Humidity: ");
    Serial.print(data.humidity);
    Serial.println("%");

    // Mengirimkan data ke Virtual Pin yang sesuai
    Blynk.virtualWrite(V4, data.temperature); // Temperatur ke V4
    Blynk.virtualWrite(V1, data.humidity);    // Kelembaban ke V1
}

// Fungsi untuk membaca nilai dari Virtual Pin V2 (Switch)
BLYNK_WRITE(V2)
{
    int nilaiBacaIO = param.asInt();
    Serial.print("Switch Value (V2): "); // Debugging: cek nilai switch
    Serial.println(nilaiBacaIO);

    digitalWrite(LED_R, nilaiBacaIO); // Menghidupkan/mematikan LED
    Blynk.virtualWrite(V3, nilaiBacaIO); // Mengirimkan status LED ke V3
}

void setup()
{
    Serial.begin(115200); // Serial monitor menggunakan baudrate 115200
    dht.setup(DHT_PIN, DHTesp::DHT22);
    pinMode(LED_R, OUTPUT);

    Blynk.begin(auth, ssid, pass); // Memulai koneksi Blynk
    Blynk.syncVirtual(V2); // Pastikan switch tidak mati saat ESP32 restart
    timer.setInterval(1000L, sendSensor); // Kirim data setiap 1 detik
}

void loop()
{
    Blynk.run(); // Menjalankan Blynk
    timer.run(); // Menjalankan timer
}
