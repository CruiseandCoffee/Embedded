#include <Adafruit_GC9A01A.h>

#include <Adafruit_GFX.h>

#include <Arduino.h>

#include <SPI.h>

#include <WiFi.h>

#include <math.h>

#include <sys/time.h>

#include <time.h>

  

// ==================================

// WIFI AND TIME

// ==================================

  

const char *WIFI_SSID = "YOUR_WIFI_NAME";

const char *WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

  

// Japan: "JST-9"

// India: "IST-5:30"

const char *TIMEZONE = "JST-9";

  

// ==================================

// DISPLAY PINS

// ==================================

  

#define TFT_SCLK 4

#define TFT_MOSI 6

#define TFT_CS 7

#define TFT_DC 5

#define TFT_RST 3

#define TFT_BL 10

  

#define W 240

#define H 240

#define CX 120

#define CY 120

  

Adafruit_GC9A01A tft(&SPI, TFT_DC, TFT_CS, TFT_RST);

  

// Offscreen buffer

GFXcanvas16 *frame = nullptr;

  

// ==================================

// COLORS

// ==================================

  

#define BLACK 0x0000

#define WHITE 0xFFFF

#define SILVER 0xC638

#define LIGHT 0xEF7D

#define DARK 0x7BCF

  

uint16_t rgb(int r, int g, int b) {

return ((r & 248) << 8) |

((g & 252) << 3) |

(b >> 3);

}

  

// ==================================

// CLOCK VARIABLES

// ==================================

  

unsigned long lastFrame = 0;

unsigned long lastWiFiTry = 0;

unsigned long bootTime = 0;

  

bool ntpStarted = false;

bool timeSynced = false;

  

const uint32_t START_SECONDS =

17 * 3600 + 15 * 60;

  

// ==================================

// GEOMETRY HELPERS

// ==================================

  

float rad(float degrees) {

return degrees * PI / 180.0f;

}

  

int getX(float radius, float a) {

return CX + roundf(radius * sinf(a));

}

  

int getY(float radius, float a) {

return CY - roundf(radius * cosf(a));

}

  

// ==================================

// TEXT

// ==================================

  

void centerText(

const char *message,

int y,

uint16_t color,

int size = 1) {

frame->setFont(NULL);

frame->setTextSize(size);

frame->setTextWrap(false);

frame->setTextColor(color);

  

int width = strlen(message) * 6 * size;

  

frame->setCursor(CX - width / 2, y);

frame->print(message);

}

  

// ==================================

// BLUE SEAMASTER DIAL

// ==================================

  

void drawDial() {

frame->fillScreen(BLACK);

  

// Outer bezel

frame->fillCircle(

CX, CY, 119,

rgb(29, 36, 44));

  

frame->drawCircle(CX, CY, 118, WHITE);

frame->drawCircle(CX, CY, 117, LIGHT);

frame->drawCircle(CX, CY, 115, SILVER);

frame->drawCircle(CX, CY, 113, DARK);

  

// Dark minute ring

frame->fillCircle(

CX, CY, 111,

rgb(5, 20, 41));

  

frame->drawCircle(

CX, CY, 109,

rgb(67, 106, 141));

  

// Deep blue textured center

for (int y = 19; y <= 221; y++) {

int dy = y - CY;

int rr = 101 * 101 - dy * dy;

  

if (rr < 0)

continue;

  

int half = sqrtf(rr);

  

int r = 9;

int g = 47;

int b = 94;

  

// Horizontal stripes

if (y % 7 == 0) {

r = 4;

g = 23;

b = 52;

} else if (y % 7 == 1) {

r = 18;

g = 71;

b = 124;

} else if (y % 7 == 2) {

r = 13;

g = 60;

b = 110;

} else {

int glow = 12 - abs(dy) / 12;

g += glow;

b += glow;

}

  

frame->drawFastHLine(

CX - half,

y,

half * 2 + 1,

rgb(r, g, b));

}

  

frame->drawCircle(

CX, CY, 102,

rgb(38, 81, 115));

}

  

// ==================================

// MINUTE TRACK

// ==================================

  

