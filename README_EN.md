[![Website](https://img.shields.io/badge/🌐%20website-teplica.denserver.ru-blue?style=flat-square)](https://teplica.denserver.ru)
[![License](https://img.shields.io/badge/license-MIT-green.svg?style=flat-square)](LICENSE)
[![Author](https://img.shields.io/badge/author-DenSawer-orange?style=flat-square&logo=github)](https://github.com/DenSawer)
[![English](https://img.shields.io/badge/EN-English-blue?style=flat-square)](README_EN.md)
[![Русский](https://img.shields.io/badge/RU-Русский-red?style=flat-square)](README.md)

# 🌿 Teplica — Automation and Greenhouse Control Block

A complete IoT system for monitoring and managing a greenhouse.  
Deployed at: **[teplica.denserver.ru](https://teplica.denserver.ru)**

---

## 📦 Project Structure

```
/
├── firmware/              # ESP32 firmware code
│   ├── Teplica/           # Main firmware
│   ├── libraries/         # Third-party libraries
│   └── sd/                # Files for SD card
├── server/
│   ├── backend/           # C++ server (Crow)
│   └── web-interface/     # Web interface: HTML, JS, CSS
├── LICENSE                # MIT License
└── README.md              # This file
```

---

## ⚙️ How the System Works

1. **ESP32** reads data from sensors (temperature, humidity, CO₂, etc.)
2. Sends data as JSON to the API:
   ```
   POST https://teplica.denserver.ru/api/upload/{device_id}
   Content-Type: application/json
   ```
3. **C++ server** (based on Crow) receives the data and stores it in `data_*.json`
4. **Web interface** visualizes the data as graphs

---

## 🔧 Backend

Located in `server/backend/`

- Written in C++ using Crow (lightweight REST framework)
- Handles POST requests from ESP and saves data to JSON files
- Supports HTTPS via nginx (optional config available)

### ▶️ Build

```bash
cd server/backend
mkdir build && cd build
cmake ..
make
./teplica-api
```

---

## 🌐 Web Interface

Located in `server/web-interface/`

- Uses `index.html` + `chart.min.js`
- Connects to `data_{CODE}.json`, displays current and historical data by entered code

---

## 📡 Firmware (ESP32)

Located in `firmware/Teplica/`

- Written for Arduino IDE
- Supports Internet connectivity via Wi-Fi, SIM, and Ethernet
- Uses CO₂ sensors (SCD30, SCD4x), air temperature/humidity sensors (DHT22, BME280, SHT2x, SHT3x, SHT4x), soil temperature (DS18B20), soil moisture (capacitive sensor), and light level (BH1750)
- Uses SD card to store data
- Configuration (Wi-Fi, device ID, API URL) is set in the code

---

## 🌍 Demo

Website: **https://teplica.denserver.ru**  
API Endpoint: `https://teplica.denserver.ru/api/upload/{device_id}`

---

