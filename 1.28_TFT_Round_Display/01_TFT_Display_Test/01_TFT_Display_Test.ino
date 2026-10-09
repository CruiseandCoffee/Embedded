#include <Adafruit_GC9A01A.h>

#include <Adafruit_GFX.h>

#include <SPI.h>

  

// ESP32-C3 pin configuration

#define TFT_SCLK 4

#define TFT_MOSI 6

#define TFT_CS 7

#define TFT_DC 5

#define TFT_RST 3

#define TFT_BL 10

  

// Create display object using hardware SPI

Adafruit_GC9A01A tft(&SPI, TFT_DC, TFT_CS, TFT_RST);

  

void setup() {

Serial.begin(115200);

  

// Turn on backlight

pinMode(TFT_BL, OUTPUT);

digitalWrite(TFT_BL, HIGH);

  

// Initialize SPI

SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);

  

// Initialize display

tft.begin();

tft.setRotation(0);

  

Serial.println("GC9A01 Display Test Started!");

}

  

void loop() {

// Test 1: RED

Serial.println("Testing RED");

tft.fillScreen(0xF800);

delay(1000);

  

// Test 2: GREEN

Serial.println("Testing GREEN");

tft.fillScreen(0x07E0);

delay(1000);

  

// Test 3: BLUE

Serial.println("Testing BLUE");

tft.fillScreen(0x001F);

delay(1000);

  

// Test 4: Draw circles

Serial.println("Drawing circles");

tft.fillScreen(0x0000);

  

for (int r = 100; r > 10; r -= 10) {

tft.drawCircle(

120, 120, r,

tft.color565(255 - r, r * 2, 255));

delay(100);

}

  

delay(1500);

  

// Test 5: Display text

Serial.println("Testing text");

tft.fillScreen(0x0000);

  

tft.setTextColor(0xFFFF);

tft.setTextSize(2);

tft.setCursor(60, 75);

tft.println("ESP32-C3");

  

tft.setTextColor(0x07E0);

tft.setCursor(48, 115);

tft.println("DISPLAY");

  

tft.setCursor(90, 140);

tft.println("OK!");

  

delay(3000);

}
