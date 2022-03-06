#pragma once
#include "HSEPD_BASIC.h"
#include <Arduino.h>

#ifndef NOT_USE_FS
#include <FS.h>
#endif // !NOT_USE_FS

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
#ifndef NOT_USE_FS
    fs::FS *EPDFs;
#endif // !NOT_USE_FS
    uint16_t _width;
    uint16_t _height;

    ORIGIN _origin;

    uint8_t _fontWidth;
    uint8_t _fontHeight;
    bool _fontVariable;
    const char* _fontIndex;

    bool DrawAbsolutePixel(uint16_t x, uint16_t y, COLOR color = COLOR::black);
   

public:
    uint8_t *DisBuffer;
    HSEPD_GUI(/* args */);
    ~HSEPD_GUI();

    void SetOrigin(ORIGIN origin);
    ORIGIN GetOrigin();
    void SetFS(fs::FS *fs);
    bool GUIBegin(uint16_t width, uint16_t height, ORIGIN origin = ORIGIN::TL); //默认原点在左上
    void GUIEnd();
    void ClearBuffer();
    void FontBegin(const char* fontIndex, bool variable, uint8_t height, uint8_t width);
    //void FontEnd();
    int printf(uint16_t x, uint16_t y,const char* format,...);
    bool DrawPixel(uint16_t x, uint16_t y, COLOR color = COLOR::black);
    bool DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, COLOR color = COLOR::black);
    bool DrawStraightLine(bool direction, uint16_t fixed, uint16_t move0, uint16_t move1, uint16_t lineWeidth = 1, COLOR color = COLOR::black);
    bool DrawSolidBox(uint16_t x, uint16_t y, uint16_t weidth, uint16_t height, COLOR color = COLOR::black);
    bool DrawHollowBox(uint16_t x, uint16_t y, uint16_t weidth, uint16_t height, uint16_t lineWeidth = 1);
    bool DrawSoildCircle(uint16_t x, uint16_t y, uint16_t radius, COLOR color = COLOR::black);
    bool DrawHollowCircle(uint16_t x, uint16_t y, uint16_t radius, uint16_t lineWeidth = 1);
    bool DrawImageArr(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *image);

     int putchar(uint16_t x, uint16_t y,char ch);
};
