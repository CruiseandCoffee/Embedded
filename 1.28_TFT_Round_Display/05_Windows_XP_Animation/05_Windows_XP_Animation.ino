// Single-file authentic XP animation for ESP32-C3 + GC9A01 (240x240).

// Graphics are JPEG conversions of the FOUR screenshots you uploaded.

// Libraries (Arduino IDE Library Manager):

// Adafruit GFX Library, Adafruit GC9A01A, TJpg_Decoder (Bodmer)

// No SD card, image headers, Python, WiFi, or filesystem required.

  

#include <Adafruit_GC9A01A.h>

#include <Adafruit_GFX.h>

#include <Arduino.h>

#include <SPI.h>

#include <TJpg_Decoder.h>

#include <mbedtls/base64.h>

#include <pgmspace.h>

  

#define TFT_SCLK 4

#define TFT_MOSI 6

#define TFT_CS 7

#define TFT_DC 5

#define TFT_RST 3

#define TFT_BL 10

  

Adafruit_GC9A01A tft(&SPI, TFT_DC, TFT_CS, TFT_RST);

  

// BOOT: image supplied by user; 3026 JPEG bytes.

static const char BOOT_B64[] PROGMEM =

"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";

  

// WELCOME: image supplied by user; 2122 JPEG bytes.

static const char WELCOME_B64[] PROGMEM =

"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"

"SAykoCogAiJIDMiAJL//2Q==";

  

// DESKTOP: image supplied by user; 8110 JPEG bytes.

static const char DESKTOP_B64[] PROGMEM =

"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";

  

// ERROR: image supplied by user; 2902 JPEG bytes.

static const char ERROR_B64[] PROGMEM =

"/9j/4AAQSkZJRgABAQAAAQABAAD/2wBDAAcFBQYFBAcGBgYIBwcICxILCwoKCxYPEA0SGhYbGhkWGRgcICgiHB4mHhgZIzAkJiorLS4t"

"GyIyNTEsNSgsLSz/2wBDAQcICAsJCxULCxUsHRkdLCwsLCwsLCwsLCwsLCwsLCwsLCwsLCwsLCwsLCwsLCwsLCwsLCwsLCwsLCwsLCws"

"LCz/wAARCAAyALgDASIAAhEBAxEB/8QAGwAAAQUBAQAAAAAAAAAAAAAAAAIDBAUGAQf/xABCEAABAwMBAwcKAwcCBwAAAAABAgMEAAUR"

"IQYSMRMVQVFUk9EHFyIyRFVhcZHSFBaSNUJzgbLB8DOxJENSU3Kh4f/EABoBAQEAAwEBAAAAAAAAAAAAAAABAgMGBQT/xAAtEQACAAQD"

"BgQHAAAAAAAAAAAAAQIEESEFUaIDFBVSkqESMUGyImGBgsHR0v/aAAwDAQACEQMRAD8AutoXdp3r1c3bftPLYQmY+AwZBQhpCVqHQdB6"

"PV0nqNVbjm2aJS2UbWvr3AVbxkuJykHG9w0GQdfEZ3F38k1mut7mXF6dPQ7KeU6pKFI3QSckD0eFQx5GbDvbvONx3h0b6M/011myxORh"

"gSia8uRHhxykw22lqMsU7dICyvaZ9AQColcxaRjJHEjXh0cNM4zUWfddrrfD5dzaaeshS0rQl130SkpHEjGu9pw4aZyK3C/I9ZnAgLud"

"zWG07iQVoO6Mk4Ho8Mk/WueZux+8Ll+pH21shxaQr8VOgwclM0t7jLgbaJSpTm1T6BulSMSlqLnDgMcNePQNeFNSZG20RhDrt/nEKIBS"

"l9ZUATx4dHEjPDXhWu8ztk94XH9SPtpavJJaFNobVcrmpCM7qStBCc8cej01isVka3a6A5KZp69Rm0PbRKffR+abslLLrjWqlby91JO+"

