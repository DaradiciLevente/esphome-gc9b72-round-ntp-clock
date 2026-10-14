#pragma once

#include "esphome.h"
#include <Arduino_GFX_Library.h>
#include "Arduino_GC9B72.h"
#include <math.h>

#ifndef BLACK
#define BLACK   0x0000
#endif
#ifndef WHITE
#define WHITE   0xFFFF
#endif
#ifndef RED
#define RED     0xF800
#endif
#ifndef GRAY
#define GRAY    0x8410
#endif

#define TFT_SCLK  0
#define TFT_MOSI  1
#define TFT_RST   2
#define TFT_DC    3
#define TFT_CS    10

class GC9B72Display {
 public:
  Arduino_DataBus *bus = nullptr;
  Arduino_GC9B72 *gfx = nullptr;
  bool is_ready = false;

  const int CX = 180;
  const int CY = 180;

  const int R_OUTER   = 178;
  const int R_MINUTE  = 168;
  const int R_INNER   = 110;
  const int R_NUMBERS = 132;

  // Dimensiuni limbi stil aviație
  const float HOUR_LEN   = 90;
  const float HOUR_TAIL  = 18;
  const float HOUR_WIDTH = 5;

  const float MIN_LEN    = 118;
  const float MIN_TAIL   = 22;
  const float MIN_WIDTH  = 3;

  const float SEC_LEN    = 130;
  const float SEC_TAIL   = 28;

  int lastHour   = -1;
  int lastMinute = -1;
  int lastSecond = -1;

  float oldHourAngle   = -1;
  float oldMinuteAngle = -1;
  float oldSecondAngle = -1;

  void drawCircle(int cx, int cy, int r, uint16_t color) {
    if (!gfx) return;
    int x = r, y = 0;
    int err = 0;
    while (x >= y) {
      gfx->drawPixel(cx + x, cy + y, color);
      gfx->drawPixel(cx + y, cy + x, color);
      gfx->drawPixel(cx - y, cy + x, color);
      gfx->drawPixel(cx - x, cy + y, color);
      gfx->drawPixel(cx - x, cy - y, color);
      gfx->drawPixel(cx - y, cy - x, color);
      gfx->drawPixel(cx + y, cy - x, color);
      gfx->drawPixel(cx + x, cy - y, color);
      if (err <= 0) { y++; err += 2 * y + 1; }
      if (err > 0)  { x--; err -= 2 * x + 1; }
    }
  }

  void drawRadialLine(int cx, int cy, float angleDeg, int r1, int r2, int thickness, uint16_t color) {
    if (!gfx) return;
    float rad = (angleDeg - 90.0) * M_PI / 180.0;
    float dx = cos(rad);
    float dy = sin(rad);
    int x1 = cx + dx * r1;
    int y1 = cy + dy * r1;
    int x2 = cx + dx * r2;
    int y2 = cy + dy * r2;

    for (int t = -thickness / 2; t <= thickness / 2; t++) {
      float px = -dy * t;
      float py =  dx * t;
      gfx->drawLine(x1 + px, y1 + py, x2 + px, y2 + py, color);
    }
  }

  void drawHourNumber(int h) {
    if (!gfx) return;
    float angle = h * 30.0;
    float rad = (angle - 90.0) * M_PI / 180.0;
    int x = CX + cos(rad) * R_NUMBERS - 8;
    int y = CY + sin(rad) * R_NUMBERS - 12;
    if (h >= 10) x -= 8;

    gfx->setTextColor(WHITE);
    gfx->setTextSize(3);
    gfx->setCursor(x, y);
    gfx->print(h);
  }

  void drawMinuteNumber(int m) {
    if (!gfx) return;
    float angle = m * 6.0;
    float rad = (angle - 90.0) * M_PI / 180.0;
    int x = CX + cos(rad) * (R_MINUTE - 12) - 5;
    int y = CY + sin(rad) * (R_MINUTE - 12) - 4;

    gfx->setTextColor(WHITE);
    gfx->setTextSize(1);
    gfx->setCursor(x, y);
    if (m < 10) gfx->print("0");
    gfx->print(m);
  }

