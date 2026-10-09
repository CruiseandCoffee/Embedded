#include <Adafruit_GC9A01A.h>

#include <Adafruit_GFX.h>

#include <SPI.h>

#include <math.h>

  

// Display pins - same as before

#define TFT_SCLK 4

#define TFT_MOSI 6

#define TFT_CS 7

#define TFT_DC 5

#define TFT_RST 3

#define TFT_BL 10

  

Adafruit_GC9A01A tft(&SPI, TFT_DC, TFT_CS, TFT_RST);

  

// Colors

#define BLACK 0x0000

#define WHITE 0xFFFF

#define RED 0xF800

#define GREEN 0x07E0

#define YELLOW 0xFFE0

#define CYAN 0x07FF

#define GREY 0x7BEF

  

#define CX 120

#define CY 120

#define MAX_SPEED 260

  

const float START_ANGLE = 135.0f;

const float END_ANGLE = 405.0f;

  

// Small canvas covering the needle and

// digital number without touching dial ticks.

// Coordinates: x=49..190, y=49..190

#define CANVAS_X 49

#define CANVAS_Y 49

#define CANVAS_SIZE 142

  

GFXcanvas16 canvas(CANVAS_SIZE, CANVAS_SIZE);

  

int speed = 0;

int direction = 1;

  

// Convert speed to angle

float speedAngle(int s) {

return (START_ANGLE +

(float)s / MAX_SPEED *

(END_ANGLE - START_ANGLE)) *

PI / 180.0f;

}

  

// Draw static dial ONCE

void drawDial() {

tft.fillScreen(BLACK);

  

tft.drawCircle(CX, CY, 118, GREY);

tft.drawCircle(CX, CY, 116, GREY);

  

for (int s = 0; s <= MAX_SPEED; s += 5) {

float a = speedAngle(s);

  

bool major = (s % 20 == 0);

int r1 = major ? 91 : 99;

int r2 = 108;

  

uint16_t color = CYAN;

  

if (s >= 200)

color = RED;

else if (s >= 140)

color = YELLOW;

  

int x1 = CX + cos(a) * r1;

int y1 = CY + sin(a) * r1;

int x2 = CX + cos(a) * r2;

int y2 = CY + sin(a) * r2;

  

tft.drawLine(x1, y1, x2, y2, color);

}

  

tft.setTextSize(1);

tft.setTextColor(WHITE);

  

for (int s = 0; s <= MAX_SPEED; s += 20) {

float a = speedAngle(s);

  

int x = CX + cos(a) * 77;

int y = CY + sin(a) * 77;

  

String label = String(s);

int width = label.length() * 6;

  

tft.setCursor(x - width / 2, y - 4);

tft.print(label);

}

}

  

// Draw needle inside canvas

void drawNeedle(int s) {

float a = speedAngle(s);

  

int center = CANVAS_SIZE / 2;

  

int x = center + cos(a) * 65;

int y = center + sin(a) * 65;

  

canvas.drawLine(center, center, x, y, RED);

canvas.drawLine(center + 1, center, x + 1, y, RED);

canvas.drawLine(center, center + 1, x, y + 1, RED);

  

canvas.fillCircle(center, center, 6, RED);

canvas.fillCircle(center, center, 3, BLACK);

}

  

// Draw digital readout inside canvas

void drawSpeed(int s) {

uint16_t color = GREEN;

  

if (s >= 200)

color = RED;

else if (s >= 140)

color = YELLOW;

  

int center = CANVAS_SIZE / 2;

  

canvas.setTextSize(3);

canvas.setTextColor(color);

  

String value = String(s);

int width = value.length() * 18;

  

canvas.setCursor(center - width / 2, 92);

canvas.print(value);

  

canvas.setTextSize(2);

canvas.setTextColor(GREY);

canvas.setCursor(center - 24, 123);

canvas.print("km/h");

  

canvas.setTextSize(2);

canvas.setTextColor(CYAN);

canvas.setCursor(center - 30, 12);

canvas.print("SPEED");

}

  

// Prepare and transfer one complete frame

void updateSpeedometer(int s) {

canvas.fillScreen(BLACK);

  

drawNeedle(s);

drawSpeed(s);

  

// Send the completed canvas in one operation.

// No full-screen clearing.

tft.drawRGBBitmap(

CANVAS_X,

CANVAS_Y,

canvas.getBuffer(),

CANVAS_SIZE,

CANVAS_SIZE);

}

  

void setup() {

Serial.begin(115200);

  

pinMode(TFT_BL, OUTPUT);

digitalWrite(TFT_BL, HIGH);

  

SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);

  

tft.begin();

tft.setRotation(0);

  

// Draw outer dial once

drawDial();

  

// Draw initial speed

updateSpeedometer(0);

}

  

void loop() {

updateSpeedometer(speed);

  

speed += direction * 2;

  

if (speed >= MAX_SPEED) {

speed = MAX_SPEED;

direction = -1;

} else if (speed <= 0) {

speed = 0;

direction = 1;

}

  

delay(30);

}
