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

    uint16_t _width;
    uint16_t _height;

    ORIGIN _origin;

    bool DrawAbsolutePixel(uint16_t x, uint16_t y, COLOR color = COLOR::black);

public:
    uint8_t *DisBuffer;
    HSEPD_GUI(/* args */);
    ~HSEPD_GUI();

    void SetOrigin(ORIGIN origin);
    ORIGIN GetOrigin();
    bool GUIBegin(uint16_t width, uint16_t height, ORIGIN origin = ORIGIN::TL); //默认原点在左上
    void GUIEnd();
    bool DrawPixel(uint16_t x, uint16_t y, COLOR color = COLOR::black);
    void ClearBuffer();
    bool DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, COLOR color = COLOR::black);
    bool DrawStraightLine(bool direction, uint16_t fixed, uint16_t move0, uint16_t move1, uint16_t lineWeidth = 1, COLOR color = COLOR::black);
    bool DrawSolidBox(uint16_t x, uint16_t y, uint16_t weidth, uint16_t height, COLOR color = COLOR::black);
    bool DrawHollowBox(uint16_t x, uint16_t y, uint16_t weidth, uint16_t height, uint16_t lineWeidth = 1);
    bool DrawSoildCircle(uint16_t x, uint16_t y, uint16_t radius, COLOR color = COLOR::black);
    bool DrawHollowCircle(uint16_t x, uint16_t y, uint16_t radius,uint16_t lineWeidth = 1);
};