void drawTrack() {

for (int i = 0; i < 60; i++) {

float a = rad(i * 6.0f);

  

int inner = (i % 5 == 0) ? 101 : 106;

  

frame->drawLine(

getX(inner, a),

getY(inner, a),

getX(109, a),

getY(109, a),

(i % 5 == 0) ? WHITE : DARK);

}

  

// Minute numerals

for (int i = 5; i <= 60; i += 5) {

float a = rad(i * 6.0f);

  

int x = getX(114, a);

int y = getY(114, a);

  

char str[4];

sprintf(str, "%02d", i);

  

frame->setFont(NULL);

frame->setTextSize(1);

frame->setTextColor(LIGHT);

frame->setCursor(x - 6, y - 4);

frame->print(str);

}

}

  

// ==================================

// SILVER HOUR MARKERS

// ==================================

  

void drawMarkers() {

for (int i = 0; i < 12; i++) {

if (i == 6)

continue;

  

float a = rad(i * 30.0f);

  

int inner = (i % 3 == 0) ? 78 : 83;

int outer = 98;

  

int tipX = getX(inner, a);

int tipY = getY(inner, a);

  

int lx = getX(outer - 2, a - 0.055f);

int ly = getY(outer - 2, a - 0.055f);

  

int rx = getX(outer - 2, a + 0.055f);

int ry = getY(outer - 2, a + 0.055f);

  

int ox = getX(outer, a);

int oy = getY(outer, a);

  

frame->fillTriangle(

lx, ly, ox, oy,

tipX, tipY, WHITE);

  

frame->fillTriangle(

ox, oy, rx, ry,

tipX, tipY, SILVER);

}

}

  

// ==================================

// BRANDING

// ==================================

  

void drawBrand() {

// Stylized Omega symbol

frame->drawCircle(CX, 75, 6, WHITE);

frame->fillRect(

CX - 8, 77, 16, 5,

rgb(10, 49, 94));

  

frame->drawLine(114, 81, 114, 84, WHITE);

frame->drawLine(126, 81, 126, 84, WHITE);

frame->drawFastHLine(111, 84, 18, WHITE);

  

centerText("OMEGA", 89, WHITE, 2);

centerText("Seamaster", 111, LIGHT);

  

centerText("CO-AXIAL", 160, LIGHT);

centerText("MASTER CHRONOMETER", 171, LIGHT);

}

  

// ==================================

// DATE WINDOW

// ==================================

  

void drawDate(int day) {

frame->fillRoundRect(

107, 181, 26, 27, 2,

rgb(2, 11, 23));

  

frame->drawRoundRect(

107, 181, 26, 27, 2, DARK);

  

char dateStr[4];

sprintf(dateStr, "%02d", day);

  

centerText(dateStr, 186, WHITE, 2);

}

  

// ==================================

// SILVER HAND

// ==================================

  

void drawHand(

float angle,

int length,

int tail,

int width) {

int tx = getX(length, angle);

int ty = getY(length, angle);

  

int bx = getX(-tail, angle);

int by = getY(-tail, angle);

  

int lx = getX(width, angle + PI / 2);

int ly = getY(width, angle + PI / 2);

  

int rx = getX(width, angle - PI / 2);

int ry = getY(width, angle - PI / 2);

  

frame->fillTriangle(

bx, by, lx, ly, tx, ty, WHITE);

  

frame->fillTriangle(

bx, by, rx, ry, tx, ty, SILVER);

  

frame->drawLine(

CX, CY,

getX(length - 5, angle),

getY(length - 5, angle),

LIGHT);

}

  

// ==================================

// SECONDS HAND

// ==================================

  

void drawSecond(float a) {

int tx = getX(96, a);

int ty = getY(96, a);

  

int bx = getX(-22, a);

int by = getY(-22, a);

  

frame->drawLine(bx, by, tx, ty, DARK);

frame->drawLine(CX, CY, tx, ty, WHITE);

  

// Counterweight

float opposite = a + PI;

  

frame->fillTriangle(

getX(30, opposite),

getY(30, opposite),

getX(19, opposite - 0.14f),

getY(19, opposite - 0.14f),

getX(19, opposite + 0.14f),

getY(19, opposite + 0.14f),

WHITE);

}

  

// ==================================

// WATCH HANDS

// ==================================

  

