# WiFi-HID

Turn an **ESP32-S3 USB dongle with a 0.96" LCD** into a wireless keyboard, mouse and media remote for any PC.

Plug the dongle into a computer: it shows up as a standard **USB composite HID device** (keyboard + mouse + consumer control) and, at the same time, opens its own **Wi-Fi access point**. Join that network with your phone, open the dongle's IP address, and a mobile web UI lets you type, press keys, run shortcuts and macros, move the mouse and control media on the PC the dongle is plugged into. No drivers, no app, no internet connection required.

```
 Phone ──Wi-Fi──▶ [ ESP32-S3 dongle ] ──USB HID──▶ PC
 (browser)         AP + web server                 (keyboard / mouse / media)
                   LCD status display
```

## Features

- **USB composite HID**: keyboard, relative mouse and consumer control (media keys) over native USB, plus CDC serial for debugging.
- **Self-hosted Wi-Fi AP** with a captive-portal DNS server, so phones usually open the control page automatically.
- **Mobile web UI**, served from flash with no external assets, so it works fully offline:
  - **Type**: send text (optionally followed by Enter) and quick-command chips.
  - **Keys**: sticky Ctrl/Shift/Alt/Win modifiers, arrows, Home/End/PgUp/PgDn, F1–F12.
  - **Shortcuts**: Win+R, Alt+Tab, Alt+F4, Ctrl+C/V/X/Z/A/S/F, Ctrl+Alt+Del and more, plus a DuckyScript-style script runner.
  - **Mouse & Media**: touchpad, left/middle/right click, scroll, volume, mute, play/pause, next/previous.
- **0.96" ST7735 LCD** (80×160) showing the SSID, IP, connected client count and the last action. The BOOT button (or the web UI) flips the screen 180°.

## Hardware

This project was built and tested with this ESP32-S3 USB dongle with a 0.96" LCD:

**[ESP32-S3 0.96" LCD USB dongle (AliExpress)](https://aliexpress.com/item/1005009024098181.html)**

| Part | Details |
|------|---------|
| MCU | ESP32-S3, 16 MB flash, 8 MB octal PSRAM (N16R8) |
| Display | 0.96" IPS ST7735, 80×160, SPI |
| Form factor | USB pocket dongle (native USB on GPIO19/20) |

Other ESP32-S3 boards with native USB should work too. You would need to adjust the pins in [`include/pin_config.h`](include/pin_config.h) and the display setup in [`include/display.h`](include/display.h).

Pin assignments are in [`include/pin_config.h`](include/pin_config.h):

| Function | GPIO |
|----------|------|
| LCD SCLK / MOSI / CS / DC / RST | 10 / 11 / 12 / 13 / 14 |
| BOOT button | 0 |
| USB D- / D+ | 19 / 20 |

The LCD backlight is wired directly to GND, so it is always on.

## Getting started

### Requirements

- [PlatformIO](https://platformio.org/) (VS Code extension or CLI)
- An ESP32-S3 dongle as described above

### Build and flash

```bash
git clone https://github.com/MaqxG/WiFi-HID.git
cd WiFi-HID
pio run -t upload
```

If the board isn't detected, hold **BOOT** while plugging it in to enter the bootloader, then upload again.

> [!NOTE]
> The firmware runs the S3's native USB in TinyUSB device mode (`ARDUINO_USB_MODE=0`). After flashing, the board re-enumerates as a HID device, so the serial port may change.

### Use it

1. Plug the dongle into the target PC.
2. On your phone, join the Wi-Fi network shown on the LCD (default SSID `ESP32-KeyDongle`).
3. Open **http://192.168.4.1** in a browser, or wait for the captive-portal prompt.
4. Type or tap away. Input appears on the PC immediately.

## Configuration

Edit the constants near the top of [`src/main.cpp`](src/main.cpp):

```cpp
const char* AP_SSID = "ESP32-KeyDongle";
const char* AP_PASS = "123454321";   // at least 8 characters, or "" for an open network
IPAddress apIP(192, 168, 4, 1);
```

> [!WARNING]
> Anyone on this Wi-Fi network can type on the connected PC. **Change the default password** before using the dongle anywhere you don't fully trust.

The default keyboard layout is `en_US`. For another layout, pass one to `Keyboard.begin()`, for example `Keyboard.begin(KeyboardLayout_de_DE);`.

## HTTP API

The web UI is a thin client over a small JSON API, so you can also script the dongle with `curl`:

| Method | Endpoint | Body example |
|--------|----------|--------------|
| POST | `/api/type` | `{"text":"hello","enter":true}` |
| POST | `/api/key` | `{"key":"f5","modifiers":["ctrl"]}` (`"key":"release_all"` releases everything) |
| POST | `/api/shortcut` | `{"shortcut":"alt_tab"}` |
| POST | `/api/mouse` | `{"dx":10,"dy":-5}`, `{"click":"left"}` or `{"wheel":2}` |
| POST | `/api/media` | `{"action":"play_pause"}` (`vol_up`, `vol_down`, `mute`, `next`, `prev`) |
| POST | `/api/script` | `{"script":"GUI r\nDELAY 500\nSTRING notepad\nENTER"}` |
| POST | `/api/flip_screen` | `{}` |
| GET | `/api/status` | |

```bash
curl -X POST http://192.168.4.1/api/type -d '{"text":"Hello from curl","enter":true}'
```

### Script runner

Supported commands: `STRING <text>`, `DELAY <ms>`, `ENTER`, `TAB`, `ESC`, `GUI <key>`, `CTRL <key>`, `ALT <key>`, `REM <comment>`.

## Project layout

```
platformio.ini        Board, USB mode and library configuration
src/main.cpp          Wi-Fi AP, web server, REST API, HID logic
include/pin_config.h  Dongle pinout
include/display.h     LCD status screen
include/web_page.h    Embedded mobile web UI (HTML/CSS/JS)
```

## Troubleshooting

- **`FS.h` / `Network.h` not found while building**: `platformio.ini` already adds the include paths for these Arduino-ESP32 3.x core libraries. If you still see the errors, run `pio run -t clean` and rebuild.
- **Phone says "no internet" and drops the network**: this is normal for a captive portal with no uplink. Choose to stay connected, then open the IP manually.
- **Keys type the wrong characters**: the target PC's layout differs from the firmware's. Change the layout as described under Configuration.
- **Screen is upside down**: press the BOOT button or use *Flip LCD* in the web UI.

## Security and responsible use

This device injects keystrokes into whatever computer it is plugged into. Use it only on machines you own or are authorised to control.

## License

Released under the [MIT License](LICENSE).
