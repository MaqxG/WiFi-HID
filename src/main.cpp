#include <Arduino.h>
#include <FS.h>
#include <Network.h>
#include <AsyncUDP.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include "USB.h"
#include "USBHIDKeyboard.h"
#include "USBHIDMouse.h"
#include "USBHIDConsumerControl.h"
#include "pin_config.h"
#include "display.h"
#include "web_page.h"

// --- Hardware and Protocol Objects ---
USBHIDKeyboard Keyboard;
USBHIDMouse Mouse;
USBHIDConsumerControl ConsumerControl;
WebServer server(80);
DNSServer dnsServer;
DongleDisplay display;

// --- Network Configuration ---
const char* AP_SSID = "ESP32-KeyDongle";
const char* AP_PASS = "123454321";
const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);

// --- State Variables ---
int lastClientCount = -1;
unsigned long lastDisplayCheck = 0;
int lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

// --- Helper Functions for Lightweight JSON Parsing ---
String extractJsonString(const String& json, const String& key) {
    int keyPos = json.indexOf("\"" + key + "\"");
    if (keyPos == -1) return "";
    int colonPos = json.indexOf(":", keyPos);
    if (colonPos == -1) return "";
    
    int quoteStart = json.indexOf("\"", colonPos);
    if (quoteStart == -1) return "";
    int quoteEnd = quoteStart + 1;
    while (quoteEnd < json.length()) {
        if (json.charAt(quoteEnd) == '"' && json.charAt(quoteEnd - 1) != '\\') {
            break;
        }
        quoteEnd++;
    }
    if (quoteEnd >= json.length()) return "";
    
    String val = json.substring(quoteStart + 1, quoteEnd);
    // Unescape common characters
    val.replace("\\\"", "\"");
    val.replace("\\\\", "\\");
    val.replace("\\n", "\n");
    val.replace("\\r", "\r");
    val.replace("\\t", "\t");
    return val;
}

int extractJsonInt(const String& json, const String& key, int defaultVal = 0) {
    int keyPos = json.indexOf("\"" + key + "\"");
    if (keyPos == -1) return defaultVal;
    int colonPos = json.indexOf(":", keyPos);
    if (colonPos == -1) return defaultVal;
    int start = colonPos + 1;
    while (start < json.length() && (json.charAt(start) == ' ' || json.charAt(start) == '\t')) start++;
    int end = start;
    if (json.charAt(end) == '-') end++;
    while (end < json.length() && isDigit(json.charAt(end))) end++;
    if (start == end) return defaultVal;
    return json.substring(start, end).toInt();
}

bool extractJsonBool(const String& json, const String& key, bool defaultVal = false) {
    int keyPos = json.indexOf("\"" + key + "\"");
    if (keyPos == -1) return defaultVal;
    int colonPos = json.indexOf(":", keyPos);
    if (colonPos == -1) return defaultVal;
    String sub = json.substring(colonPos + 1, colonPos + 10);
    sub.toLowerCase();
    if (sub.indexOf("true") != -1) return true;
    if (sub.indexOf("false") != -1) return false;
    return defaultVal;
}

uint8_t resolveKeyCode(const String& keyName) {
    String k = keyName;
    k.toLowerCase();
    k.trim();
    if (k == "enter" || k == "return") return KEY_RETURN;
    if (k == "esc" || k == "escape") return KEY_ESC;
    if (k == "backspace") return KEY_BACKSPACE;
    if (k == "tab") return KEY_TAB;
    if (k == "space") return ' ';
    if (k == "delete" || k == "del") return KEY_DELETE;
    if (k == "up") return KEY_UP_ARROW;
    if (k == "down") return KEY_DOWN_ARROW;
    if (k == "left") return KEY_LEFT_ARROW;
    if (k == "right") return KEY_RIGHT_ARROW;
    if (k == "home") return KEY_HOME;
    if (k == "end") return KEY_END;
    if (k == "pageup" || k == "pgup") return KEY_PAGE_UP;
    if (k == "pagedown" || k == "pgdn") return KEY_PAGE_DOWN;
    if (k == "insert" || k == "ins") return KEY_INSERT;
    if (k == "capslock") return KEY_CAPS_LOCK;
    if (k == "printscreen" || k == "prtscn") return KEY_PRINT_SCREEN;
    if (k == "f1") return KEY_F1;
    if (k == "f2") return KEY_F2;
    if (k == "f3") return KEY_F3;
    if (k == "f4") return KEY_F4;
    if (k == "f5") return KEY_F5;
    if (k == "f6") return KEY_F6;
    if (k == "f7") return KEY_F7;
    if (k == "f8") return KEY_F8;
    if (k == "f9") return KEY_F9;
    if (k == "f10") return KEY_F10;
    if (k == "f11") return KEY_F11;
    if (k == "f12") return KEY_F12;
    if (k.length() == 1) return (uint8_t)k.charAt(0);
    return 0;
}

