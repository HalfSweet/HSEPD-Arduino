#include "HSEPD_GUI.h"

HSEPD_GUI::HSEPD_GUI(/* args */)
{
}

HSEPD_GUI::~HSEPD_GUI()
{
}

void SetOrigin(ORIGIN origin)
{
    _origin = origin;
}

ORIGIN GetOrigin()
{
    return _origin;
}

bool HSEPD_GUI::GUIBegin(uint16_t width, uint16_t height, ORIGIN origin, CDOT cdot)
{
    DisBuffer = new (std::nothrow) uint8_t[width * height / 8];
    if (DisBuffer == nullptr)
    {
        EPD_LOGE("Your RAM is too low, please check it.");
        return false;
    }
    _width = width;
    _height = height;
    SetOrigin(origin);

    EPD_LOGD("GUIBegin finish.");
    return true;
}

bool HSEPD_GUI::DrawAbsolutePixel(uint16_t x, uint32_t y, COLOR color)
{
    if (x >= _width || y >= _height)
    {
        EPD_LOGE("The coordinates you entered are outside the window.");
        return false;
    }
    switch (color)
    {
    case COLOR::write:
        DisBuffer[(x + y * _width) / 8] |= 0x80 >> (x % 8);
        break;

    case COLOR::black:
        DisBuffer[(x + y * _width) / 8] &= 0x80 >> (x % 8);
        break;

    default:
        break;
    }
}

bool HSEPD_GUI::DrawPixel(uint16_t x, uint32_t y, COLOR color)
{
    uint16_t temp;
    switch (_origin)
    {
    case ORIGIN::TL:
    case ORIGIN::TopLeft:
        if (x >= _width || y >= _height)
        {
            EPD_LOGE("The coordinates you entered are outside the window.");
            return false;
        }
        DrawAbsolutePixel(x, y, color);
        break;

    case ORIGIN::BL:
    case ORIGIN::BottomLeft:
        if (x >= height || y >= width)
        {
            EPD_LOGE("The coordinates you entered are outside the window.");
            return false;
        }
        temp = x;
        x = y;
        y = height - point_temp;
        DrawAbsolutePixel(x, y, color);
        break;

    case ORIGIN::BR:
    case ORIGIN::BottomRight:
        if (x >= width || y >= height)
        {
            EPD_LOGE("The coordinates you entered are outside the window.");
            return false;
        }
        x = width - x;
        y = height - y;
        DrawAbsolutePixel(x, y, color);
        break;

    case ORIGIN::TR:
    case ORIGIN::TopRight:
        if (x >= height || y >= width)
        {
            EPD_LOGE("The coordinates you entered are outside the window.");
            return false;
        }
        temp = x;
        x = width - y;
        y = temp;
        DrawAbsolutePixel(x, y, color);
        break;

    default:
        EPD_LOGE("Please enter correct coordinates.");
        return false;
        break;
    }
    EPD_LOGV("Draw pixel finish.");
}