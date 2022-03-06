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

void HSEPD_GUI::SetFS(fs::FS *fs)
{
    EPDFs = fs;
}

bool HSEPD_GUI::GUIBegin(uint16_t width, uint16_t height, ORIGIN origin)
{
    DisBuffer = new (std::nothrow) uint8_t[width * height / 8];
    if (DisBuffer == nullptr)
    {
        EPD_LOGE("Your RAM is too low, please check it.");
        return false;
    }
    switch (origin)
    {
    case ORIGIN::BL:
    case ORIGIN::BottomLeft:
    case ORIGIN::TR:
    case ORIGIN::TopRight:
        _width = height;
        _height = width;
        break;

    case ORIGIN::BR:
    case ORIGIN::BottomRight:
    case ORIGIN::TL:
    case ORIGIN::TopLeft:
        _width = width;
        _height = height;
        break;

    default:
        break;
    }

    SetOrigin(origin);

    EPD_LOGD("GUIBegin finish.");
    return true;
}

void HSEPD_GUI::GUIEnd()
{
    delete[] DisBuffer;
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
    switch (_origin)
    {
    case ORIGIN::TL:
    case ORIGIN::TopLeft:
        if (x >= _width || y >= _height)
        {
            // EPD_LOGE("The coordinates you entered are outside the window.");
            return false;
        }
        DrawAbsolutePixel(x, _height - y, color);
        break;

    case ORIGIN::BL:
    case ORIGIN::BottomLeft:
        if (x >= _height || y >= _height)
        {
            // EPD_LOGE("The coordinates you entered are outside the window.");
            return false;
        }
        DrawAbsolutePixel(y, x, color);
        break;

    case ORIGIN::TR:
    case ORIGIN::TopRight:
        if (x >= _width || y >= _height)
        {
            // EPD_LOGE("The coordinates you entered are outside the window.");
            return false;
        }
        DrawAbsolutePixel(_width - y, _height - x, color);
        break;

    case ORIGIN::BR:
    case ORIGIN::BottomRight:

        if (x >= _height || y >= _width)
        {
            // EPD_LOGE("The coordinates you entered are outside the window.");
            return false;
        }
        DrawAbsolutePixel(_width - x, y, color);
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
    int16_t dx = x1 - x0 >= 0 ? x1 - x0 : x0 - x1;
    int8_t sx = x0 < x1 ? 1 : -1;
    int16_t dy = y1 - y0 <= 0 ? y1 - y0 : y0 - y1;
    int8_t sy = y0 < y1 ? 1 : -1;
    int16_t err = dx + dy;

    while ((x0 != x1) && (y0 != y1))
    {
        if (DrawPixel(x0, y0, color) == false)
        {
            EPD_LOGE("The coordinates you entered are outside the window.");
        }
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

    EPD_LOGD("Draw line finish.");
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
                EPD_LOGE("The coordinates you entered are outside the window.");
                return false;
            }
            EPD_LOGD("Draw straight line finish.");
            return true;
        }
        for (size_t i = 0; i < lineWeidth; i++)
        {
            for (size_t j = moveMin; j < moveMax; j++)
            {
                if (DrawPixel(j, fixed + i, color) == false)
                {
                    EPD_LOGE("The coordinates you entered are outside the window.");
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
                EPD_LOGE("The coordinates you entered are outside the window.");
                return false;
            }
            EPD_LOGD("Draw straight line finish.");
            return true;
        }

        for (size_t i = 0; i < lineWeidth; i++)
        {
            for (size_t j = moveMin; j < moveMax; j++)
            {
                if (DrawPixel(fixed + i, j, color) == false)
                {
                    EPD_LOGE("The coordinates you entered are outside the window.");
                    return false;
                }
            }
        }
    }

    EPD_LOGD("Draw straight line finish.");
    return true;
}

bool HSEPD_GUI::DrawSolidBox(uint16_t x, uint16_t y, uint16_t weidth, uint16_t height, COLOR color)
{
    if (DrawStraightLine(1, x, y, y + height - 1, weidth, color))
    {
        return false;
    }
    EPD_LOGD("Draw Solid Box finish.");
    return true;
}

bool HSEPD_GUI::DrawHollowBox(uint16_t x, uint16_t y, uint16_t weidth, uint16_t height, uint16_t lineWeidth)
{
    if (lineWeidth == 0)
    {
        EPD_LOGE("Stupid human, are you kidding me?");
        return false;
    }
    if (DrawSolidBox(x, y, weidth, height, COLOR::black))
    {
        return false;
    }
    DrawSolidBox(x + lineWeidth, y + lineWeidth, weidth - 2 * lineWeidth, height - 2 * lineWeidth, COLOR::write);

    EPD_LOGD("Draw Hollow Box finish.");
    return true;
}