void executeKeyWithModifiers(uint8_t keyCode, bool ctrl, bool shift, bool alt, bool gui) {
    if (ctrl)  Keyboard.press(KEY_LEFT_CTRL);
    if (shift) Keyboard.press(KEY_LEFT_SHIFT);
    if (alt)   Keyboard.press(KEY_LEFT_ALT);
    if (gui)   Keyboard.press(KEY_LEFT_GUI);
    
    if (keyCode != 0) {
        Keyboard.press(keyCode);
    }
    delay(20);
    Keyboard.releaseAll();
}

void executeShortcut(const String& sc) {
    String s = sc;
    s.toLowerCase();
    s.trim();
    
    if (s == "copy") {
        executeKeyWithModifiers('c', true, false, false, false);
    } else if (s == "paste") {
        executeKeyWithModifiers('v', true, false, false, false);
    } else if (s == "cut") {
        executeKeyWithModifiers('x', true, false, false, false);
    } else if (s == "undo") {
        executeKeyWithModifiers('z', true, false, false, false);
    } else if (s == "select_all") {
        executeKeyWithModifiers('a', true, false, false, false);
    } else if (s == "save") {
        executeKeyWithModifiers('s', true, false, false, false);
    } else if (s == "find") {
        executeKeyWithModifiers('f', true, false, false, false);
    } else if (s == "win_r") {
        executeKeyWithModifiers('r', false, false, false, true);
    } else if (s == "win_l") {
        executeKeyWithModifiers('l', false, false, false, true);
    } else if (s == "win_d") {
        executeKeyWithModifiers('d', false, false, false, true);
    } else if (s == "win_e") {
        executeKeyWithModifiers('e', false, false, false, true);
    } else if (s == "alt_tab") {
        executeKeyWithModifiers(KEY_TAB, false, false, true, false);
    } else if (s == "alt_f4") {
        executeKeyWithModifiers(KEY_F4, false, false, true, false);
    } else if (s == "taskmgr") {
        executeKeyWithModifiers(KEY_ESC, true, true, false, false); // Ctrl+Shift+Esc
    } else if (s == "ctrl_alt_del") {
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.press(KEY_LEFT_ALT);
        Keyboard.press(KEY_DELETE);
        delay(30);
        Keyboard.releaseAll();
    } else if (s == "printscreen") {
        Keyboard.write(KEY_PRINT_SCREEN);
    }
}

// --- DuckyScript Parser / Macro Interpreter ---
void executeScript(const String& script) {
    int start = 0;
    while (start < script.length()) {
        int nextLine = script.indexOf('\n', start);
        if (nextLine == -1) nextLine = script.length();
        String line = script.substring(start, nextLine);
        line.trim();
        start = nextLine + 1;

        if (line.length() == 0 || line.startsWith("REM")) continue;

        if (line.startsWith("STRING ")) {
            String str = line.substring(7);
            Keyboard.print(str);
        } else if (line.startsWith("DELAY ")) {
            int ms = line.substring(6).toInt();
            if (ms > 0 && ms < 10000) delay(ms);
        } else if (line.equalsIgnoreCase("ENTER") || line.equalsIgnoreCase("RETURN")) {
            Keyboard.write(KEY_RETURN);
        } else if (line.equalsIgnoreCase("TAB")) {
            Keyboard.write(KEY_TAB);
        } else if (line.equalsIgnoreCase("ESC") || line.equalsIgnoreCase("ESCAPE")) {
            Keyboard.write(KEY_ESC);
        } else if (line.startsWith("GUI ") || line.startsWith("WINDOWS ")) {
            int spaceIdx = line.indexOf(' ');
            String k = line.substring(spaceIdx + 1);
            k.trim();
            if (k.length() > 0) {
                executeKeyWithModifiers((uint8_t)k.charAt(0), false, false, false, true);
            }
        } else if (line.startsWith("CTRL ") || line.startsWith("CONTROL ")) {
            int spaceIdx = line.indexOf(' ');
            String k = line.substring(spaceIdx + 1);
            k.trim();
            if (k.length() > 0) {
                executeKeyWithModifiers((uint8_t)k.charAt(0), true, false, false, false);
            }
        } else if (line.startsWith("ALT ")) {
            int spaceIdx = line.indexOf(' ');
            String k = line.substring(spaceIdx + 1);
            k.trim();
            if (k.equalsIgnoreCase("F4")) {
                executeKeyWithModifiers(KEY_F4, false, false, true, false);
            } else if (k.length() > 0) {
                executeKeyWithModifiers((uint8_t)k.charAt(0), false, false, true, false);
            }
        }
        delay(15);
    }
}

