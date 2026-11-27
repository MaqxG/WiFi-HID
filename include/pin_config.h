#pragma once

// =========================================================================
// ESP32-S3 0.96" LCD Pocket Dongle Pinout Configuration
// Derived from hardware schematic: ESP32-S3-0.96LCD.pdf
// =========================================================================

// --- Onboard 0.96" IPS ST7735 LCD (80x160) ---
#define PIN_LCD_SCLK      10  // SPI Clock (SCLK)
#define PIN_LCD_MOSI      11  // SPI Data (MOSI / DIN)
#define PIN_LCD_CS        12  // Chip Select (CE1)
#define PIN_LCD_DC        13  // Data / Command control
#define PIN_LCD_RST       14  // Hardware Reset
// Backlight (LEDK): Connected directly to GND through 10-ohm resistor (always ON when powered).

// --- MicroSD / TF Card Slot (SPI Bus) ---
#define PIN_SD_CS         47  // TF Card CS (CD/D3)
#define PIN_SD_MOSI       18  // TF Card MOSI (CMD)
#define PIN_SD_SCLK       17  // TF Card SCK (CLK)
#define PIN_SD_MISO       16  // TF Card MISO (D0)
#define PIN_SD_D1         15  // TF Card D1 (for 4-bit SD mode)
#define PIN_SD_D2         48  // TF Card D2 (for 4-bit SD mode)

// --- Onboard Buttons ---
#define PIN_BUTTON_BOOT    0  // Tactile button K1 (pulled up, active LOW)

// --- Native ESP32-S3 USB ---
#define PIN_USB_DM        19  // USB D-
#define PIN_USB_DP        20  // USB D+

// --- External Header Pin Breakouts ---
// Group 1:
#define PIN_HEADER_IO1     1
#define PIN_HEADER_IO2     2
#define PIN_HEADER_IO3     3
#define PIN_HEADER_IO4     4
#define PIN_HEADER_IO5     5
#define PIN_HEADER_IO6     6
#define PIN_HEADER_IO7     7
#define PIN_HEADER_IO8     8
#define PIN_HEADER_IO9     9

// Group 2:
#define PIN_HEADER_IO40   40
#define PIN_HEADER_IO41   41
#define PIN_HEADER_IO42   42
#define PIN_HEADER_IO45   45
#define PIN_HEADER_TX0    43  // UART0 TX (U0TXD)
#define PIN_HEADER_RX0    44  // UART0 RX (U0RXD)