bool HSEPD_GUI::DrawSoildCircle(uint16_t x, uint16_t y, uint16_t radius, COLOR color)
{
    /* Bresenham algorithm */
    int32_t x_pos = -radius;
    int32_t y_pos = 0;
    int32_t err = 2 - 2 * radius;
    int32_t e2 = err;
    bool error = 0;

    while (x_pos <= 0)
    {
        error += !DrawPixel(x - x_pos, y + y_pos, color);
        error += !DrawPixel(x + x_pos, y + y_pos, color);
        error += !DrawPixel(x + x_pos, y - y_pos, color);
        error += !DrawPixel(x - x_pos, y - y_pos, color);

        DrawStraightLine(0, y + y_pos, x + x_pos, x + x_pos + 2 * (-x_pos) + 1, 1, color);
        DrawStraightLine(0, y - y_pos, x + x_pos, x + x_pos + 2 * (-x_pos) + 1, 1, color);
        e2 = err;
        if (e2 <= y_pos)
        {
            err += ++y_pos * 2 + 1;
            if (-x_pos == y_pos && e2 <= x_pos)
            {
                e2 = 0;
            }
        }
        if (e2 > x_pos)
        {
            err += ++x_pos * 2 + 1;
        }
    }
    if (error == 1)
    {
        EPD_LOGE("The coordinates you entered are outside the window.");
        return false;
    }

    EPD_LOGD("Draw soild circle finish.");
    return true;
}

bool HSEPD_GUI::DrawHollowCircle(uint16_t x, uint16_t y, uint16_t radius, uint16_t lineWeidth)
{
    if (DrawSoildCircle(x, y, radius, COLOR::black) == false)
    {
        return false;
    }
    DrawSoildCircle(x, y, radius - 2 * lineWeidth, COLOR::write);
    return true;
}

bool HSEPD_GUI::DrawImageArr(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *image)
{
    uint8_t px = 0;
    for (size_t cx = 0; cx < width; cx++)
    {
        for (size_t cy = 0; cy < height; cy++)
        {
            if (cy % 8 == 0)
            {
                px = *image;
                image++;
            }
            if ((px & (0x01 << (7 - (cy % 8)))) >> (7 - (cy % 8)) == 1) //不要吐槽为什么那么长，看懂就行
            {
                if (DrawPixel(x + cx, y + cy) == false)
                {
                    EPD_LOGW("Draw image fail.");
                    return false;
                }
            }
        }
    }

    EPD_LOGD("Draw image success.");
    return true;
}

void HSEPD_GUI::FontBegin(const char *fontIndex, bool variable, uint8_t height, uint8_t width) // variable为0说明是固定宽度，是1说明是可变宽度
{
    _fontIndex = fontIndex;
    _fontHeight = height;
    _fontVariable = variable;
    _fontWidth = width;
}

int HSEPD_GUI::printf(uint16_t x, uint16_t y, const char *format, ...)
{
    uint16_t fontX = x;
    uint16_t fontY = y;

    va_list vp;
    va_start(vp, format);

    char *pfmt = const_cast<char *>(format);

    while (*pfmt)
    {
        if (*pfmt == '%')
        {
            pfmt++;
            switch (*pfmt)
            {
            case 'c':
                char ch = va_arg(vp, int);
                int i;
                EPD_LOGV("fontX:%d,fontY:%d",fontX,fontY);
                i = putchar(fontX, fontY, ch);
                if (i != -1)
                {
                    fontX += i;
                }
                break;

                // default:
                // break;
            }
            pfmt++;
        }
    }
    va_end(vp);
    return 0;
}

int HSEPD_GUI::putchar(uint16_t x, uint16_t y, char ch)
{
    uint16_t charSize;
    uint16_t offset;
    uint8_t fontWidth;
    if (_fontHeight % 8 == 0)
    {
        charSize = (_fontHeight / 8) * _fontWidth;
    }
    else
    {
        charSize = (_fontHeight / 8 + 1) * _fontWidth;
    }
    if (_fontVariable == 1)
    {
        charSize += 2; //前两字节存放宽度信息
    }
    offset = ch * charSize;
    File f = EPDFs->open(_fontIndex, "r");
    f.seek(offset, SeekSet);
    uint8_t *data = new uint8_t[charSize];
    f.read(data, charSize);
    if (_fontVariable == 1)
    {
        fontWidth = *(data + 1); //默认不支持255像素以上的字体
        if (DrawImageArr(x, y, fontWidth, _fontHeight, data + 2) == false)
        {
            EPD_LOGW("Draw char fail.");
            return -1;
        }
    }
    else
    {
        fontWidth = _fontWidth;
        if (DrawImageArr(x, y, _fontWidth, _fontHeight, data) == false)
        {
            EPD_LOGW("Draw char fail.");
            return -1;
        }
    }
    delete[] data;
    EPD_LOGD("Draw char success.");
    return fontWidth;
}