void drawHands(

int hour,

int minute,

float second) {

float ha = rad(

((hour % 12) +

minute / 60.0f +

second / 3600.0f) *

30.0f);

  

float ma = rad(

(minute + second / 60.0f) * 6.0f);

  

float sa = rad(second * 6.0f);

  

drawHand(ha, 58, 12, 6);

drawHand(ma, 82, 13, 5);

drawSecond(sa);

  

frame->fillCircle(CX, CY, 5, DARK);

frame->fillCircle(CX, CY, 3, WHITE);

}

  

// ==================================

// TIME - ONLINE OR OFFLINE

// ==================================

  

void getClockTime(

int &hour,

int &minute,

float &second,

int &day) {

struct timeval tv;

gettimeofday(&tv, nullptr);

  

if (tv.tv_sec > 1700000000) {

struct tm tmNow;

localtime_r(&tv.tv_sec, &tmNow);

  

hour = tmNow.tm_hour;

minute = tmNow.tm_min;

second = tmNow.tm_sec +

tv.tv_usec / 1000000.0f;

day = tmNow.tm_mday;

  

if (!timeSynced) {

timeSynced = true;

Serial.println("NTP synchronized");

}

} else {

float elapsed =

(millis() - bootTime) / 1000.0f;

  

uint32_t total =

START_SECONDS + (uint32_t)elapsed;

  

hour = (total / 3600) % 24;

minute = (total / 60) % 60;

second = (total % 60) +

(elapsed - floorf(elapsed));

day = 9;

}

}

  

// ==================================

// DRAW FRAME IN MEMORY

// ==================================

  

void renderWatch() {

int hour, minute, day;

float second;

  

getClockTime(hour, minute, second, day);

  

// IMPORTANT:

// Everything below draws to RAM,

// NOT directly to the TFT.

drawDial();

drawTrack();

drawMarkers();

drawBrand();

drawDate(day);

drawHands(hour, minute, second);

  

// Transfer completed frame to TFT.

// No fillScreen or blackout on TFT.

tft.drawRGBBitmap(

0, 0,

frame->getBuffer(),

W, H);

  

static int loggedSecond = -1;

int sec = (int)second;

  

if (sec != loggedSecond) {

loggedSecond = sec;

Serial.printf(

"%02d:%02d:%02d | %s\n",

hour, minute, sec,

timeSynced ? "NTP" : "DEMO");

}

}

  

// ==================================

// WIFI

// ==================================

  

void connectWiFi() {

WiFi.mode(WIFI_STA);

WiFi.setSleep(false);

  

WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  

lastWiFiTry = millis();

Serial.println("Connecting WiFi...");

}

  

void updateWiFi() {

if (WiFi.status() == WL_CONNECTED) {

if (!ntpStarted) {

configTzTime(

TIMEZONE,

"pool.ntp.org",

"time.google.com");

  

ntpStarted = true;

Serial.println("NTP started");

}

} else if (

millis() - lastWiFiTry >= 20000UL) {

connectWiFi();

}

}

  

// ==================================

// SETUP

// ==================================

  

void setup() {

Serial.begin(115200);

delay(300);

  

pinMode(TFT_BL, OUTPUT);

digitalWrite(TFT_BL, HIGH);

  

SPI.begin(

TFT_SCLK, -1, TFT_MOSI, TFT_CS);

  

tft.begin();

tft.setRotation(0);

  

// Faster SPI transfers

tft.setSPISpeed(40000000);

  

tft.fillScreen(BLACK);

  

// Allocate 240x240 RGB565 framebuffer

// BEFORE starting WiFi.

frame = new GFXcanvas16(W, H);

  

if (frame == nullptr ||

frame->getBuffer() == nullptr) {

Serial.println("FRAME BUFFER ALLOCATION FAILED");

Serial.println("Not enough contiguous RAM");

  

tft.setTextColor(WHITE);

tft.setTextSize(2);

tft.setCursor(25, 95);

tft.println("NO BUFFER");

tft.setTextSize(1);

tft.setCursor(25, 125);

tft.println("Check Serial Monitor");

  

while (true) {

delay(1000);

}

}

  

Serial.printf(

"Frame buffer ready: %u bytes\n",

W * H * 2);

  

bootTime = millis();

  

// Draw first frame

renderWatch();

  

// Start WiFi only after buffer exists

connectWiFi();

}

  

// ==================================

// LOOP

// ==================================

  

void loop() {

updateWiFi();

  

// 1 update per second

if (millis() - lastFrame >= 1000UL) {

lastFrame = millis();

renderWatch();

}

  

delay(5);

}
