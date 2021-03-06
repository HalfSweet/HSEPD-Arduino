#include "HSEPD_GUI.h"

HSEPD_GUI::HSEPD_GUI(/* args */)
{
}

HSEPD_GUI::~HSEPD_GUI()
{
}

bool HSEPD_GUI::ToRealPixel(uint16_t x, uint16_t y, uint16_t &realX, uint16_t &realY)
{
    switch (_origin)
    {
    //case ORIGIN::TL:
    case ORIGIN::TopLeft:
        if (x >= _width || y >= _height)
        {
            return false;
        }
        realX = x;
        realY = _height - y - 1;
        break;

    //case ORIGIN::BL:
    case ORIGIN::BottomLeft:
        if (x >= _height || y >= _height)
        {
            return false;
        }
        realX = y;
        realY = x;
        break;

    //case ORIGIN::TR:
    case ORIGIN::TopRight:
        if (x >= _width || y >= _height)
        {
            return false;
        }
        realX = _width - y - 1;
        realY = _height - x - 1;
        break;

    //case ORIGIN::BR:
    case ORIGIN::BottomRight:

        if (x >= _height || y >= _width)
        {
            return false;
        }
        realX = _width - x - 1;
        realY = y;
        break;

    default:
        EPD_LOGW("Please enter correct coordinates.");
        return false;
        break;
    }
    return true;
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

bool HSEPD_GUI::GUIBegin(uint16_t width, uint16_t height, ORIGIN origin, Gray gary)
{
    uint32_t pixel;
    if (height % 8 == 0)
    {
        pixel = (height / 8) * width;
    }
    else
    {
        pixel = (height / 8 + 1) * width;
    }
    switch (gary)
    {
    case Gray::Gray2: // 2灰度啥都不干，保持原状
        break;

    case Gray::Gray4: // 4灰度的话buffer宽度得*2
        pixel *= 2;
        break;

    case Gray::Gray16: // 16灰度的话buffer宽度得*4
        pixel *= 4;
        break;
    }
    DisBuffer = new (std::nothrow) uint8_t[pixel];
    if (DisBuffer == nullptr)
    {
        EPD_LOGE("Your RAM is too low, please check it.");
        return false;
    }
    switch (origin)
    {
    //case ORIGIN::BL:
    case ORIGIN::BottomLeft:
    //case ORIGIN::TR:
    case ORIGIN::TopRight:
        _width = height;
        _height = width;
        break;

    //case ORIGIN::BR:
    case ORIGIN::BottomRight:
    //case ORIGIN::TL:
    case ORIGIN::TopLeft:
        _width = width;
        _height = height;
        break;

    default:
        break;
    }

    SetOrigin(origin);
    _realWidth = width;
    _realHeight = height;
    _gray = gary;

    ClearBuffer();

    EPD_LOGD("GUIBegin finish.");
    return true;
}

void HSEPD_GUI::GUIEnd()
{
    delete[] DisBuffer;
}

bool HSEPD_GUI::DrawAbsolutePixel(uint16_t x, uint16_t y, int16_t color)
{
    if (x >= _width || y >= _height)
    {
        EPD_LOGE("The coordinates you entered are outside the window.");
        return false;
    }

    switch (_gray)
    {
    case Gray::Gray2:
        switch (color) //此处需要注意，白色为1，黑色为0
        {
        case 0:
            DisBuffer[(x + y * _width) / 8] |= 0x80 >> (x % 8);
            break;

        case -1:
        case 1:
            DisBuffer[(x + y * _width) / 8] &= ~(0x80 >> (x % 8));
            break;

        default:
            EPD_LOGW("Please enter the correct color.");
            return false;
            break;
        }
        break;

    case Gray::Gray4:
        if (color > 3)
        {
            EPD_LOGW("Please enter the correct color.");
            return false;
        }
        else if (color == -1)
        {
            SetBit(DisBuffer[(x + y * _width) / 4], 7 - 2 * (x % 4));
            SetBit(DisBuffer[(x + y * _width) / 4], 7 - (2 * (x % 4) + 1));
        }
        else
        {
            for (int i = 0; i < 2; i++)
            {
                if (((color & (0x01 << i)) >> i) == 0)
                {
                    ResBit(DisBuffer[(x + y * _width) / 4], 7 - (2 * (x % 4) + 1 - i));
                }
                else
                {
                    SetBit(DisBuffer[(x + y * _width) / 4], 7 - (2 * (x % 4) + 1 - i));
                }
            }
        }

        break;

    case Gray::Gray16:
        if (color > 15)
        {
            EPD_LOGW("Please enter the correct color.");
            return false;
        }
        else if (color == -1)
        {
            SetBit(DisBuffer[(x + y * _width) / 2], 7 - 4 * (x % 2));
            SetBit(DisBuffer[(x + y * _width) / 2], 7 - (4 * (x % 2) + 1));
            SetBit(DisBuffer[(x + y * _width) / 2], 7 - (4 * (x % 2) + 2));
            SetBit(DisBuffer[(x + y * _width) / 2], 7 - (4 * (x % 2) + 3));
        }
        else
        {
            for (int i = 0; i < 4; i++)
            {
                if (((color & (0x01 << i)) >> i) == 0)
                {
                    ResBit(DisBuffer[(x + y * _width) / 2], 7 - (4 * (x % 2) + 3 - i));
                }
                else
                {
                    SetBit(DisBuffer[(x + y * _width) / 2], 7 - (4 * (x % 2) + 3 - i));
                }
            }
        }
        break;
    }
    // EPD_LOGV("Draw absolute pixel finish.");
    return true;
}

bool HSEPD_GUI::DrawPixel(uint16_t x, uint16_t y, int16_t color)
{
    uint16_t realX;
    uint16_t realY;
    ToRealPixel(x, y, realX, realY);
    return DrawAbsolutePixel(realX, realY, color);
}

void HSEPD_GUI::ClearBuffer()
{
    uint8_t temp = 0;
    uint8_t color = 0xFF;
    switch (_gray)
    {
    case Gray::Gray2:
        temp = 8;
        color = 0xFF;
        break;
    case Gray::Gray4:
        temp = 4;
        color = 0x00;
        break;
    case Gray::Gray16:
        temp = 2;
        color = 0x00;
        break;
    }
    for (size_t i = 0; i < (_width * _height / temp); i++)
    {
        DisBuffer[i] = color;
    }
}

bool HSEPD_GUI::DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, int16_t color)
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

