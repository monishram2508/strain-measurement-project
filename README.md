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
    → Node-RED Dashboard (local browser)
```

---

## What's in this repo

| File/Folder | Description |
|---|---|
| `firmware/esp32_main.ino` | ESP32 Arduino code — tare, calibration, MQTT publish |
| `simulation/Strain_measurement.asc` | Full LTspice schematic |
| `simulation/ad620.cir` | AD620 SPICE model |
| `dashboard/nodered_flow.json` | Node-RED flow — import directly |
| `docs/report.pdf` | Full IEEE-format project report |

---

## Quickstart

**Dependencies:**
```bash
brew install mosquitto
npm install -g node-red
```

**Run:**
```bash
# Terminal 1 — MQTT broker
echo "listener 1883 0.0.0.0\nallow_anonymous true" > /tmp/mosquitto.conf
/opt/homebrew/sbin/mosquitto -c /tmp/mosquitto.conf

# Terminal 2 — Dashboard
node-red
```

Update `ssid`, `password`, and `mqtt_server` in the firmware, flash to ESP32, then open `http://localhost:1880/ui`.

---

## Dashboard

Two options — both work over MQTT:

### Option 1 — HTML Dashboard (no install needed)
Open `dashboard/index.html` directly in a browser. Shows a live weight readout and a scrolling weight-over-time chart. Update `config.js` with your MQTT broker URL first.

### Option 2 — Node-RED
```bash
npm install -g node-red
node-red
```
Import `dashboard/nodered_flow.json` via hamburger menu → Import, then open `http://localhost:1880/ui`.

---

## Quickstart

**Start MQTT broker:**
```bash
echo "listener 1883 0.0.0.0\nallow_anonymous true" > /tmp/mosquitto.conf
/opt/homebrew/sbin/mosquitto -c /tmp/mosquitto.conf
```

**Flash ESP32:**
Update `WIFI_SSID`, `WIFI_PASSWORD`, and `MQTT_SERVER` in `firmware/esp32_main.ino`, then flash to ESP32.

**Open dashboard:**
Open `dashboard/index.html` in your browser.

___

## Report

A detailed IEEE-format report covering circuit design, calculations, LTspice simulation, calibration methodology, and challenges faced during implementation is available in `docs/report.pdf`.

---

## Authors

[Monishram Selvaraj](https://github.com/monishram2508) (2024102076) · [Neel Rajesh](https://github.com/neel-r-3) (2024102007)  
IIIT Hyderabad — Department of Electronics and Communication Engineering
