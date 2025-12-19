#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

// ===== KONFIGURASI PIN =====
#define DHTPIN 27
#define DHTTYPE DHT22

#define LED_HIJAU 2
#define LED_MERAH 15

// ===== KONFIGURASI WIFI =====
const char* ssid = "Shoosahn";
const char* password = "angjaybgtt";

// ===== URL SERVER (LOKAL ATAU NGROK) =====
const char* serverUrl = "http://192.168.1.24:8000/api/data";
// contoh ngrok:
// const char* serverUrl = "https://xxxx.ngrok.io/api/data";

DHT dht(DHTPIN, DHTTYPE);

// ===== TIMER =====
unsigned long lastSend = 0;
const unsigned long interval = 5000; // 5 detik

void connectWiFi() {
  Serial.print("Connecting WiFi");

  WiFi.begin(ssid, password);

  unsigned long startAttempt = millis();

  while (WiFi.status() != WL_CONNECTED &&
         millis() - startAttempt < 15000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi CONNECTED");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi FAILED");
    Serial.print("WiFi Status Code: ");
    Serial.println(WiFi.status());
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(LED_HIJAU, OUTPUT);
  pinMode(LED_MERAH, OUTPUT);

  digitalWrite(LED_HIJAU, LOW);
  digitalWrite(LED_MERAH, LOW);

  dht.begin();

  connectWiFi();
}

void loop() {
  // ===== JAGA WIFI =====
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi terputus, reconnect...");
    connectWiFi();
    delay(2000);
    return;
  }

  // ===== TIMER TANPA DELAY BLOK =====
  if (millis() - lastSend < interval) return;
  lastSend = millis();

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("ERROR: Gagal baca DHT22");
    return;
  }

  Serial.print("Suhu: ");
  Serial.print(temperature);
  Serial.print(" °C | Kelembapan: ");
  Serial.print(humidity);
  Serial.println(" %");

  // ===== LOGIKA LED =====
  if (temperature > 30.0 || humidity > 70.0) {
    digitalWrite(LED_MERAH, HIGH);
    digitalWrite(LED_HIJAU, LOW);
  } else {
    digitalWrite(LED_MERAH, LOW);
    digitalWrite(LED_HIJAU, HIGH);
  }

  // ===== KIRIM KE SERVER =====
  HTTPClient http;
  http.begin(serverUrl);
  http.addHeader("Content-Type", "application/json");

  String payload = "{";
  payload += "\"temperature\":" + String(temperature) + ",";
  payload += "\"humidity\":" + String(humidity);
  payload += "}";

  int httpCode = http.POST(payload);

  Serial.print("HTTP Response Code: ");
  Serial.println(httpCode);

  http.end();
}