bool HSEPD_GUI::DrawStraightLine(bool direction, uint16_t fixed, uint16_t move0, uint16_t move1, uint16_t lineWeidth, int16_t color)
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

bool HSEPD_GUI::DrawSolidBox(uint16_t x, uint16_t y, uint16_t weidth, uint16_t height, int16_t color)
{
    if (DrawStraightLine(1, x, y, y + height, weidth, color))
    {
        return false;
    }
    EPD_LOGD("Draw Solid Box finish.");
    return true;
}

bool HSEPD_GUI::DrawHollowBox(uint16_t x, uint16_t y, uint16_t weidth, uint16_t height, int16_t color, uint16_t lineWeidth)
{
    if (lineWeidth == 0)
    {
        EPD_LOGE("Stupid human, are you kidding me?");
        return false;
    }
    if (DrawSolidBox(x, y, weidth, height, color))
    {
        return false;
    }
    DrawSolidBox(x + lineWeidth, y + lineWeidth, weidth - 2 * lineWeidth, height - 2 * lineWeidth, 0);

    EPD_LOGD("Draw Hollow Box finish.");
    return true;
}

bool HSEPD_GUI::DrawSoildCircle(uint16_t x, uint16_t y, uint16_t radius, int16_t color)
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

bool HSEPD_GUI::DrawHollowCircle(uint16_t x, uint16_t y, uint16_t radius, int16_t color, uint16_t lineWeidth)
{
    if (DrawSoildCircle(x, y, radius, color) == false)
    {
        return false;
    }
    DrawSoildCircle(x, y, radius - 2 * lineWeidth, 0);
    return true;
}

bool HSEPD_GUI::DrawImageArr(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *image)
{
    uint8_t px = 0;
    uint8_t bpp = 0; // bpp为一个字节中可以存的下多少个像素，例如2灰度可以存8个，4灰度可以存4个等
    uint8_t color = 0;
    switch (_gray)
    {
    case Gray::Gray2:
        bpp = 8;
        break;

    case Gray::Gray4:
        bpp = 4;
        break;

    case Gray::Gray16:
        bpp = 2;
        break;
    }
    for (size_t cx = 0; cx < width; cx++)
    {
        for (size_t cy = 0; cy < height; cy++)
        {
            color = 0;
            if (cy % bpp == 0)
            {
                px = *image;
                image++;
            }

            for (int i = 0; i < 8 / bpp; i++)
            {
                color += GetBit(px, (7 - (8 / bpp) * (cy % bpp) - i)) << ((8 / bpp) - i - 1);
            }

            if (DrawPixel(x + cx, y + cy, color) == false)
            {
                EPD_LOGW("Draw image fail.");
                return false;
            }
        }
    }

    EPD_LOGD("Draw image success.");
    return true;
}

