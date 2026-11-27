#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include "pin_config.h"

class DongleDisplay {
public:
    DongleDisplay() : _tft(&SPI, PIN_LCD_CS, PIN_LCD_DC, PIN_LCD_RST), _rotation(1) {}

    void begin() {
        // Initialize SPI with custom pins from pin_config.h
        SPI.begin(PIN_LCD_SCLK, -1, PIN_LCD_MOSI, PIN_LCD_CS);
        
        // 0.96" 80x160 ST7735 IPS display init
        _tft.initR(INITR_MINI160x80);
        _tft.invertDisplay(true); // IPS panels typically require inversion
        _tft.setRotation(_rotation);
        
        drawBootScreen();
    }

    void toggleRotation() {
        // Toggle between landscape 1 and landscape 3 (180 deg flip)
        _rotation = (_rotation == 1) ? 3 : 1;
        _tft.setRotation(_rotation);
        refresh();
    }

    void drawBootScreen() {
        _tft.fillScreen(ST77XX_BLACK);
        
        // Border header
        _tft.fillRoundRect(2, 2, 156, 18, 3, 0x037F); // Deep cyan/blue
        _tft.setTextColor(ST77XX_WHITE);
        _tft.setTextSize(1);
        _tft.setCursor(18, 7);
        _tft.print("WiFi HID DONGLE");

        // Subtitle
        _tft.setTextColor(ST77XX_CYAN);
        _tft.setCursor(30, 32);
        _tft.print("Initializing...");

        _tft.setTextColor(ST77XX_GREEN);
        _tft.setCursor(15, 52);
        _tft.print("USB HID + AP Mode");
    }

    void updateStatus(const char* ssid, const char* ip, int clients, const char* lastAction) {
        _lastSsid = ssid;
        _lastIp = ip;
        _clientCount = clients;
        if (lastAction != nullptr && strlen(lastAction) > 0) {
            _lastAction = lastAction;
        }
        refresh();
    }

    void refresh() {
        _tft.fillScreen(ST77XX_BLACK);

        // Header Bar (y: 1 to 17)
        _tft.fillRoundRect(1, 1, 158, 16, 2, 0x1113); // Dark slate
        _tft.drawRoundRect(1, 1, 158, 16, 2, 0x041F); // Cyan border
        _tft.setTextColor(0x07FF); // Bright cyan
        _tft.setTextSize(1);
        _tft.setCursor(6, 5);
        _tft.print("WiFi-HID");

        // Client count indicator in header
        _tft.setTextColor(_clientCount > 0 ? ST77XX_GREEN : ST77XX_YELLOW);
        _tft.setCursor(105, 5);
        _tft.printf("CLI: %d", _clientCount);

        // Middle Section: Network Info
        _tft.setTextColor(ST77XX_WHITE);
        _tft.setCursor(6, 22);
        _tft.print("AP: ");
        _tft.setTextColor(0xFDE0); // Gold / warm yellow
        _tft.print(_lastSsid.c_str());

        _tft.setTextColor(ST77XX_WHITE);
        _tft.setCursor(6, 34);
        _tft.print("IP: ");
        _tft.setTextColor(ST77XX_GREEN);
        _tft.print(_lastIp.c_str());

        _tft.setTextColor(ST77XX_WHITE);
        _tft.setCursor(6, 46);
        _tft.print("USB: ");
        _tft.setTextColor(0x07E0); // Bright green
        _tft.print("Composite HID [OK]");

        // Footer Bar: Last Keystroke / Action
        _tft.drawFastHLine(2, 59, 156, 0x39E7); // Divider line
        _tft.setTextColor(0xA51F); // Light magenta/gray
        _tft.setCursor(6, 64);
        _tft.print("Act: ");
        _tft.setTextColor(ST77XX_WHITE);
        
        // Truncate action text if too long to fit width
        String act = _lastAction;
        if (act.length() > 19) {
            act = act.substring(0, 16) + "...";
        }
        _tft.print(act.c_str());
    }

private:
    Adafruit_ST7735 _tft;
    uint8_t _rotation;
    String _lastSsid = "ESP32-KeyDongle";
    String _lastIp = "192.168.4.1";
    int _clientCount = 0;
    String _lastAction = "Ready";
};
