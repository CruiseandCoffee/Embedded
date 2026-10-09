#include <Adafruit_GC9A01A.h>

#include <Adafruit_GFX.h>

#include <Arduino.h>

#include <ArduinoJson.h>

#include <HTTPClient.h>

#include <SPI.h>

#include <WiFi.h>

#include <WiFiClientSecure.h>

#include <math.h>

  

// Beautiful fonts

#include <Fonts/FreeSans9pt7b.h>

#include <Fonts/FreeSansBold12pt7b.h>

#include <Fonts/FreeSansBold18pt7b.h>

#include <Fonts/FreeSansBold9pt7b.h>

  

// =====================================

// WIFI CONFIGURATION

// =====================================

  

const char *WIFI_SSID = "PUT_WIFI_NAME_HERE";

const char *WIFI_PASSWORD = "";

  

// =====================================

// YOUTUBE CONFIGURATION

// =====================================

  

const char *API_KEY =

"PASTE_YOUR_YOUTUBE_API_KEY_HERE";

  

// YouTube channel lookup handle

const char *CHANNEL_HANDLE = "PUT_CHANNEL_HANDLE_HERE";

  

// Display title

const char *DISPLAY_NAME = "PUT_TEXT_HERE";

  

// =====================================

// TFT DISPLAY PINS

// =====================================

  

#define TFT_SCLK 4

#define TFT_MOSI 6

#define TFT_CS 7

#define TFT_DC 5

#define TFT_RST 3

#define TFT_BL 10

  

// Onboard LED

#define LED_PIN 8

#define LED_ON LOW

#define LED_OFF HIGH

  

Adafruit_GC9A01A tft(

&SPI, TFT_DC, TFT_CS, TFT_RST);

  

// =====================================

// COLORS

// =====================================

  

#define BG 0x0843

#define CARD 0x1107

#define COLOR_BORDER 0x294B

#define WHITE 0xFFFF

#define SOFT 0xCE79

#define MUTED 0x94B3

#define RED 0xFA0B

#define CYAN 0x5EFF

#define GREEN 0x36D3

#define YELLOW 0xFE60

  

// =====================================

// TIMING

// =====================================

  

const unsigned long WIFI_RETRY_MS = 30000UL;

const unsigned long API_REFRESH_MS = 300000UL;

const unsigned long API_RETRY_MS = 30000UL;

const unsigned long LED_INTERVAL = 500UL;

  

unsigned long lastWiFiAttempt = 0;

unsigned long lastApiAttempt = 0;

unsigned long lastBlink = 0;

unsigned long apiInterval = API_RETRY_MS;

  

bool wifiAttempted = false;

bool wasConnected = false;

bool apiAttempted = false;

bool ledState = false;

  

// =====================================

// CHANNEL DATA

// =====================================

  

String subscribers = "--";

String videos = "--";

  

String lastStatus = "";

uint16_t lastStatusColor = WHITE;

  

volatile int disconnectReason = -1;

  

// =====================================

// FUNCTION DECLARATIONS

// Fixes 'not declared in this scope'

// =====================================

  

void updateNumbers();

void showStatus(String message, uint16_t color);

  

// =====================================

// CENTERED SMOOTH FONT

// =====================================

  

void drawCentered(

String text,

int baseline,

const GFXfont *font,

uint16_t color) {

tft.setFont(font);

tft.setTextSize(1);

tft.setTextColor(color);

tft.setTextWrap(false);

  

int16_t x1, y1;

uint16_t w, h;

  

tft.getTextBounds(

text, 0, 0,

&x1, &y1, &w, &h);

  

int x = 120 - (int)w / 2 - x1;

  

tft.setCursor(x, baseline);

tft.print(text);

}

  

// =====================================

// CENTERED SMALL FONT

// =====================================

  

void drawSmall(

String text,

int y,

uint16_t color) {

tft.setFont(NULL);

tft.setTextSize(1);

tft.setTextColor(color);

tft.setTextWrap(false);

  

int x = (240 - text.length() * 6) / 2;

  

tft.setCursor(max(0, x), y);

tft.print(text);

}

  