"Bn1dMZ6yBrVTb9odpZ1wZi/mOe1y6ihBVIOiugKGdMnAzW580VlznnC45699H213zRWTt1w/Uj7asOK4ek63+xEclNOlPcZt53alMrkY"

"+1j7/E5/EKGcbpyOOmFoPwyc6AmmpUvayBEVLkbSSeQQsIUUSVKPrYOBjo1+eK1Q8kllHt0/9SPtp5PkvtiI646bncgysgqQFowo/Ebv"

"wH0rDikiqXXQVyUy6/0ZsDalTriUbUSVJbBBUJBwVYyB1YwU/HU6aVxCdslBA/MDyVrwQhUpQPXrkadNaLzUWYe2z/1I+2u+aqz9un/q"

"R9tTiclmugu5zOWow0jaXaOLKcYXfpq1Nq3SUPqIPypA2r2g99z++Nb3zWWfts79SPtro8ltnHtk76o+2tyxbDaXWk0uRnM+5hBtVtB7"

"6nd8aWNqr/76nd8a3PmvtHbJv1R9td82NpHtk36o+2pxbDeXSNwnM+5hxtTf/fM7vjSxtRfvfM7vjW3HkztI9rm/VHhXfNrah7XM+qfC"

"pxbDeXSNwnM+5ihtPfffE3vjShtNfPe83vjW0829qHtcz6p8K6PJxax7XM+qfCpxXDuXSNwnM+5jRtLfPe83vjT8baK8uSmUKu0wpU4k"

"EcseBIrWjydWse1S/qnwpSPJ9bW1pWmXLCkkEHKdCP5Vi8Uw9qy0hSM5n3IFy2iXImXG3MR7hF/BSAyp92QtTclJK0ncz6wG7hR6Dprx"

"oq7uljccgOuSbvPkpZSp5LbhRulQSccEiiuP2kVUlWr+v5OjgVG7UE+UC+PbPbIzZ8fIdC0tJUP3N5WN6vPrmZEfZBO1bdwmJhraQY7C"

"WAmQ28SRvrX/ANvKThX72QK9XvVqiXy2S7bOb5SNJSULSDg/Ag9BBwQfhXlsjyQbSPXaQ4NtV/hZTQjOrU2ovLYwByZGd06AfCvjjgcT"

"Ohw6d2Uvs/C7OtXatVa3y9b/ALZvNhL+7tNsVAukgAPuBSHCBgKUlRSVfzxmpFw2iRa5byJMN8sNJ3uWawvOgPq6Eetjp+mtSrJZ4mz9"

"kiWqAgojRUBCN45J6SSekkkk/Op9bVbzPG2kSijcUKom/IqXto4bLgQpuUVFAcwGuCSneyTnHCkRtqIEuQyw23JC3gS2FNgbwAJ0119U"

"jT58CDVz0UhTTa3EOKbSpaM7qikEpzxweirY1lGvbG2tvlCkyAkIK94t8eBAA6yDpwpadrYCm5K+RmJ/DIDjgUzg4KsYGup/zNXlFWwK"

"hW0cYIkKQxIXyDPL5CQAoboVgEnjhXy+NMt7Vw1sSFGPKQ5HaS6tCkAD0sAAKzg8Rrwx/MVe0VLAo07Vwi00tbEtHKlASOSz62ME66DX"

"/OFdn7TMW+ZyTsSQ4xlIEhpO8klQBAH16/7Zu+PHWirYFFE2wtk2a1FZRKLjq9xOWsDOupOeGnH4/PDattba2G99makuJCkjkM5B6eP+"

"fPArQ0calgVK9oYrMZTzzMhtKVqRjdBPogEk4OBorOpHTUVO2dtLSVlqUTulSwhre5MAka9OuM4xWgopYFE5tZEaW1vxpQadbbcQsIyS"

