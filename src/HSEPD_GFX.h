#pragma once

#if defined(Arduino)
#include <Arduino.h>
#endif

#include "HSEPD_LOG.h"

enum class Rotate
{
    rotateNone,
    rotate90,
    rotate180,
    rotate270
};

enum class Gray
{
    Gray2 = 0, // 仅黑白
    Gray4,     // 4灰度
    Gray16,    // 16灰度
};

class HSEPD_GFX
{
public:
    HSEPD_GFX(int16_t width, int16_t height);
    ~HSEPD_GFX();

    void begin();
    void begin(Gray gray, Rotate rotate = Rotate::rotateNone, int8_t hw_rotate = 0) = 0;
    void end();

    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void DrawPixel(int16_t x, int16_t y, uint16_t color);

protected:
    int16_t _width;
    int16_t _height;

    Gray _gray;
    Rotate _rotate;
    int8_t _hw_rotate;

    uint8_t *_buffer;
};