// --- HTTP Route Handlers ---
void handleRoot() {
    server.send_P(200, "text/html", INDEX_HTML);
}

void handleCaptivePortal() {
    server.sendHeader("Location", "http://192.168.4.1/", true);
    server.send(302, "text/plain", "Redirecting to controller...");
}

void handleApiType() {
    if (!server.hasArg("plain")) {
        server.send(400, "application/json", "{\"error\":\"Missing body\"}");
        return;
    }
    String body = server.arg("plain");
    String text = extractJsonString(body, "text");
    bool enter = extractJsonBool(body, "enter", false);

    if (text.length() > 0) {
        Keyboard.print(text);
        if (enter) {
            delay(10);
            Keyboard.write(KEY_RETURN);
        }
        String logText = "Type: \"" + text + "\"";
        display.updateStatus(AP_SSID, apIP.toString().c_str(), WiFi.softAPgetStationNum(), logText.c_str());
    }

    server.send(200, "application/json", "{\"status\":\"ok\"}");
}

void handleApiKey() {
    if (!server.hasArg("plain")) {
        server.send(400, "application/json", "{\"error\":\"Missing body\"}");
        return;
    }
    String body = server.arg("plain");
    String key = extractJsonString(body, "key");

    if (key.equalsIgnoreCase("release_all")) {
        Keyboard.releaseAll();
        ConsumerControl.release();
        display.updateStatus(AP_SSID, apIP.toString().c_str(), WiFi.softAPgetStationNum(), "Released All");
        server.send(200, "application/json", "{\"status\":\"ok\"}");
        return;
    }

    bool ctrl = (body.indexOf("\"ctrl\"") != -1);
    bool shift = (body.indexOf("\"shift\"") != -1);
    bool alt = (body.indexOf("\"alt\"") != -1);
    bool gui = (body.indexOf("\"gui\"") != -1);

    uint8_t keyCode = resolveKeyCode(key);
    if (keyCode != 0) {
        executeKeyWithModifiers(keyCode, ctrl, shift, alt, gui);
        String act = "Key: [" + key + "]";
        display.updateStatus(AP_SSID, apIP.toString().c_str(), WiFi.softAPgetStationNum(), act.c_str());
    }

    server.send(200, "application/json", "{\"status\":\"ok\"}");
}

void handleApiShortcut() {
    if (!server.hasArg("plain")) {
        server.send(400, "application/json", "{\"error\":\"Missing body\"}");
        return;
    }
    String body = server.arg("plain");
    String sc = extractJsonString(body, "shortcut");

    executeShortcut(sc);
    String act = "Shortcut: " + sc;
    display.updateStatus(AP_SSID, apIP.toString().c_str(), WiFi.softAPgetStationNum(), act.c_str());

    server.send(200, "application/json", "{\"status\":\"ok\"}");
}

void handleApiMouse() {
    if (!server.hasArg("plain")) {
        server.send(400, "application/json", "{\"error\":\"Missing body\"}");
        return;
    }
    String body = server.arg("plain");
    String click = extractJsonString(body, "click");
    int dx = extractJsonInt(body, "dx", 0);
    int dy = extractJsonInt(body, "dy", 0);
    int wheel = extractJsonInt(body, "wheel", 0);

    if (click.length() > 0) {
        if (click == "left") Mouse.click(MOUSE_LEFT);
        else if (click == "right") Mouse.click(MOUSE_RIGHT);
        else if (click == "middle") Mouse.click(MOUSE_MIDDLE);
    } else if (dx != 0 || dy != 0 || wheel != 0) {
        Mouse.move((int8_t)constrain(dx, -127, 127), (int8_t)constrain(dy, -127, 127), (int8_t)constrain(wheel, -127, 127));
    }

    server.send(200, "application/json", "{\"status\":\"ok\"}");
}

void handleApiMedia() {
    if (!server.hasArg("plain")) {
        server.send(400, "application/json", "{\"error\":\"Missing body\"}");
        return;
    }
    String body = server.arg("plain");
    String action = extractJsonString(body, "action");

    if (action == "vol_up") {
        ConsumerControl.press(CONSUMER_CONTROL_VOLUME_INCREMENT);
    } else if (action == "vol_down") {
        ConsumerControl.press(CONSUMER_CONTROL_VOLUME_DECREMENT);
    } else if (action == "mute") {
        ConsumerControl.press(CONSUMER_CONTROL_MUTE);
    } else if (action == "play_pause") {
        ConsumerControl.press(CONSUMER_CONTROL_PLAY_PAUSE);
    } else if (action == "next") {
        ConsumerControl.press(CONSUMER_CONTROL_SCAN_NEXT);
    } else if (action == "prev") {
        ConsumerControl.press(CONSUMER_CONTROL_SCAN_PREVIOUS);
    }
    delay(20);
    ConsumerControl.release();

    String act = "Media: " + action;
    display.updateStatus(AP_SSID, apIP.toString().c_str(), WiFi.softAPgetStationNum(), act.c_str());

    server.send(200, "application/json", "{\"status\":\"ok\"}");
}

