# ESP32 Wi-Fi AC Bulb Brightness Controller

Control AC bulb brightness wirelessly through a web browser
using ESP32 and RobotDyn AC Dimmer Module.

---

## Hardware Required

- ESP32-WROOM board
- RobotDyn AC Dimmer Module 8A (Logic 3.3V/5V)
- AC Bulb (Incandescent recommended)
- Jumper wires

---

## Wiring — ESP32 to Module (Low Voltage Side)

| Module Pin | ESP32 Pin |
|------------|-----------|
| VCC | 3.3V |
| GND | GND |
| PWM | D1 (GPIO5) |
| ZC | D2 (GPIO4) |

---

## Wiring — Module to Bulb (220V Side)

| Module Pin | Connection |
|------------|-----------|
| AC-IN L | 220V Live |
| AC-IN N | 220V Neutral |
| LOAD L | Bulb Live |
| LOAD N | Bulb Neutral |

---

## Project Structure

```
ESP32_BULB_CONTROLLER/
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

**Step 1** — Change Wi-Fi credentials in src/main.cpp:

```cpp
const char* WIFI_SSID     = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
```

**Step 2** — Check ESP32 port:

```bash
ls /dev/ttyUSB* /dev/ttyACM* 2>/dev/null
```

**Step 3** — Build:

```bash
cd ~/Documents/PlatformIO/Projects/ESP32_BULB_CONTROLLER
~/.local/bin/pio run
```

**Step 4** — Upload:

```bash
~/.local/bin/pio run --target upload
```

**Step 5** — Open Serial Monitor:

```bash
sudo fuser -k /dev/ttyACM0
~/.local/bin/pio device monitor --port /dev/ttyACM0 --baud 115200
```

**Step 6** — Press EN/Reset on ESP32 — copy IP address shown

**Step 7** — Switch ON MCB

**Step 8** — Open browser:

```
http://192.168.1.4
```

**Step 9** — Move slider → bulb brightness changes!

---

## Features

- Real-time AC bulb brightness control
- Web-based slider UI
- OFF / FULL / 25% / 50% / 75% presets
- Connection status indicator
- Zero cross detection for smooth dimming
- No external library needed

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
# Set to 50%
curl http://192.168.1.4/set?brightness=50

# Turn off
curl http://192.168.1.4/set?brightness=0

# Full brightness
curl http://192.168.1.4/set?brightness=100
```

---

## Known Issues and Solutions

### Issue 1 — RobotDyn Library Not Found

```
Error: Could not find the package with
'RobotDyn/Dimmer @ ^1.0.0' requirements
```

**Cause:** Library not available in PlatformIO registry.

**Solution:** Removed external library dependency.
Used custom zero cross interrupt code instead.
No external library needed now.

---

### Issue 2 — GitHub library clone failed

```
fatal: could not read Username for
'https://github.com': No such device or address
```

**Cause:** Git could not connect to GitHub to clone library.

**Solution:** Wrote custom zero cross detection code
from scratch. Removed lib_deps from platformio.ini completely.

---

### Issue 3 — LED Bulb Cutoff at 83-91%

**Cause:** LED bulbs have internal driver circuits
that don't work well with TRIAC dimmers below
certain firing angle. TRIAC stops conducting
suddenly causing bulb to turn off.

**Solution:** Adjusted dimValue mapping to working
range only. Used incandescent bulb for full
0-100% smooth dimming range.

```cpp
// Adjusted applyBrightness function
void applyBrightness(int pct) {
  pct = constrain(pct, 0, 100);
  if (pct == 0) {
    dimValue = 128;  // fully off
  } else {
    dimValue = map(pct, 0, 100, 110, 25);
  }
}
```

---

### Issue 4 — Port Busy Error

```
UserSideException: [Errno 11] Could not exclusively
lock port /dev/ttyACM0
```

**Cause:** Another PlatformIO process holding the port.

**Solution:**

```bash
sudo fuser -k /dev/ttyACM0
```

---

### Issue 5 — Old PlatformIO Version

```
Obsolete PIO Core v4.3.4 is used (previous was 6.1.19)
```

**Cause:** Multiple PlatformIO versions installed on system.

**Solution:**

```bash
pip3 install --upgrade platformio
~/.local/bin/pio run
```

---

### Issue 6 — Gibberish in Serial Monitor

```
␀��␀␀�␀�␀����␀��␀␀��
```

**Cause:** Baud rate mismatch between code and monitor.

**Solution:** Always use explicit baud rate:

```bash
~/.local/bin/pio device monitor --port /dev/ttyACM0 --baud 115200
```

---

## Bulb Type Comparison

| Bulb Type | Dimming | Sound | Recommended |
|-----------|---------|-------|-------------|
| Incandescent | 0-100% smooth | Minimal | ✅ Best |
| Halogen | 0-100% smooth | Minimal | ✅ Good |
| Dimmable LED | 10-100% | None | ✅ Good |
| Normal LED | 17-83% only | Buzz | ⚠️ Not ideal |
| CFL | Poor | Loud | ❌ Avoid |

---

## Tech Stack

| Layer | Technology |
|-------|------------|
| Hardware | ESP32-WROOM |
| Dimmer | RobotDyn AC Dimmer 8A |
| Firmware | Arduino Framework |
| IDE | PlatformIO + VS Code |
| Frontend | HTML + CSS + JavaScript |
| Protocol | HTTP WebServer |
| Dimming | Zero Cross Detection (custom) |
| OS | Ubuntu 22.04 |

---

## GitHub Setup

```bash
git init
git add .
git commit -m "ESP32 AC Bulb Controller - Initial commit"
git branch -M main
git remote add origin https://github.com/jit2eapen/EMBEDDED.git
git push -u origin main
```

---

## Author

jit2eapen | jit2vakathanam@gmail.com
