# ESP32 Wi-Fi Exhaust Fan Speed Controller

Control AC exhaust fan speed wirelessly through a web browser.

## Device Specs
- Type  : AC 220V Exhaust Fan
- Power : 25W
- Speed : 1300 RPM (full)
- Min start speed : 30%

## Wiring — ESP32 to Module
| Module Pin | ESP32 Pin |
|------------|-----------|
| VCC | 5V (VIN) |
| GND | GND |
| PWM | D5 (GPIO14) |
| ZC | D2 (GPIO4) |

## Wiring — Module to Fan
| Module Pin | Connection |
|------------|-----------|
| AC-IN L | 220V Live |
| AC-IN N | 220V Neutral |
| LOAD L | Fan Live |
| LOAD N | Fan Neutral |

## Speed Reference
| Preset | Speed | Est. RPM | Power |
|--------|-------|----------|-------|
| OFF | 0% | 0 RPM | 0W |
| LOW | 40% | ~540 RPM | ~10W |
| MED | 65% | ~845 RPM | ~16W |
| HIGH | 100% | 1300 RPM | 25W |

## Known Issues
- Below 30% fan stalls
- Use 5V (VIN) not 3.3V for VCC
- Port is /dev/ttyACM1 on this laptop

## Author
jit2eapen | jit2vakathanam@gmail.com