  void drawHand(float angleDeg, float length, float tail, float halfWidth,
                uint16_t colorOutline, uint16_t colorFill) {
    if (!gfx) return;
    float rad = (angleDeg - 90.0) * M_PI / 180.0;
    float dx = cos(rad);
    float dy = sin(rad);
    float px = -dy;
    float py =  dx;

    int tipX  = CX + dx * length;
    int tipY  = CY + dy * length;
    int baseX = CX - dx * tail;
    int baseY = CY - dy * tail;
    int c1x = baseX + px * halfWidth;
    int c1y = baseY + py * halfWidth;
    int c2x = baseX - px * halfWidth;
    int c2y = baseY - py * halfWidth;

    gfx->fillTriangle(tipX, tipY, c1x, c1y, c2x, c2y, colorFill);
    if (colorOutline != colorFill) {
      gfx->drawTriangle(tipX, tipY, c1x, c1y, c2x, c2y, colorOutline);
    }
  }

  void drawSecondHand(float angleDeg, uint16_t color) {
    if (!gfx) return;
    float rad = (angleDeg - 90.0) * M_PI / 180.0;
    float dx = cos(rad);
    float dy = sin(rad);

    int tipX  = CX + dx * SEC_LEN;
    int tipY  = CY + dy * SEC_LEN;
    int baseX = CX - dx * SEC_TAIL;
    int baseY = CY - dy * SEC_TAIL;

    gfx->drawLine(baseX, baseY, tipX, tipY, color);
  }

  // ȘTERGERE SECTORIALĂ LATĂ CU NEGRU (Pentru Minutar și Orar)
  void eraseWideSector(float angleDeg, float length, float tail, float sweepAngleDeg) {
    if (angleDeg < 0 || !gfx) return;

    float radL = (angleDeg - sweepAngleDeg - 90.0f) * M_PI / 180.0f;
    float radR = (angleDeg + sweepAngleDeg - 90.0f) * M_PI / 180.0f;

    int rTip = length + 6;
    int rTail = tail + 6;

    int tLx = CX + cos(radL) * rTip;
    int tLy = CY + sin(radL) * rTip;
    int tRx = CX + cos(radR) * rTip;
    int tRy = CY + sin(radR) * rTip;

    int bLx = CX - cos(radR) * rTail;
    int bLy = CY - sin(radR) * rTail;
    int bRx = CX - cos(radL) * rTail;
    int bRy = CY - sin(radL) * rTail;

    gfx->fillTriangle(CX, CY, tLx, tLy, tRx, tRy, BLACK);
    gfx->fillTriangle(CX, CY, bLx, bLy, bRx, bRy, BLACK);
  }

  // ȘTERGERE SECUNDAR (Sector mai restrâns de 5 grade)
  void eraseSecondHand(float angleDeg) {
    eraseWideSector(angleDeg, SEC_LEN, SEC_TAIL, 4.5f);
  }

  void repairLocalArea(float angleDeg) {
    if (angleDeg < 0 || !gfx) return;

    drawCircle(CX, CY, R_INNER, WHITE);

    // Re-desenăm cele 3 cifre din jurul unghiului șters
    int centerHour = (int)((angleDeg + 15.0) / 30.0) % 12;
    for (int offset = -1; offset <= 1; offset++) {
      int h = centerHour + offset;
      while (h < 1) h += 12;
      while (h > 12) h -= 12;
      drawHourNumber(h);
    }

    if (angleDeg >= 110.0f && angleDeg <= 250.0f) {
      gfx->setTextColor(WHITE);
      gfx->setTextSize(2);
      gfx->setCursor(CX - 50, CY + R_INNER - 30);
      gfx->print("NTP Clock");
    }
  }

