# ESP32 Wi-Fi AC Bulb Brightness Controller

Control AC bulb brightness wirelessly through a web browser
using ESP32 and RobotDyn AC Dimmer Module.

---

## Hardware Required

- ESP32-WROOM board
- RobotDyn AC Dimmer Module 8A (Logic 3.3V/5V)
- AC Bulb (Incandescent recommended)
- Jumper wires
- USB data cable

---

## Wiring — ESP32 to Module (Low Voltage — Safe)

| Module Pin | ESP32 Pin |
|------------|-----------|
| VCC | 3.3V |
| GND | GND |
| PWM | D1 (GPIO5) |
| ZC | D2 (GPIO4) |

---

## Wiring — Module to Bulb (220V — Be Careful)

| Module Pin | Connection |
|------------|-----------|
| AC-IN L | 220V Live (Phase) |
| AC-IN N | 220V Neutral |
| LOAD L | Bulb Live |
| LOAD N | Bulb Neutral |

---

## Circuit Diagram

```
220V Live   → AC-IN L
                        [DIMMER MODULE] → LOAD L → Bulb
220V Neutral→ AC-IN N                  → LOAD N → Bulb

ESP32 3.3V  → VCC
ESP32 GND   → GND
ESP32 D1    → PWM
ESP32 D2    → ZC
```

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

**Step 5** — Kill port if busy:

```bash
sudo fuser -k /dev/ttyACM0
```

**Step 6** — Open Serial Monitor:

```bash
~/.local/bin/pio device monitor --port /dev/ttyACM0 --baud 115200
```

**Step 7** — Press EN/Reset on ESP32

Expected output:
```
=== ESP32 Bulb Controller ===
Dimmer ready
Connecting to YourWiFi.....
Connected!
Open browser: http://192.168.1.4
Web server started!
```

**Step 8** — Switch ON MCB (only after seeing IP in Serial Monitor)

**Step 9** — Open browser:

```
http://192.168.1.4
```

**Step 10** — Move slider → bulb brightness changes!

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

# Check status
curl http://192.168.1.4/status
```

---

## How It Works

```
1. ESP32 powers on
2. Connects to Wi-Fi
3. You open browser → IP address
4. You move slider to 75%
5. Browser calls /set?brightness=75
6. ESP32 reads 75
7. Zero cross interrupt fires
8. TRIAC fires at correct angle
9. Bulb shines at 75% brightness
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
No external library needed.

---

### Issue 2 — GitHub Library Clone Failed

```
fatal: could not read Username for
'https://github.com': No such device or address
```

**Cause:** Git could not connect to GitHub to clone library.

**Solution:** Wrote custom zero cross detection code
from scratch. Removed lib_deps from platformio.ini.

---

### Issue 3 — Port Busy Error

```
UserSideException: Could not exclusively lock port /dev/ttyACM0
```

**Cause:** Another PlatformIO process holding the port.

**Solution:**

```bash
sudo fuser -k /dev/ttyACM0
```

---

### Issue 4 — Old PlatformIO Version

```
Obsolete PIO Core v4.3.4
```

**Solution:**

```bash
pip3 install --upgrade platformio
~/.local/bin/pio run
```

---

### Issue 5 — Gibberish in Serial Monitor

```
␀��␀␀�␀�␀����
```

**Cause:** Baud rate mismatch.

**Solution:**

```bash
~/.local/bin/pio device monitor --port /dev/ttyACM0 --baud 115200
```

---

### Issue 6 — LED Bulb Cutoff at 83-91%

**Cause:** LED bulbs have internal driver circuits
that don't work well with TRIAC dimmers below
certain firing angle.

**Solution:** Adjusted dimValue mapping to working range:

```cpp
void applyBrightness(int pct) {
  pct = constrain(pct, 0, 100);
  if (pct == 0) {
    dimValue = 128;
  } else {
    dimValue = map(pct, 0, 100, 110, 25);
  }
}
```

Use incandescent bulb for full 0-100% range.

---

### Issue 7 — No Sound from Dimmer

Good! No sound means clean wiring and correct connections.
Buzzing sound usually means loose connections or wrong bulb type.

---

## Bulb Type Comparison

| Bulb Type | Dimming Range | Sound | Recommended |
|-----------|---------------|-------|-------------|
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

## Author

jit2eapen | jit2vakathanam@gmail.com