// =====================================

// FORMAT NUMBER

// =====================================

  

String formatNumber(String raw) {

if (raw.length() == 0) {

return "--";

}

  

String result = "";

  

for (int i = 0; i < raw.length(); i++) {

result += raw[i];

  

int remain = raw.length() - i - 1;

  

if (remain > 0 && remain % 3 == 0) {

result += ",";

}

}

  

return result;

}

  

// =====================================

// YOUTUBE LOGO

// =====================================

  

void drawLogo() {

tft.fillRoundRect(

100, 15, 40, 25, 7, RED);

  

tft.fillTriangle(

116, 20,

116, 35,

130, 27,

WHITE);

}

  

// =====================================

// TOP ACCENT ARC

// =====================================

  

void drawAccentArc() {

for (int a = 246; a <= 294; a++) {

float rad = a * PI / 180.0f;

  

int x1 = 120 + cos(rad) * 116;

int y1 = 120 + sin(rad) * 116;

  

int x2 = 120 + cos(rad) * 118;

int y2 = 120 + sin(rad) * 118;

  

tft.drawLine(x1, y1, x2, y2, RED);

}

}

  

// =====================================

// DRAW DASHBOARD ONCE

// =====================================

  

void drawDashboard() {

tft.fillScreen(BG);

  

// Outer rings

tft.drawCircle(120, 120, 118, COLOR_BORDER);

tft.drawCircle(120, 120, 116, COLOR_BORDER);

  

drawAccentArc();

drawLogo();

  

// Channel name

drawCentered(

DISPLAY_NAME,

73,

&FreeSansBold18pt7b,

WHITE);

  

// YouTube handle

drawSmall(

String("@") + CHANNEL_HANDLE,

85,

MUTED);

  

// ==================================

// SUBSCRIBERS CARD

// ==================================

  

tft.fillRoundRect(

27, 102, 186, 58, 11, CARD);

  

tft.drawRoundRect(

27, 102, 186, 58, 11, COLOR_BORDER);

  

tft.fillRoundRect(

29, 114, 3, 32, 1, RED);

  

tft.setFont(NULL);

tft.setTextSize(1);

tft.setTextColor(MUTED);

  

tft.setCursor(43, 112);

tft.print("SUBSCRIBERS");

  

// ==================================

// VIDEOS CARD

// ==================================

  

tft.fillRoundRect(

27, 166, 186, 49, 11, CARD);

  

tft.drawRoundRect(

27, 166, 186, 49, 11, COLOR_BORDER);

  

tft.fillRoundRect(

29, 176, 3, 27, 1, CYAN);

  

tft.setFont(NULL);

tft.setTextSize(1);

tft.setTextColor(MUTED);

  

tft.setCursor(43, 175);

tft.print("TOTAL VIDEOS");

  

// Function declaration fixes the error

updateNumbers();

}

  

// =====================================

// AUTO-FIT NUMBER FONTS

// =====================================

  

void drawFittedNumber(

String value,

int baseline,

int maxWidth,

bool large,

uint16_t color) {

const GFXfont *fonts[] = {

&FreeSansBold18pt7b,

&FreeSansBold12pt7b,

&FreeSansBold9pt7b};

  

int startIndex = large ? 0 : 1;

  

const GFXfont *chosen = fonts[2];

  

for (int i = startIndex; i < 3; i++) {

tft.setFont(fonts[i]);

tft.setTextSize(1);

  

int16_t x1, y1;

uint16_t w, h;

  

tft.getTextBounds(

value, 0, 0,

&x1, &y1, &w, &h);

  

if (w <= maxWidth) {

chosen = fonts[i];

break;

}

}

  

drawCentered(

value,

baseline,

chosen,

color);

}

  

// =====================================

// UPDATE NUMBERS WITHOUT FULL REDRAW

// =====================================

  