  void drawDial() {
    if (!gfx) return;
    gfx->fillScreen(BLACK);

    drawCircle(CX, CY, R_OUTER, WHITE);
    drawCircle(CX, CY, R_OUTER - 2, WHITE);
    drawCircle(CX, CY, R_MINUTE, GRAY);
    drawCircle(CX, CY, R_INNER, WHITE);

    for (int i = 0; i < 60; i++) {
      float angle = i * 6.0;
      int r1 = R_OUTER - 4;
      int r2 = (i % 5 == 0) ? (R_MINUTE + 6) : (R_OUTER - 12);
      int thickness = (i % 5 == 0) ? 3 : 2;
      drawRadialLine(CX, CY, angle, r1, r2, thickness, WHITE);
    }

    for (int m = 5; m <= 60; m += 5) drawMinuteNumber(m);
    for (int h = 1; h <= 12; h++) drawHourNumber(h);

    gfx->setTextColor(WHITE);
    gfx->setTextSize(2);
    gfx->setCursor(CX - 50, CY + R_INNER - 30);
    gfx->print("NTP Clock");
  }

  void setup() {
    if (is_ready) return;

    bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, GFX_NOT_DEFINED);
    gfx = new Arduino_GC9B72(bus, TFT_RST, 0, true, 360, 360);

    if (!gfx->begin(8000000)) {
      ESP_LOGE("GC9B72", "Eroare la initializarea display-ului!");
      return;
    }

    gfx->invertDisplay(true);
    drawDial();
    is_ready = true;
  }

  void update_time(int h, int m, int s) {
    if (!is_ready || !gfx) return;

    bool hourChanged   = (h != lastHour || m != lastMinute);
    bool minuteChanged = (m != lastMinute);
    bool secondChanged = (s != lastSecond);

    if (!hourChanged && !minuteChanged && !secondChanged) return;

    // 1. Ștergere sectorială cu negru
    if (secondChanged && oldSecondAngle >= 0) eraseSecondHand(oldSecondAngle);
    if (minuteChanged && oldMinuteAngle >= 0) eraseWideSector(oldMinuteAngle, MIN_LEN, MIN_TAIL, 8.0f);
    if (hourChanged   && oldHourAngle >= 0)   eraseWideSector(oldHourAngle,   HOUR_LEN, HOUR_TAIL, 12.0f);

    // 2. Curățare disc central mic pentru cozi (35px)
    gfx->fillCircle(CX, CY, 35, BLACK);

    // 3. Reparăm cercul interior și cifrele locale afectate
    if (secondChanged && oldSecondAngle >= 0) repairLocalArea(oldSecondAngle);
    if (minuteChanged && oldMinuteAngle >= 0) repairLocalArea(oldMinuteAngle);
    if (hourChanged   && oldHourAngle >= 0)   repairLocalArea(oldHourAngle);

    // 4. Calcul unghiuri
    float hourAngle = (h % 12) * 30.0 + m * 0.5;
    float minAngle  = m * 6.0;
    float secAngle  = s * 6.0;

    // 5. Redesenăm toate cele 3 limbi (prevenim mușcăturile când se suprapun)
    drawHand(hourAngle, HOUR_LEN, HOUR_TAIL, HOUR_WIDTH, WHITE, WHITE);
    drawHand(minAngle, MIN_LEN, MIN_TAIL, MIN_WIDTH, WHITE, WHITE);
    drawSecondHand(secAngle, RED);

    // Dop central curat
    gfx->fillCircle(CX, CY, 5, RED);
    gfx->fillCircle(CX, CY, 3, WHITE);

    oldHourAngle   = hourAngle;
    oldMinuteAngle = minAngle;
    oldSecondAngle = secAngle;

    lastHour   = h;
    lastMinute = m;
    lastSecond = s;
  }
};

static GC9B72Display my_display;