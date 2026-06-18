# ESP32 Wi-Fi LED Brightness Controller

Control LED brightness wirelessly through a web browser
using ESP32 and PWM control.

---

## Hardware Required

- ESP32-WROOM board
- 1x LED
- 1x 220Ω resistor
- Breadboard + jumper wires
- USB data cable

---

## Wiring

```
GPIO2 → 220Ω resistor → LED long leg (+) → LED short leg (-) → GND
```

| ESP32 Pin | Component |
|-----------|-----------|
| GPIO2 | 220Ω resistor |
| Resistor other end | LED long leg (+) |
| LED short leg (-) | GND |

---

## Project Structure

```
ESP32_LED_CONTROLLER/
├── platformio.ini
├── README.md
└── src/
    └── main.cpp
```

---

## platformio.ini

```ini
[env:esp32dev]
platform      = espressif32
board         = esp32dev
framework     = arduino
monitor_speed = 115200
upload_speed  = 921600
monitor_port  = /dev/ttyACM0
upload_port   = /dev/ttyACM0
```

---

## Setup Steps

**Step 1** — Install VS Code:

```bash
sudo snap install code --classic
```

**Step 2** — Install PlatformIO extension in VS Code:

```
Ctrl + Shift + X → search PlatformIO IDE → Install
```

**Step 3** — Fix USB permissions:

```bash
sudo usermod -a -G dialout $USER
```

Log out and log back in after this.

**Step 4** — Create project in PlatformIO:

```
PlatformIO icon → New Project
Name      : ESP32_LED_CONTROLLER
Board     : Espressif ESP32 Dev Module
Framework : Arduino
```

**Step 5** — Change Wi-Fi credentials in src/main.cpp:

```cpp
const char* WIFI_SSID     = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
```

**Step 6** — Check ESP32 port:

```bash
ls /dev/ttyUSB* /dev/ttyACM* 2>/dev/null
```

**Step 7** — Build:

```bash
cd ~/Documents/PlatformIO/Projects/ESP32_LED_CONTROLLER
~/.local/bin/pio run
```

**Step 8** — Upload:

```bash
~/.local/bin/pio run --target upload
```

**Step 9** — Open Serial Monitor:

```bash
sudo fuser -k /dev/ttyACM0
~/.local/bin/pio device monitor --port /dev/ttyACM0 --baud 115200
```

**Step 10** — Press EN/Reset on ESP32 — copy IP address

**Step 11** — Open browser:

```
http://192.168.x.x
```

**Step 12** — Move slider → LED brightness changes!

---

## Features

- Real-time PWM brightness control (0-100%)
- Web-based slider UI
- OFF / FULL / 25% / 50% / 75% presets
- Connection status indicator

---

## API Endpoints

| Endpoint | Description |
|----------|-------------|
| `GET /` | Web UI page |
| `GET /set?brightness=X` | Set brightness 0-100 |
| `GET /status` | Current brightness JSON |

---

## Test from Terminal

```bash
# Set to 75%
curl http://192.168.1.4/set?brightness=75

# Turn off
curl http://192.168.1.4/set?brightness=0

# Full brightness
curl http://192.168.1.4/set?brightness=100

# Check status
curl http://192.168.1.4/status
```

---

## How It Works

```
1. ESP32 powers on
2. Connects to Wi-Fi
3. Gets IP address
4. Hosts web page at that IP
5. You open IP in browser
6. You move slider to 75%
7. Browser calls /set?brightness=75
8. ESP32 reads 75
9. map(75, 0,100, 0,255) = 191
10. ledcWrite(0, 191)
11. LED shines at 75% brightness
```

---

## Known Issues and Solutions

### Issue 1 — No /dev/ttyUSB0 or /dev/ttyACM0

**Cause:** USB driver not installed or charge-only cable.

**Solution:**

```bash
dmesg | tail -20
```

Try a different USB cable. Must be data cable not charge-only.

---

### Issue 2 — Upload Permission Denied

**Cause:** User not in dialout group.

**Solution:**

```bash
sudo usermod -a -G dialout $USER
```

Log out and log back in.

---

### Issue 3 — Upload Stuck at Connecting

**Cause:** ESP32 not in flash mode.

**Solution:** Hold BOOT button on ESP32 while uploading.
Release when Connecting appears in terminal.

---

### Issue 4 — Gibberish in Serial Monitor

```
␀��␀␀�␀�␀����
```

**Cause:** Baud rate mismatch.

**Solution:** Always use explicit baud rate:

```bash
~/.local/bin/pio device monitor --port /dev/ttyACM0 --baud 115200
```

---

### Issue 5 — Port Busy Error

```
UserSideException: Could not exclusively lock port /dev/ttyACM0
```

**Cause:** Another process using the port.

**Solution:**

```bash
sudo fuser -k /dev/ttyACM0
```

---

### Issue 6 — Old PlatformIO Version

```
Obsolete PIO Core v4.3.4
```

**Solution:**

```bash
pip3 install --upgrade platformio
~/.local/bin/pio run
```

---

### Issue 7 — Wi-Fi Not Connecting

**Cause:** Wrong SSID/password or 5GHz network.

**Solution:**
- Check SSID spelling (case-sensitive)
- ESP32 only supports 2.4GHz Wi-Fi
- Check router is 2.4GHz

---

## Tech Stack

| Layer | Technology |
|-------|------------|
| Hardware | ESP32-WROOM |
| Firmware | Arduino Framework |
| IDE | PlatformIO + VS Code |
| Frontend | HTML + CSS + JavaScript |
| Protocol | HTTP WebServer |
| PWM | LEDC peripheral (5000Hz, 8-bit) |
| OS | Ubuntu 22.04 |

---

## Author

jit2eapen | jit2vakathanam@gmail.com