void updateNumbers() {

// Clear only the changing number areas

tft.fillRect(

37, 126, 166, 31, CARD);

  

tft.fillRect(

37, 187, 166, 25, CARD);

  

drawFittedNumber(

subscribers,

155,

158,

true,

WHITE);

  

drawFittedNumber(

videos,

207,

158,

false,

CYAN);

}

  

// =====================================

// CENTERED STATUS INDICATOR

// =====================================

  

void showStatus(

String message,

uint16_t color) {

if (message == lastStatus &&

color == lastStatusColor) {

return;

}

  

lastStatus = message;

lastStatusColor = color;

  

// Clear previous status

tft.fillRect(55, 220, 130, 17, BG);

  

tft.setFont(NULL);

tft.setTextSize(1);

tft.setTextWrap(false);

  

// Prevent text overflow

if (message.length() > 18) {

message = message.substring(0, 18);

}

  

// Measure text

int textWidth = message.length() * 6;

  

// Center indicator + text as one group

int totalWidth = 10 + textWidth;

int startX = (240 - totalWidth) / 2;

  

// Colored status dot

tft.fillCircle(

startX + 3,

228,

3,

color);

  

// Centered status text

tft.setTextColor(SOFT);

  

tft.setCursor(

startX + 10,

224);

  

tft.print(message);

  

Serial.print("Status: ");

Serial.println(message);

}

  

// =====================================

// WIFI EVENT HANDLER

// =====================================

  

void wifiEvent(

WiFiEvent_t event,

WiFiEventInfo_t info) {

if (event == ARDUINO_EVENT_WIFI_STA_DISCONNECTED) {

disconnectReason =

info.wifi_sta_disconnected.reason;

}

}

  

// =====================================

// WORKING WIFI STABILITY FIXES

// =====================================

  

void configureWiFi() {

WiFi.mode(WIFI_STA);

  

// Disable power saving

WiFi.setSleep(false);

  

// Preserve working ESP32-C3 setting

WiFi.setTxPower(WIFI_POWER_8_5dBm);

  

Serial.println("WiFi TX power: 8.5 dBm");

Serial.println("WiFi sleep: disabled");

}

  

// =====================================

// CONNECT WIFI

// =====================================

  

void connectWiFi() {

if (WiFi.status() == WL_CONNECTED) {

return;

}

  

wifiAttempted = true;

lastWiFiAttempt = millis();

  

showStatus("CONNECTING", YELLOW);

  

configureWiFi();

  

if (strlen(WIFI_PASSWORD) == 0) {

WiFi.begin(WIFI_SSID);

} else {

WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

}

  

Serial.println("Connecting to WiFi...");

}

  

// =====================================

// FETCH YOUTUBE STATISTICS

// =====================================

  

