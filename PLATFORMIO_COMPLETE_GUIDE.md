# PlatformIO Complete Guide
### VS Code + Terminal | ESP32 | Ubuntu 22.04
### jit2eapen | jit2vakathanam@gmail.com

---

## Table of Contents

1. [What is PlatformIO](#1-what-is-platformio)
2. [Installation](#2-installation)
3. [Create New Project](#3-create-new-project)
4. [Project Structure](#4-project-structure)
5. [platformio.ini Config](#5-platformioini-config)
6. [Write Code in VS Code](#6-write-code-in-vs-code)
7. [Build the Project](#7-build-the-project)
8. [Upload to ESP32](#8-upload-to-esp32)
9. [Serial Monitor](#9-serial-monitor)
10. [All Terminal Commands](#10-all-terminal-commands)
11. [VS Code Toolbar Buttons](#11-vs-code-toolbar-buttons)
12. [Troubleshooting](#12-troubleshooting)

---

## 1. What is PlatformIO

PlatformIO is an IDE tool for embedded development.

```
PlatformIO = Arduino IDE + better tools + terminal support
```

| Feature | Arduino IDE | PlatformIO |
|---------|-------------|------------|
| File type | .ino | .cpp |
| #include Arduino.h | Not needed | Required |
| Terminal commands | ❌ | ✅ pio run |
| Multiple projects | Hard | Easy |
| Library management | Manual | Automatic |
| VS Code integration | ❌ | ✅ |

---

## 2. Installation

### Install VS Code

```bash
sudo snap install code --classic
```

### Install Python (required by PlatformIO)

```bash
sudo apt install python3 python3-pip python3-venv -y
```

```bash
sudo apt install python-is-python3 -y
```

### Install PlatformIO Extension in VS Code

```
Ctrl + Shift + X → search PlatformIO IDE → Install
```

Wait 3-5 minutes for ESP32 toolchain to download.

### Fix USB permissions (Ubuntu only)

```bash
sudo usermod -a -G dialout $USER
```

> ⚠️ Log out and log back in after this command!

### Verify USB permission

```bash
groups $USER
# You must see "dialout" in the list
```

### Upgrade PlatformIO if old version shows

```bash
pip3 install --upgrade platformio
```

### Use correct PlatformIO version

```bash
~/.local/bin/pio --version
```

Expected:
```
PlatformIO Core, version 6.x.x
```

---

## 3. Create New Project

### Method A — VS Code GUI

```
① Click 👽 alien icon in left sidebar
② Click New Project
③ Fill in:
   Name      : ESP32_PROJECT_NAME
   Board     : Espressif ESP32 Dev Module
   Framework : Arduino
④ Click Finish — wait 2-5 minutes
```

### Method B — Terminal

```bash
mkdir -p ~/Documents/PlatformIO/Projects/ESP32_PROJECT_NAME/src
cd ~/Documents/PlatformIO/Projects/ESP32_PROJECT_NAME
```

Then create platformio.ini manually (see Section 5).

---

## 4. Project Structure

```
ESP32_PROJECT_NAME/
├── platformio.ini        ← board and library config
├── README.md             ← project documentation
├── src/
│   └── main.cpp          ← your firmware code
├── lib/                  ← local libraries (optional)
├── include/              ← header files (optional)
├── test/                 ← unit tests (optional)
└── .pio/                 ← build files (auto-generated, ignore)
```

> Only edit: `platformio.ini`, `src/main.cpp`, `README.md`
> Never touch: `.pio/` folder

---

## 5. platformio.ini Config

### Basic config (most projects)

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

### With external library

```ini
[env:esp32dev]
platform      = espressif32
board         = esp32dev
framework     = arduino
monitor_speed = 115200
upload_speed  = 921600
monitor_port  = /dev/ttyACM0
upload_port   = /dev/ttyACM0
lib_deps      = knolleary/PubSubClient@^2.8
```

### Find your port

```bash
ls /dev/ttyUSB* /dev/ttyACM* 2>/dev/null
```

| Port | Chip |
|------|------|
| `/dev/ttyUSB0` | CP2102 or CH340 |
| `/dev/ttyACM0` | CH9102X or MCU-Link |

---

## 6. Write Code in VS Code

### Open main.cpp

```bash
code ~/Documents/PlatformIO/Projects/ESP32_PROJECT_NAME/src/main.cpp
```

### Basic main.cpp template

```cpp
#include <Arduino.h>       // required in PlatformIO

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("ESP32 Started!");
}

void loop() {
  // your code here
  delay(1000);
}
```

> ⚠️ Always include `#include <Arduino.h>` at the top.
> This is NOT needed in Arduino IDE but REQUIRED in PlatformIO.

### Save file

```
Ctrl + S
```

---

## 7. Build the Project

Build = compile your code into firmware binary.
Does NOT upload to ESP32 — just checks for errors.

### Method A — VS Code

```
Click ✓ checkmark in bottom toolbar
```

### Method B — Terminal

```bash
cd ~/Documents/PlatformIO/Projects/ESP32_PROJECT_NAME
~/.local/bin/pio run
```

### Expected success output

```
Compiling .pio/build/esp32dev/src/main.cpp.o
Linking .pio/build/esp32dev/firmware.elf
RAM:   [=         ]  13.8% (used 45208 bytes from 327680 bytes)
Flash: [======    ]  59.6% (used 780893 bytes from 1310720 bytes)
====== [SUCCESS] Took 8.4 seconds ======
```

### What the output means

| Line | Meaning |
|------|---------|
| `Compiling .cpp.o` | Converting your code to machine code |
| `Linking firmware.elf` | Combining all parts together |
| `RAM: [= ]` | How much RAM your code uses |
| `Flash: [====] ` | How much flash storage your code uses |
| `[SUCCESS]` | Code compiled without errors |

---

## 8. Upload to ESP32

Upload = flash compiled firmware to ESP32 chip.

### Before uploading — check ESP32 is detected

```bash
ls /dev/ttyUSB* /dev/ttyACM* 2>/dev/null
```

Expected:
```
/dev/ttyACM0
```

### Kill any process using the port

```bash
sudo fuser -k /dev/ttyACM0
```

### Method A — VS Code

```
Click → right arrow in bottom toolbar
```

### Method B — Terminal

```bash
cd ~/Documents/PlatformIO/Projects/ESP32_PROJECT_NAME
~/.local/bin/pio run --target upload
```

### Expected success output

```
Connecting........
Chip is ESP32-D0WDQ6
Uploading stub...
Running stub...
Configuring flash size...
Writing at 0x00001000... (100 %)
Hash of data verified.
====== [SUCCESS] Took 5.1 seconds ======
```

### If stuck at Connecting.........

Hold **BOOT button** on ESP32 while uploading.
Release when you see `Connecting` in terminal.

### Build and upload in one command

```bash
~/.local/bin/pio run --target upload
```

> PlatformIO builds first then uploads automatically.

---

## 9. Serial Monitor

Serial Monitor shows debug messages from ESP32.
Use it to see IP address, error messages, sensor values.

### Before opening — kill port if busy

```bash
sudo fuser -k /dev/ttyACM0
```

### Method A — VS Code

```
Click 🔌 plug icon in bottom toolbar
```

Then press **EN/Reset** button on ESP32.

### Method B — Terminal

```bash
~/.local/bin/pio device monitor --port /dev/ttyACM0 --baud 115200
```

Press **EN/Reset** on ESP32.

### Quit Serial Monitor

```
Ctrl + C
```

### If you see gibberish text

```
␀��␀␀�␀�␀����
```

Cause: Baud rate mismatch.

Fix: Always specify baud rate explicitly:

```bash
~/.local/bin/pio device monitor --port /dev/ttyACM0 --baud 115200
```

Also check `Serial.begin()` in your code:

```bash
grep "Serial.begin" src/main.cpp
```

Expected:
```
Serial.begin(115200);
```

---

## 10. All Terminal Commands

### Project commands

```bash
# Build only (no upload)
~/.local/bin/pio run

# Upload only (builds first automatically)
~/.local/bin/pio run --target upload

# Clean build files
~/.local/bin/pio run --target clean

# Build + upload + open monitor (all in one)
~/.local/bin/pio run --target upload && ~/.local/bin/pio device monitor --port /dev/ttyACM0 --baud 115200
```

### Device commands

```bash
# List connected devices
~/.local/bin/pio device list

# Open serial monitor
~/.local/bin/pio device monitor --port /dev/ttyACM0 --baud 115200

# Open serial monitor with filter
~/.local/bin/pio device monitor --port /dev/ttyACM0 --baud 115200 --filter esp32_exception_decoder
```

### Library commands

```bash
# Search for a library
~/.local/bin/pio lib search "DHT sensor"

# Install a library
~/.local/bin/pio lib install "DHT sensor library"

# List installed libraries
~/.local/bin/pio lib list
```

### Project commands

```bash
# Initialize new project in current folder
~/.local/bin/pio project init --board esp32dev

# Show project info
~/.local/bin/pio project config

# Update platforms and libraries
~/.local/bin/pio update
```

### Platform commands

```bash
# List installed platforms
~/.local/bin/pio platform list

# Update ESP32 platform
~/.local/bin/pio platform update espressif32
```

---

## 11. VS Code Toolbar Buttons

Bottom toolbar of VS Code — left to right:

```
✓    →    🔌    ...
```

| Button | Symbol | Action | Terminal equivalent |
|--------|--------|--------|-------------------|
| Build | ✓ | Compile code | `pio run` |
| Upload | → | Flash to ESP32 | `pio run --target upload` |
| Serial Monitor | 🔌 | Open serial output | `pio device monitor` |
| PlatformIO Home | 🏠 | Open PIO dashboard | — |

> ⚠️ Always use these toolbar buttons — NOT the VS Code play button (▶) at top right. That runs code locally on your PC not on ESP32.

---

## 12. Troubleshooting

| Problem | Cause | Fix |
|---------|-------|-----|
| `Arduino.h: No such file` | Using wrong terminal | Use PlatformIO terminal only. Click ✓ button. |
| `pio not recognized` | Using system terminal | Run `~/.local/bin/pio` instead of `pio` |
| `Permission denied on port` | Not in dialout group | `sudo usermod -a -G dialout $USER` then re-login |
| `Port busy` | Another program using port | `sudo fuser -k /dev/ttyACM0` |
| `No such port /dev/ttyACM0` | ESP32 not detected | Try different USB cable. Check `dmesg | tail -20` |
| `Stuck at Connecting.........` | ESP32 not in flash mode | Hold BOOT button while uploading |
| `Obsolete PIO Core v4.3.4` | Old version | `pip3 install --upgrade platformio` |
| `Library not found` | Wrong library name | Search: `~/.local/bin/pio lib search "name"` |
| Gibberish in serial monitor | Baud rate mismatch | Use `--baud 115200` explicitly |
| `Nothing to build` | No main.cpp in src/ | Create `src/main.cpp` with your code |
| `Guru Meditation Error` | Crash in ESP32 | Press EN/Reset. Check code for infinite loops |
| Upload success but no output | Wrong baud rate | Check `Serial.begin(115200)` in code |

---

## Our Projects

| # | Project | Folder | Port |
|---|---------|--------|------|
| 01 | ESP32 LED Controller | EP32_LED_CONTROLLER | /dev/ttyACM0 |
| 02 | ESP32 Bulb Controller | ESP32_BULB_CONTROLLER | /dev/ttyACM0 |

---

## Typical Session Workflow

```bash
# 1. Go to project folder
cd ~/Documents/PlatformIO/Projects/ESP32_PROJECT_NAME

# 2. Edit your code
code src/main.cpp

# 3. Build to check for errors
~/.local/bin/pio run

# 4. Kill port if busy
sudo fuser -k /dev/ttyACM0

# 5. Upload to ESP32
~/.local/bin/pio run --target upload

# 6. Open serial monitor
~/.local/bin/pio device monitor --port /dev/ttyACM0 --baud 115200

# 7. Press EN/Reset on ESP32
# 8. See output in terminal
# 9. Press Ctrl+C to stop monitor

# 10. Push to GitHub
git add .
git commit -m "what you changed"
git push
```

---

*PlatformIO Complete Guide · jit2eapen · Ubuntu 22.04 · ESP32 · VS Code*
