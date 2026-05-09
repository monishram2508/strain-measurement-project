/*
 * Strain Gauge Load Measurement System
 * IIIT Hyderabad — Monishram Selvaraj, Neel Rajesh
 *
 * Hardware:
 *   - HX711 DT  → GPIO 4
 *   - HX711 SCK → GPIO 5
 *   - HX711 VCC → 3.3V
 *   - HX711 GND → GND
 *   - HX711 E+  → VIN (4.5V)
 *   - HX711 E-  → GND
 *   - HX711 A+  → Fixed resistor midpoint
 *   - HX711 A-  → Strain gauge midpoint
 *
 * Dependencies:
 *   - HX711 by Bogdan Necula (Library Manager)
 *   - PubSubClient by Nick O'Leary (Library Manager)
 *
 * Usage:
 *   1. Update WIFI_SSID, WIFI_PASSWORD, MQTT_SERVER below
 *   2. Flash to ESP32
 *   3. Open Serial Monitor at 115200 baud
 *   4. Wait for "Warming up..." — leave untouched for 15-20 min
 *   5. Send 't' to tare
 *   6. Place known calibration weight, send 'c' to calibrate
 *   7. Place any object — weight prints to Serial and MQTT
 */

#include <HX711.h>
#include <WiFi.h>
#include <PubSubClient.h>

// ── Configuration ────────────────────────────────────────────
#define DT_PIN   4
#define SCK_PIN  5

const char* WIFI_SSID     = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
const char* MQTT_SERVER   = "YOUR_LAPTOP_IP";   // e.g. "172.20.10.9"
const int   MQTT_PORT     = 1883;
const char* MQTT_TOPIC    = "strain/weight";

// Known weight used for calibration (grams)
// Replace with whatever object you use to calibrate
const float CALIBRATION_WEIGHT_G = 210.0;  // iPhone 16 Pro

// Averaging — higher = smoother but slower
const int AVERAGE_SAMPLES = 20;
// ─────────────────────────────────────────────────────────────

HX711 scale;
WiFiClient espClient;
PubSubClient mqtt(espClient);

float calibration_factor = 1.0;
long  zero_offset        = 0;
bool  calibrated         = false;

// ── WiFi ─────────────────────────────────────────────────────
void connectWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(" Connected.");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println(" Failed. Running without WiFi.");
  }
}

// ── MQTT ─────────────────────────────────────────────────────
void connectMQTT() {
  if (WiFi.status() != WL_CONNECTED) return;
  mqtt.setServer(MQTT_SERVER, MQTT_PORT);
  int attempts = 0;
  while (!mqtt.connected() && attempts < 5) {
    if (mqtt.connect("ESP32_StrainGauge")) {
      Serial.println("MQTT connected.");
    } else {
      delay(1000);
      attempts++;
    }
  }
  if (!mqtt.connected()) {
    Serial.println("MQTT unavailable. Serial only.");
  }
}

void publishWeight(float weight) {
  if (!mqtt.connected()) return;
  char msg[32];
  dtostrf(weight, 6, 2, msg);
  mqtt.publish(MQTT_TOPIC, msg);
}

// ── Setup ─────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  delay(1000);

  // HX711 init
  scale.begin(DT_PIN, SCK_PIN);
  scale.set_gain(128);

  Serial.println("========================================");
  Serial.println("  Strain Gauge Load Measurement System");
  Serial.println("  IIIT Hyderabad");
  Serial.println("========================================");
  Serial.println("Warming up — leave beam untouched.");
  Serial.println("Send 't' to tare when readings stabilise.");
  Serial.println("Send 'c' after placing calibration weight.");
  Serial.println("----------------------------------------");

  // WiFi + MQTT
  connectWiFi();
  connectMQTT();
}

// ── Loop ──────────────────────────────────────────────────────
void loop() {
  // Keep MQTT alive
  if (mqtt.connected()) mqtt.loop();

  // Handle serial commands
  if (Serial.available()) {
    char cmd = Serial.read();

    // ── Tare ──
    if (cmd == 't' || cmd == 'T') {
      Serial.println("Stabilizing before tare...");
      delay(3000);
      zero_offset = scale.read_average(50);
      Serial.print("Tared. Zero offset = ");
      Serial.println(zero_offset);
      Serial.print("Now place calibration weight (");
      Serial.print(CALIBRATION_WEIGHT_G);
      Serial.println(" g) and send 'c'.");
      calibrated = false;
    }

    // ── Calibrate ──
    else if (cmd == 'c' || cmd == 'C') {
      Serial.println("Reading calibration weight...");
      delay(3000);
      long cal_raw = scale.read_average(50) - zero_offset;
      if (cal_raw == 0) {
        Serial.println("Error: zero reading. Tare first, then place weight.");
      } else {
        calibration_factor = (float)cal_raw / CALIBRATION_WEIGHT_G;
        // Use abs in case beam bends downward
        if (calibration_factor < 0) calibration_factor = -calibration_factor;
        calibrated = true;
        Serial.print("Calibration factor: ");
        Serial.print(calibration_factor);
        Serial.println(" counts/g");
        Serial.println("Ready. Place any object to measure.");
      }
    }
  }

  // ── Continuous reading ────────────────────────────────────
  if (scale.is_ready()) {
    long raw     = scale.read_average(AVERAGE_SAMPLES);
    long adjusted = raw - zero_offset;

    if (calibrated) {
      float weight = (float)adjusted / calibration_factor;
      // Clamp negatives to zero at rest
      if (weight < 0 && weight > -5.0) weight = 0.0;

      Serial.print("Weight: ");
      Serial.print(weight, 1);
      Serial.println(" g");

      publishWeight(weight);
    } else {
      // Before calibration — show raw counts so user can watch stabilisation
      Serial.println(adjusted);
    }
  }

  delay(500);
}
