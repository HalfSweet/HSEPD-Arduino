#pragma once
#include "HSEPD_BASIC.h"
#include <Arduino.h>

enum class ORIGIN 
{
    TL,
    BL,
    TR,
    BR,
    TopLeft,
    BottomLeft,
    TopRight,
    BottomRight,
};

enum class COLOR
{
    write,
    black,
};

class HSEPD_GUI
{
private:
    /* data */
    uint8_t *DisBuffer;

    uint16_t _width;
    uint16_t _height;

    ORIGIN _origin;

    bool DrawAbsolutePixel(uint16_t x, uint32_t y, COLOR color = COLOR::black);

public:
    HSEPD_GUI(/* args */);
    ~HSEPD_GUI();

    void SetOrigin(ORIGIN origin);
    ORIGIN GetOrigin();
    bool GUIBegin(uint16_t width, uint16_t height, ORIGIN origin = ORIGIN::TL);//默认原点在左上
    bool DrawPixel(uint16_t x, uint32_t y, COLOR color = COLOR::black);
};
