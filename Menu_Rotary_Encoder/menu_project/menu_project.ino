/*
  =====================================================
   ESP32-S3 | KY-40 Rotary Encoder | SSD1306 OLED
   Menu System: Main Pages + Sub Pages
  =====================================================
  PIN CONNECTIONS:
    OLED SDA  -> GPIO 8
    OLED SCL  -> GPIO 9
    KY-40 CLK -> GPIO 15
    KY-40 DT  -> GPIO 16
    KY-40 SW  -> GPIO 17
  =====================================================
*/

#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include "images.h"

// ─────────────────────────────────────────────
//  OLED
// ─────────────────────────────────────────────
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(
  U8G2_R0,
  /* reset=*/ U8X8_PIN_NONE,
  /* clock=*/ 9,
  /* data= */ 8
);

// ─────────────────────────────────────────────
//  Pins
// ─────────────────────────────────────────────
#define ENC_CLK 15
#define ENC_DT  16
#define ENC_SW  17

// ─────────────────────────────────────────────
//  State
// ─────────────────────────────────────────────
enum MainPage { PAGE_TIME = 0, PAGE_BATTERY, PAGE_SENSOR, PAGE_COUNT };
enum ViewMode { VIEW_MAIN, VIEW_SUB };

MainPage currentPage = PAGE_TIME;
ViewMode currentView = VIEW_MAIN;

const char* pageNames[] = { "TIME", "BATTERY", "SENSOR" };

// ─────────────────────────────────────────────
//  Encoder ISR
// ─────────────────────────────────────────────
volatile int     encoderDelta = 0;
volatile uint8_t lastEncoded  = 0;

void IRAM_ATTR encoderISR() {
  uint8_t clk     = digitalRead(ENC_CLK);
  uint8_t dt      = digitalRead(ENC_DT);
  uint8_t encoded = (clk << 1) | dt;
  uint8_t sum     = (lastEncoded << 2) | encoded;

  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderDelta = 1;
  if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderDelta = -1;

  lastEncoded = encoded;
}

// ─────────────────────────────────────────────
//  Tick counter — 10 ticks = advance main page
// ─────────────────────────────────────────────
#define AUTO_ADVANCE_TICKS 10
int tickCount = 0;

// ─────────────────────────────────────────────
//  Button
// ─────────────────────────────────────────────
bool          lastButtonState = HIGH;
unsigned long lastButtonTime  = 0;
const unsigned long BTN_DEBOUNCE_MS = 50;

// ─────────────────────────────────────────────
//  Serial log
// ─────────────────────────────────────────────
void logState(const char* event) {
  Serial.println("-----------------------------");
  Serial.print("[EVENT]  "); Serial.println(event);
  Serial.print("[PAGE]   "); Serial.println(pageNames[currentPage]);
  Serial.print("[VIEW]   "); Serial.println(currentView == VIEW_MAIN ? "MAIN" : "SUB");
  Serial.print("[TICKS]  "); Serial.println(tickCount);
  Serial.println("-----------------------------");
}

// ─────────────────────────────────────────────
//  Draw — Main
// ─────────────────────────────────────────────
void drawMainTime() {
  u8g2.clearBuffer();
  u8g2.setFontMode(1);
  u8g2.setBitmapMode(1);
  u8g2.setFont(u8g2_font_profont22_tr);
  u8g2.drawStr(43, 59, "TIME");
  u8g2.drawXBMP(51, 10, 30, 30, image_clock_quarters_bits);
  u8g2.sendBuffer();
}

void drawMainBattery() {
  u8g2.clearBuffer();
  u8g2.setFontMode(1);
  u8g2.setBitmapMode(1);
  u8g2.setFont(u8g2_font_profont22_tr);
  u8g2.drawStr(25, 59, "BATTERY");
  u8g2.drawXBMP(39, 9, 48, 32, image_battery_50_2_bits);
  u8g2.sendBuffer();
}

void drawMainSensor() {
  u8g2.clearBuffer();
  u8g2.setFontMode(1);
  u8g2.setBitmapMode(1);
  u8g2.setFont(u8g2_font_profont22_tr);
  u8g2.drawStr(26, 59, "Sensors");
  u8g2.drawXBMP(80, 8, 32, 32, image_weather_temperature_bits);
  u8g2.drawXBMP(28, 8, 38, 32, image_wifi_5_bars_bits);
  u8g2.sendBuffer();
}

