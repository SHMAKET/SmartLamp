# 💡 SmartLamp

Smart Wi-Fi lamp controller based on **ESP32** with a built-in web interface. Control lighting from any device on your local network — no cloud, no app required.

---

## Tech Stack

| Layer | Technology |
|---|---|
| Firmware | C++ / Arduino (PlatformIO) |
| MCU | ESP32C6 |
| Web UI | HTML + Tailwind CSS + JavaScript |
| Build tool | Node.js / npm |
| IDE | VS Code + PlatformIO extension |
| 3D Models | `/3d` folder (enclosure parts) |

---

## Project Structure

```
SmartLamp/
├── src/             # C++ firmware source code
├── src_web/         # Web UI source (Tailwind, JS)
├── data/web/        # Compiled web assets (uploaded to ESP32 flash)
├── 3d/              # 3D printable enclosure models
├── platformio.ini   # PlatformIO project config
├── partitions.csv   # Flash partition table
├── tailwind.config.js
└── package.json
```

---

## Getting Started

### Prerequisites

- [VS Code](https://code.visualstudio.com/) + [PlatformIO extension](https://platformio.org/install/ide?install=vscode)
- [Node.js](https://nodejs.org/) (for building the web UI)

### 1. Clone the repository

```bash
git clone https://github.com/SHMAKET/SmartLamp.git
cd SmartLamp
```

### 2. Build the web interface

```bash
npm install
npm run build
```

This compiles the frontend from `src_web/` into `data/web/`, which gets uploaded to the ESP32's filesystem.

### 3. Configure Wi-Fi credentials

Open the firmware source in `src/` and update your Wi-Fi SSID and password before flashing.

### 4. Flash firmware + filesystem

In PlatformIO (VS Code sidebar or CLI):

```bash
# Build and upload firmware
pio run --target upload

# Upload web files to SPIFFS/LittleFS
pio run --target uploadfs
```

### 5. Access the web UI

After booting, the ESP32 will connect to your Wi-Fi and print its IP to the serial monitor. Open that IP in any browser on the same network.

---

## 3D Printing

Enclosure files are located in the `/3d` folder. Print with PLA or PETG. Check fit before final assembly.

---

## License

This project is licensed under the [GNU General Public License v3.0](LICENSE).