"VnATjrGcnX+1Nq2ztzUdLrzMtoKBOC2DjCt08Fdedf74B0OBjGNOquEBQIIBB6DVsCgTtlb3UuckzKUpviChKQenRRVg6dRPwyASHJW1"

"cKI6tDrErdQ4tsrCBu5TkaZIzwq7AAAwBppXaWBTObTRG7i/FUzIIY3d9wIyBnOdM5GMdX/1I2nirS2tth/cU6to8oA2oFBSDhJ9b1vh"

"oCeiruipYEOc4h6ySnEKCklhzUHP7popVz/ZMz+Av+k0VQTF/wCor5mk11f+or5muVAFFFFAFQ5Lsn8czHjrab321uKU42V+qUAAAKH/"

"AFH6VMqvuLFwVKYkW5UPfQhbaxJ38YUUnI3f/Hp66ICX3ZkZTaXJkfLhISEw1qOnE6OaAdJ4CoovbJaLgv8Aay2Bkq5I4A688pSJUO+z"

"UJTJZsToScgKL+mmP9qjcxXHkUtCBs6EIBAA5cYyc/76/PWqC4YVOkxm32psVTbid5J/Cq1HeV2yzXZ0WUuSEb8eU7Hy2CkKCFYBwScZ"

"+dRG29o2WktNosaEIGEpBfwB9KdtUKfb7c8hxURyTIlOyF7pWG076s4GmfrQEty6wGZa47zimXEAElwFKTkZ0URg/wAqS5ebW0+tlyW2"

"2tG7vBSsesN4dHVrTEiE/KS4Ho0BfKp3FkleVJ6uHCo8uxJnSA/Ig25Tm9vKUFOJK/grA9IfA6aDqqAs4dzt9w3vwcluRugE8msHAPA0"

"5GeDy5IKcBl0tjB46JOf/dQIkF6AjciRLcwnGMI3x8eqn2GpbDL5H4dTzzpcIJVugYA6s50oByRcYcRzcfWpGE76lEHdQCcDeVjAyQQM"

"8cVH/MVk5Tc5xYzjOeU06enGOg03Kt7s1YVJiW91QTu5UpzhnONKbNoClAm32skI5Pgv1cYxw4Y0+VUFkxOhSX3WWH23XWccohCwSjPD"

"I6OFDL/KvyUbuAysIGvH0Qf71BiW92AVmJEt7Bcxvbhc9LHXpUuIy80p9b5b33nAvDecDCQnp+VQEmiiigCiiigItz/ZEz+Av+k0UXP9"

"kTP4C/6TRQGFXdbhyiv+Pk8T/wA1XjXOdbh2+T3qvGiigDnW4dvk96rxo51uHb5Peq8aKKAOdbh2+T3qvGjnW4dvk96rxoooA51uHb5P"

"eq8aOdbh2+T3qvGiigDnW4dvk96rxo51uHb5Peq8aKKAOdbh2+T3qvGjnW4dvk96rxoooA51uHb5Peq8aOdbh2+T3qvGiigDnW4dvk96"

"rxo51uHb5Peq8aKKAOdbh2+T3qvGjnW4dvk96rxoooA51uHb5Peq8aOdbh2+T3qvGiigDnW4dvk96rxo51uHb5Peq8aKKAakXOeqK6lU"

"6SQUKBBdVrp86KKKqB//2Q==";

  

// TJpg_Decoder streams small tiles to the TFT; no full-screen RAM buffer.

bool jpgToScreen(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *pixels) {

if (x >= 240 || y >= 240)

return false;

tft.drawRGBBitmap(x, y, pixels, w, h);

return true;

}

  

// Decode embedded base64 into a small temporary compressed-JPEG buffer.

