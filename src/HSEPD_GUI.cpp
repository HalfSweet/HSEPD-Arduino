#include "HSEPD_GUI.h"

HSEPD_GUI::HSEPD_GUI(/* args */)
{
}

HSEPD_GUI::~HSEPD_GUI()
{
}

void HSEPD_GUI::SetOrigin(ORIGIN origin)
{
    _origin = origin;
}

ORIGIN HSEPD_GUI::GetOrigin()
{
    return _origin;
}

bool HSEPD_GUI::GUIBegin(uint16_t width, uint16_t height, ORIGIN origin)
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

bool HSEPD_GUI::DrawAbsolutePixel(uint16_t x, uint16_t y, COLOR color)
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
        DisBuffer[(x + y * _width) / 8] &= ~(0x80 >> (x % 8));
        break;

    default:
        break;
    }
    // EPD_LOGV("Draw absolute pixel finish.");
    return true;
}

bool HSEPD_GUI::DrawPixel(uint16_t x, uint16_t y, COLOR color)
{
    uint16_t temp;
    switch (_origin)
    {
    case ORIGIN::BL:
    case ORIGIN::BottomLeft:
        if (x >= _width || y >= _height)
        {
            EPD_LOGE("The coordinates you entered are outside the window.");
            return false;
        }
        DrawAbsolutePixel(x, y, color);
        break;
    case ORIGIN::TL:
    case ORIGIN::TopLeft:

        if (x >= _height || y >= _width)
        {
            EPD_LOGE("The coordinates you entered are outside the window.");
            return false;
        }
        temp = x;
        x = y;
        y = _height - temp;
        DrawAbsolutePixel(x, y, color);
        break;

    case ORIGIN::TR:
    case ORIGIN::TopRight:
        if (x >= _width || y >= _height)
        {
            EPD_LOGE("The coordinates you entered are outside the window.");
            return false;
        }
        x = _width - x;
        y = _height - y;
        DrawAbsolutePixel(x, y, color);
        break;

    case ORIGIN::BR:
    case ORIGIN::BottomRight:

        if (x >= _height || y >= _width)
        {
            EPD_LOGE("The coordinates you entered are outside the window.");
            return false;
        }
        temp = x;
        x = _width - y;
        y = temp;
        DrawAbsolutePixel(x, y, color);
        break;

    default:
        EPD_LOGE("Please enter correct coordinates.");
        return false;
        break;
    }
    // EPD_LOGV("Draw pixel finish.");
    return true;
}

void HSEPD_GUI::ClearBuffer()
{
    for (size_t i = 0; i < (_width * _height / 8); i++)
    {
        DisBuffer[i] = 0xFF;
    }
}

bool HSEPD_GUI::DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, COLOR color)
{
    /* Bresenham algorithm */
    int dx = x1 - x0 >= 0 ? x1 - x0 : x0 - x1;
    int sx = x0 < x1 ? 1 : -1;
    int dy = y1 - y0 <= 0 ? y1 - y0 : y0 - y1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;

    while ((x0 != x1) && (y0 != y1))
    {
        DrawPixel(x0, y0, color);
        if (2 * err >= dy)
        {
            err += dy;
            x0 += sx;
        }
        if (2 * err <= dx)
        {
            err += dx;
            y0 += sy;
        }
    }

    EPD_LOGV("Draw line finish.");
    return true;
}

bool HSEPD_GUI::DrawStraightLine(bool direction, uint16_t fixed, uint16_t move0, uint16_t move1, uint16_t lineWeidth, COLOR color)
{
    uint16_t moveMin = min(move0, move1);
    uint16_t moveMax = max(move0, move1);
    if (direction == 0) //横线
    {
        if (move0 == move1)
        {
            if (DrawPixel(move0, fixed, color) == false)
            {
                return false;
            }
            EPD_LOGV("Draw straight line finish.");
            return true;
        }
        for (size_t i = 0; i < lineWeidth; i++)
        {
            for (size_t j = moveMin; j < moveMax; j++)
            {
                if(DrawPixel(j, fixed + i, color) == false)
                {
                    return false;
                }
            }
        }
    }
    else
    {
        if (move0 == move1)
        {
            if (DrawPixel(fixed, move0, color) == false)
            {
                return false;
            }
            EPD_LOGV("Draw straight line finish.");
            return true;
        }
        
        for (size_t i = 0; i < lineWeidth; i++)
        {
            for (size_t j = moveMin ; j < moveMax; j++)
            {
                if(DrawPixel(fixed + i, j, color) == false)
                {
                    return false;
                }
            }
        }
    }

    EPD_LOGV("Draw straight line finish.");
    return true;
}

bool HSEPD_GUI::DrawSolidBox(uint16_t x, uint16_t y, uint16_t weidth, uint16_t height, COLOR color)
{
    if (DrawStraightLine(1, x, y, y + height - 1, weidth, color))
    {
        return false;
    }
    EPD_LOGV("Draw Solid Box finish.");
    return true;
}

bool HSEPD_GUI::DrawHollowBox(uint16_t x, uint16_t y, uint16_t weidth, uint16_t height, uint16_t lineWeidth)
{
    if (lineWeidth == 0)
    {
        EPD_LOGE("Stupid human, are you kidding me?");
        return false;
    }
    DrawSolidBox(x, y, weidth, height, COLOR::black);
    DrawSolidBox(x + lineWeidth, y + lineWeidth, weidth - 2 * lineWeidth, height - 2 * lineWeidth, COLOR::write);

    EPD_LOGV("Draw Hollow Box finish.");
    return true;
}