int8_t HSEPD_GUI::UTF8toUNICODE(const uint8_t *utf8, uint16_t *unicode)
{
    if (*utf8 <= 0x7F) // ascii
    {
        *unicode = *utf8;
        return 1;
    }
    else if ((*utf8 & 0xE0) == 0xC0) //双字节
    {
        //需要注意的是，在低字节 & 0xC0 的结果不为0x80的话，这是一个不合法的UTF8字符串，这里直接返回"\0"，希望用户别瞎往里传参
        if ((*(utf8 + 1) & 0xC0) != 0x80)
        {
            return -1;
        }
        *unicode = (((*utf8 >> 2) & 0x07) << 8) + (uint8_t((*utf8 << 6) + (*(utf8 + 1) & 0x3F))); //别问我为啥这么写，懒。不爽的话你来打我啊
        return 2;
    }
    else if ((*utf8 & 0xF0) == 0xE0) //三字节
    {
        //同理，在第二和第三个字节 & 0xC0 的结果不为0x80的话，该字符串不合法
        if ((*(utf8 + 1) & 0xC0) != 0x80 || (*(utf8 + 2) & 0xC0) != 0x80)
        {
            return -1;
        }
        *unicode = (((*utf8 << 4) + (((*(utf8 + 1) >> 2) & 0x0F))) << 8) + (uint8_t((*(utf8 + 1) << 6) + (*(utf8 + 2) & 0x7F)));
        return 3;
    }
    else //还想有四字节？爬！只支持最大0xFFFF的Unicode字符。如果你不想有这个限制，请速速来帮我写取模软件
    {
        return -1;
    }
}

void HSEPD_GUI::SetPrintfBufferLen(uint16_t printfBufferLen)
{
    _printfBufferLen = printfBufferLen;
}

void HSEPD_GUI::SetStreamCursor(uint16_t x, uint16_t y,int16_t color)
{
    _streamCursorX = x;
    _streamCursorY = y;
    _streamColor = color;
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
    va_list arg;
    va_start(arg, format);
    int a=printf(x, y, -1, format);
    va_end(arg);
    return a;
}
int HSEPD_GUI::printf(uint16_t x, uint16_t y, int16_t color ,const char *format, ...)
{
    size_t num;
    va_list arg;
    va_start(arg, format);
    char *str = new char[_printfBufferLen]; //字符串缓冲区大小，默认64
    num = vsnprintf(str, _printfBufferLen, format, arg);
    va_end(arg);
    putstr(&x, &y, str,color);
    delete[] str;

    EPD_LOGD("Printf is success.");
    return num;
}

int HSEPD_GUI::putchar(uint16_t x, uint16_t y, uint16_t ch, int16_t color)
{
    uint16_t charSize;
    uint32_t offset;
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
    // EPD_LOGV("offset:%d",offset);
    File f = EPDFs->open(_fontIndex, "r");
    f.seek(offset, SeekSet);
    uint8_t *data = new uint8_t[charSize];
    f.read(data, charSize);
    if (_fontVariable == 1)
    {
        fontWidth = *(data + 1); //默认不支持255像素以上的字体
        if (DrawBWImageArrToGray(x, y, fontWidth, _fontHeight, data + 2, color) == false)
        {
            EPD_LOGW("Draw char fail.");
            return -1;
        }
    }
    else
    {
        fontWidth = _fontWidth;
        if (DrawBWImageArrToGray(x, y, _fontWidth, _fontHeight, data, color) == false)
        {
            EPD_LOGW("Draw char fail.");
            return -1;
        }
    }
    delete[] data;
    // EPD_LOGD("Draw char success.");
    return fontWidth;
}


int HSEPD_GUI::putstr(uint16_t *x, uint16_t *y, const char *str,int16_t color, bool nor)
{
    uint16_t disX = *x;
    uint16_t disY = *y;
    uint16_t unicodeCode = 0;
    int charNum = 0;
    uint32_t charSum = 0;
    uint16_t strWidth = 0;
    while (*str)
    {
        if (nor == 1)
        {
            if (*str == '%')
            {
                *x = disX;
                *y = disY;
                return charSum;
            }
        }
        charNum = UTF8toUNICODE((uint8_t *)str, &unicodeCode);
        charSum += charNum;
        if (charNum == -1)
        {
            EPD_LOGW("Your string is not valid");
            return charSum;
        }
        strWidth = putchar(disX, disY, unicodeCode,color);
        if (disX + strWidth >= _realWidth) //如果已经没有位置塞下新的一个字符了,那就换行
        {
            disX = *x;
            if (disY + _fontHeight > _realHeight) //如果换行之后发现还是塞不下
            {
                EPD_LOGW("You don't have enough screen space to show the next character.");
                return charSum;
            }
            disY += _fontHeight;
            // EPD_LOGV("换行");
        }
        else
        {
            disX += strWidth;
        }
        str += charNum;
    }
    *x = disX;
    *y = disY;
    EPD_LOGD("Draw string success.");
    return charSum;
}

bool HSEPD_GUI::DrawBWImageArrToGray(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *image, int16_t color)
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
                if (DrawPixel(x + cx, y + cy,color) == false)
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