void handleApiScript() {
    if (!server.hasArg("plain")) {
        server.send(400, "application/json", "{\"error\":\"Missing body\"}");
        return;
    }
    String body = server.arg("plain");
    String script = extractJsonString(body, "script");

    executeScript(script);
    display.updateStatus(AP_SSID, apIP.toString().c_str(), WiFi.softAPgetStationNum(), "Executed Script");

    server.send(200, "application/json", "{\"status\":\"ok\"}");
}

void handleApiFlipScreen() {
    display.toggleRotation();
    server.send(200, "application/json", "{\"status\":\"ok\"}");
}

void handleApiStatus() {
    int clients = WiFi.softAPgetStationNum();
    String resp = "{\"usb\":true,\"clients\":" + String(clients) + ",\"ssid\":\"" + String(AP_SSID) + "\",\"ip\":\"" + apIP.toString() + "\"}";
    server.send(200, "application/json", resp);
}

void setup() {
    // 1. Initialize Onboard Button (BOOT / GPIO 0)
    pinMode(PIN_BUTTON_BOOT, INPUT_PULLUP);

    // 2. Initialize 0.96" ST7735 Display
    display.begin();

    // 3. Initialize USB Composite HID
    Keyboard.begin();
    Mouse.begin();
    ConsumerControl.begin();
    USB.productName("ESP32-S3 Wireless HID Dongle");
    USB.manufacturerName("Espressif");
    USB.begin();

    // 4. Initialize WiFi Access Point
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(apIP, apIP, netMsk);
    WiFi.softAP(AP_SSID, AP_PASS);

    // 5. Start Captive Portal DNS Server
    dnsServer.start(DNS_PORT, "*", apIP);

    // 6. Setup Web Server Endpoints
    server.on("/", HTTP_GET, handleRoot);
    
    // Captive portal detection endpoints
    server.on("/generate_204", HTTP_GET, handleCaptivePortal);
    server.on("/gen_204", HTTP_GET, handleCaptivePortal);
    server.on("/hotspot-detect.html", HTTP_GET, handleCaptivePortal);
    server.on("/connecttest.txt", HTTP_GET, handleCaptivePortal);
    server.on("/ncsi.txt", HTTP_GET, handleCaptivePortal);

    // REST API endpoints
    server.on("/api/type", HTTP_POST, handleApiType);
    server.on("/api/key", HTTP_POST, handleApiKey);
    server.on("/api/shortcut", HTTP_POST, handleApiShortcut);
    server.on("/api/mouse", HTTP_POST, handleApiMouse);
    server.on("/api/media", HTTP_POST, handleApiMedia);
    server.on("/api/script", HTTP_POST, handleApiScript);
    server.on("/api/flip_screen", HTTP_POST, handleApiFlipScreen);
    server.on("/api/status", HTTP_GET, handleApiStatus);

    server.onNotFound(handleCaptivePortal);
    server.begin();

    // Initial Display status
    display.updateStatus(AP_SSID, apIP.toString().c_str(), 0, "Ready");
}

void loop() {
    // Process DNS and HTTP requests
    dnsServer.processNextRequest();
    server.handleClient();

    // Physical BOOT button check for screen rotation toggle
    int reading = digitalRead(PIN_BUTTON_BOOT);
    if (reading != lastButtonState) {
        lastDebounceTime = millis();
    }
    if ((millis() - lastDebounceTime) > debounceDelay) {
        static int buttonState = HIGH;
        if (reading != buttonState) {
            buttonState = reading;
            if (buttonState == LOW) {
                // Button pressed (Active LOW)
                display.toggleRotation();
            }
        }
    }
    lastButtonState = reading;

    // Periodic check for WiFi clients change (every 1 second)
    unsigned long now = millis();
    if (now - lastDisplayCheck > 1000) {
        lastDisplayCheck = now;
        int currentClients = WiFi.softAPgetStationNum();
        if (currentClients != lastClientCount) {
            lastClientCount = currentClients;
            display.updateStatus(AP_SSID, apIP.toString().c_str(), currentClients, nullptr);
        }
    }
}
