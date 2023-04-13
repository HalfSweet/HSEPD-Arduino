#include "HSEPD_GFX.h"

HSEPD_GFX::HSEPD_GFX(int16_t width, int16_t height)
    : _width(width),
      _height(height)
{
}

HSEPD_GFX::~HSEPD_GFX()
{
}

inline void HSEPD_GFX::begin()
{

}

void HSEPD_GFX::end()
{
    delete[] _buffer;
}

void HSEPD_GFX::drawPixel(int16_t x, int16_t y, uint16_t color)
{

}

void HSEPD_GFX::DrawPixel(int16_t x, int16_t y, uint16_t color)
{
}