// ─────────────────────────────────────────────
//  Draw — Sub
// ─────────────────────────────────────────────
void drawSubTime() {
  u8g2.clearBuffer();
  u8g2.setFontMode(1);
  u8g2.setBitmapMode(1);
  u8g2.setFont(u8g2_font_profont22_tr);
  u8g2.drawStr(18, 39, "10:10 PM");
  u8g2.sendBuffer();
}

void drawSubBattery() {
  u8g2.clearBuffer();
  u8g2.setFontMode(1);
  u8g2.setBitmapMode(1);
  u8g2.drawXBMP(60, 12, 48, 32, image_battery_67_bits);
  u8g2.setFont(u8g2_font_profont22_tr);
  u8g2.drawStr(18, 36, "67");
  u8g2.sendBuffer();
}

void drawSubSensor() {
  u8g2.clearBuffer();
  u8g2.setFontMode(1);
  u8g2.setBitmapMode(1);
  u8g2.setFont(u8g2_font_profont22_tr);
  u8g2.drawStr(7,  20, "TEMP");
  u8g2.drawUTF8(71, 22, "28°C");
  u8g2.drawStr(7,  48, "WIFI");
  u8g2.drawStr(62, 49, "-68dB");
  u8g2.sendBuffer();
}

// ─────────────────────────────────────────────
//  Render dispatcher
// ─────────────────────────────────────────────
void renderDisplay() {
  if (currentView == VIEW_MAIN) {
    switch (currentPage) {
      case PAGE_TIME:    drawMainTime();    break;
      case PAGE_BATTERY: drawMainBattery(); break;
      case PAGE_SENSOR:  drawMainSensor();  break;
    }
  } else {
    switch (currentPage) {
      case PAGE_TIME:    drawSubTime();    break;
      case PAGE_BATTERY: drawSubBattery(); break;
      case PAGE_SENSOR:  drawSubSensor();  break;
    }
  }
}

// ─────────────────────────────────────────────
//  Setup
// ─────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("=============================");
  Serial.println("  ESP32-S3 OLED MENU SYSTEM  ");
  Serial.println("=============================");

  pinMode(ENC_CLK, INPUT_PULLUP);
  pinMode(ENC_DT,  INPUT_PULLUP);
  pinMode(ENC_SW,  INPUT_PULLUP);

  lastEncoded  = (digitalRead(ENC_CLK) << 1) | digitalRead(ENC_DT);
  encoderDelta = 0;

  attachInterrupt(digitalPinToInterrupt(ENC_CLK), encoderISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_DT),  encoderISR, CHANGE);

  u8g2.begin();
  u8g2.clearBuffer();
  renderDisplay();

  logState("STARTUP");
}

// ─────────────────────────────────────────────
//  Loop
// ─────────────────────────────────────────────
void loop() {
  unsigned long now = millis();

  // ── Flush encoder delta when on SUB view ────
  if (currentView == VIEW_SUB) {
    noInterrupts();
    encoderDelta = 0;
    interrupts();
  }

  // ── Button: falling edge only ───────────────
  bool currentButtonState = digitalRead(ENC_SW);

  if (currentButtonState == LOW && lastButtonState == HIGH) {
    if (now - lastButtonTime > BTN_DEBOUNCE_MS) {
      lastButtonTime = now;
      tickCount = 0;

      if (currentView == VIEW_MAIN) {
        currentView = VIEW_SUB;
        Serial.print("[BUTTON] Entering SUB → ");
        Serial.println(pageNames[currentPage]);
      } else {
        currentView = VIEW_MAIN;
        noInterrupts();
        encoderDelta = 0;
        interrupts();
        Serial.print("[BUTTON] Back to MAIN → ");
        Serial.println(pageNames[currentPage]);
      }

      logState("BUTTON PRESS");
      renderDisplay();
    }
  }
  lastButtonState = currentButtonState;

  // ── Encoder: only on MAIN view ──────────────
  if (currentView == VIEW_MAIN) {
    noInterrupts();
    int delta = encoderDelta;
    encoderDelta = 0;
    interrupts();

    if (delta != 0) {
      tickCount++;
      Serial.print("[TICKS]  "); Serial.print(tickCount);
      Serial.print(" / "); Serial.println(AUTO_ADVANCE_TICKS);

      if (tickCount >= AUTO_ADVANCE_TICKS) {
        // Only NOW change the page — not on every tick
        tickCount = 0;
        currentPage = (MainPage)((currentPage + delta + PAGE_COUNT) % PAGE_COUNT);
        Serial.print("[AUTO]   10 ticks → advancing to ");
        Serial.println(pageNames[currentPage]);
        logState("AUTO-ADVANCE PAGE");
        renderDisplay();
      }
      // No else — do nothing to the page on ticks 1-9
    }
  }
}
