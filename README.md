# Strain Gauge Load Measurement System

![Platform](https://img.shields.io/badge/platform-ESP32-blue)
![Language](https://img.shields.io/badge/language-Arduino%20C%2B%2B-teal)
![Protocol](https://img.shields.io/badge/protocol-MQTT-orange)
![Simulation](https://img.shields.io/badge/simulation-LTspice-red)

An IoT-enabled weight measurement system using resistive strain gauges on a cantilever beam. Covers the full stack — analog signal conditioning circuit, 24-bit ADC, ESP32 firmware, and a live MQTT dashboard.

---

## Signal Chain

```
Strain Gauges (Half Bridge)
    → Custom Instrumentation Amplifier
    → Inverting Summer + Active LPF (fc = 15.9 Hz)
    → HX711 24-bit ADC
    → ESP32 (WiFi + MQTT)
    → Live Dashboard (browser, MQTT over WebSockets)
```

---

## What's in this repo

| File/Folder | Description |
|---|---|
| `firmware/esp32_main.ino` | ESP32 Arduino code — tare, calibration, MQTT publish |
| `simulation/Strain_measurement.asc` | Full LTspice schematic |
| `simulation/ad620.cir` | AD620 SPICE model |
| `data/calibration.csv` | Empirical calibration data (weight vs. raw HX711 counts) |
| `index.html` / `config.js` | Live MQTT dashboard (also served via GitHub Pages) |
| `docs/strain-measurement-report.pdf` | Full IEEE-format project report |

---

## Quickstart

**Flash the firmware:**
Update `WIFI_SSID`, `WIFI_PASSWORD`, and `MQTT_SERVER` in `firmware/esp32_main.ino`, then flash to the ESP32.

**Run a local MQTT broker** (skip this if using the public broker configured in `config.js`):
```bash
brew install mosquitto
echo "listener 1883 0.0.0.0\nallow_anonymous true" > /tmp/mosquitto.conf
/opt/homebrew/sbin/mosquitto -c /tmp/mosquitto.conf
```

**Open the dashboard:**
Open `index.html` directly in a browser (or visit the GitHub Pages URL). It shows a live weight readout and a scrolling weight-over-time chart over MQTT. Update `config.js` with your broker URL first if not using the default.

---

## Report

A detailed IEEE-format report covering circuit design, calculations, LTspice simulation, calibration methodology, and challenges faced during implementation is available in `docs/strain-measurement-report.pdf`.

---

## Authors

[Monishram Selvaraj](https://github.com/monishram2508) (2024102076) · [Neel Rajesh](https://github.com/neel-r-3) (2024102007)  
IIIT Hyderabad — Department of Electronics and Communication Engineering