bool fetchYouTubeStats() {

if (WiFi.status() != WL_CONNECTED) {

showStatus("WIFI OFFLINE", RED);

return false;

}

  

if (strlen(API_KEY) == 0 ||

String(API_KEY) ==

"PASTE_YOUR_YOUTUBE_API_KEY_HERE") {

showStatus("SET API KEY", YELLOW);

return false;

}

  

showStatus("SYNCING", YELLOW);

  

WiFiClientSecure client;

  

// Prototype only: TLS verification disabled

client.setInsecure();

client.setTimeout(20000);

  

HTTPClient http;

  

String url =

"https://www.googleapis.com/youtube/v3/channels"

"?part=snippet%2Cstatistics"

"&forHandle=%40" +

String(CHANNEL_HANDLE) +

"&key=" + String(API_KEY);

  

if (!http.begin(client, url)) {

showStatus("HTTPS ERROR", RED);

return false;

}

  

http.setTimeout(20000);

http.addHeader("Accept-Encoding", "identity");

  

Serial.println("Requesting YouTube stats...");

  

int httpCode = http.GET();

  

Serial.print("HTTP status: ");

Serial.println(httpCode);

  

Serial.print("Content length: ");

Serial.println(http.getSize());

  

// ==================================

// JSON FIX: READ FULL RESPONSE

// ==================================

  

String payload = http.getString();

http.end();

  

Serial.print("Response length: ");

Serial.println(payload.length());

  

if (httpCode != 200) {

if (httpCode > 0) {

Serial.println(payload.substring(0, 400));

}

  

showStatus(

httpCode == 400 || httpCode == 403

? "API ERROR"

: "NET ERROR",

RED);

  

return false;

}

  

JsonDocument doc;

  

DeserializationError error =

deserializeJson(doc, payload);

  

if (error) {

Serial.print("JSON error: ");

Serial.println(error.c_str());

  

showStatus("JSON ERROR", RED);

return false;

}

  

JsonArray items =

doc["items"].as<JsonArray>();

  

if (items.isNull() || items.size() == 0) {

showStatus("NOT FOUND", RED);

return false;

}

  

JsonObject channel = items[0];

  

bool hidden =

channel["statistics"]["hiddenSubscriberCount"] | false;

  

String rawSubs =

channel["statistics"]["subscriberCount"]

.as<String>();

  

String rawVideos =

channel["statistics"]["videoCount"]

.as<String>();

  

String newSubs =

hidden ? "HIDDEN" : formatNumber(rawSubs);

  

String newVideos =

formatNumber(rawVideos);

  

Serial.println();

Serial.println("===== YOUTUBE STATS =====");

  

Serial.print("Channel: ");

Serial.println(

channel["snippet"]["title"].as<String>());

  

Serial.print("Subscribers: ");

Serial.println(newSubs);

  

Serial.print("Videos: ");

Serial.println(newVideos);

  

Serial.println("=============================");

  

// Update only when values change

if (subscribers != newSubs ||

videos != newVideos) {

  

subscribers = newSubs;

videos = newVideos;

  

updateNumbers();

}

  

showStatus("LIVE DATA", GREEN);

return true;

}

  

// =====================================

// ONBOARD LED

// =====================================

  

void updateLED() {

if (WiFi.status() != WL_CONNECTED) {

digitalWrite(LED_PIN, LED_OFF);

ledState = false;

return;

}

  

if (millis() - lastBlink >= LED_INTERVAL) {

lastBlink = millis();

  

ledState = !ledState;

  

digitalWrite(

LED_PIN,

ledState ? LED_ON : LED_OFF);

}

}

  

// =====================================

// SETUP

// =====================================

  

void setup() {

Serial.begin(115200);

delay(1500);

  

pinMode(LED_PIN, OUTPUT);

digitalWrite(LED_PIN, LED_OFF);

  

pinMode(TFT_BL, OUTPUT);

digitalWrite(TFT_BL, HIGH);

  

SPI.begin(

TFT_SCLK, -1, TFT_MOSI, TFT_CS);

  

tft.begin();

tft.setRotation(0);

  

// Draw interface once

drawDashboard();

  

// Initial status

showStatus("STARTING", YELLOW);

  

WiFi.onEvent(wifiEvent);

  

configureWiFi();

connectWiFi();

  

Serial.println("Dashboard Started");

}

  

// =====================================

// MAIN LOOP

// =====================================

  

void loop() {

unsigned long now = millis();

  

if (WiFi.status() == WL_CONNECTED) {

  

if (!wasConnected) {

wasConnected = true;

  

Serial.println("WiFi connected!");

Serial.print("IP: ");

Serial.println(WiFi.localIP());

  

showStatus("WIFI READY", GREEN);

  

// Fetch immediately

apiAttempted = false;

}

  

if (!apiAttempted ||

now - lastApiAttempt >= apiInterval) {

  

apiAttempted = true;

lastApiAttempt = millis();

  

bool success = fetchYouTubeStats();

  

apiInterval = success

? API_REFRESH_MS

: API_RETRY_MS;

}

  

} else {

if (wasConnected) {

wasConnected = false;

  

Serial.println("WiFi disconnected!");

Serial.print("Reason: ");

Serial.println(disconnectReason);

  

showStatus("WIFI OFFLINE", RED);

}

  

if (!wifiAttempted ||

now - lastWiFiAttempt >= WIFI_RETRY_MS) {

  

connectWiFi();

}

}

  

updateLED();

delay(10);

}