void showImage(const char *src, int x, int y) {

size_t encodedLen = strlen(src);

size_t maxLen = (encodedLen / 4) * 3 + 4;

uint8_t *jpg = (uint8_t *)malloc(maxLen);

if (!jpg) {

Serial.println("JPEG RAM allocation failed");

return;

}

size_t jpgLen = 0;

int err = mbedtls_base64_decode(jpg, maxLen, &jpgLen,

(const unsigned char *)src, encodedLen);

if (err == 0)

TJpgDec.drawJpg(x, y, jpg, (uint32_t)jpgLen);

else

Serial.printf("Base64 decode error: %d\n", err);

free(jpg);

}

  

void sceneBoot() {

tft.fillScreen(0x0000);

showImage(BOOT_B64, 0, 30);

}

void sceneWelcome() {

tft.fillScreen(0x08B3); // blue edges outside the 4:3 screenshot

showImage(WELCOME_B64, 0, 30);

}

void sceneDesktop() {

tft.fillScreen(0x0000);

showImage(DESKTOP_B64, 0, 30);

}

  

// Moving three-blue-block boot indicator (over the screenshot's original bar).

void drawBootLoading(uint32_t step) {

const int x = 94, y = 160, width = 52, height = 7;

tft.fillRect(x - 1, y - 1, width + 2, height + 2, 0x0000);

tft.drawRoundRect(x, y, width, height, 2, 0xBDF7);

const int slot = (step * 3) % 32;

for (int i = 0; i < 3; i++) {

int bx = x + 3 + (slot + i * 5) % 39;

tft.fillRect(bx, y + 2, 4, 3, 0x325F);

}

}

  

// Draws ORIGINAL uploaded XP error dialog without redrawing desktop.

void addError(int index) {

const int16_t xs[] = {28, 10, 42, 4, 35, 17, 48, 0, 25, 11, 40, 5, 29};

const int16_t ys[] = {78, 94, 62, 124, 112, 40, 143, 59, 160, 108, 85, 135, 52};

const int k = index % 13;

showImage(ERROR_B64, xs[k], ys[k]);

}

  

uint8_t scene = 255;

uint32_t started = 0, lastPop = 0;

int oldBootStep = -1, errorCount = 0;

  

void changeScene(uint8_t next) {

scene = next;

started = millis();

lastPop = started;

switch (next) {

case 0:

sceneBoot();

oldBootStep = -1;

break;

case 1:

sceneWelcome();

break;

case 2:

sceneDesktop();

break;

case 3:

errorCount = 0;

break; // keep actual desktop visible

case 4:

tft.fillScreen(0x0000);

tft.setTextColor(0xFFFF);

tft.setTextSize(2);

tft.setCursor(28, 116);

tft.print("Restarting...");

break;

}

}

  

void setup() {

Serial.begin(115200);

pinMode(TFT_BL, OUTPUT);

digitalWrite(TFT_BL, HIGH);

SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);

tft.begin();

tft.setRotation(0);

tft.setSPISpeed(40000000);

TJpgDec.setJpgScale(1);

TJpgDec.setSwapBytes(false); // Adafruit_GFX expects regular RGB565

TJpgDec.setCallback(jpgToScreen);

changeScene(0);

}

  

void loop() {

uint32_t now = millis(), ms = now - started;

if (scene == 0) {

int step = ms / 110;

if (step != oldBootStep) {

oldBootStep = step;

drawBootLoading(step);

}

if (ms > 6500)

changeScene(1);

} else if (scene == 1) {

if (ms > 2500)

changeScene(2);

} else if (scene == 2) {

if (ms > 3200)

changeScene(3);

} else if (scene == 3) {

uint32_t spacing = (errorCount == 0) ? 750 : (errorCount < 4 ? 510 : 190);

if (errorCount < 13 && now - lastPop >= spacing) {

lastPop = now;

addError(errorCount++);

}

if (ms > 8100)

changeScene(4);

} else if (scene == 4) {

if (ms > 1800)

changeScene(0);

}

delay(5);

}
