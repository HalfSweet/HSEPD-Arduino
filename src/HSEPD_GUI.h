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
protected:
    ORIGIN _origin;

    bool ToRealPixel(uint16_t x, uint16_t y, uint16_t &realX, uint16_t &realY);

private:
/* data */
#ifndef NOT_USE_FS
    fs::FS *EPDFs;
#endif // !NOT_USE_FS
    uint16_t _width;
    uint16_t _height;
    uint16_t _realWidth; //这两个都是真实的“窗口”的长宽
    uint16_t _realHeight;

    uint8_t _fontWidth;
    uint8_t _fontHeight;
    bool _fontVariable;
    const char *_fontIndex;

    uint16_t _streamCursorX = 0;
    uint16_t _streamCursorY = 0;

    uint16_t _printfBufferLen = 64;

    bool DrawAbsolutePixel(uint16_t x, uint16_t y, COLOR color = COLOR::black);

    int8_t UTF8toUNICODE(const uint8_t *utf8, uint16_t *unicode);

    /**
     * @brief 在屏幕缓存中画一个字符
     *
     * @param x 开始的x坐标
     * @param y 开始的y坐标
     * @param ch 打印的字符编码（Unicode，且最大为0xFFFF）
     * @return int 绘制的该字符的宽度
     */
    int putchar(uint16_t x, uint16_t y, uint16_t ch);

    /**
     * @brief 在屏幕缓存中画一个字符串
     *
     * @param x 开始的x坐标（传入的是指针，因此绘制结束之后光标位置也会改变）
     * @param y 开始的y坐标（传入的是指针，因此绘制结束之后光标位置也会改变）
     * @param str 需要打印的字符串
     * @param nor 控制是否不打印%开头（用于printf中打印一般的字符串）
     * @return int 这个字符串中的uint8_t的个数
     */
    int putstr(uint16_t *x, uint16_t *y, const char *str, bool nor = 0);

    template <typename Type>
    friend HSEPD_GUI &operator<<(HSEPD_GUI &, Type);

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
    void FontBegin(const char *fontIndex, bool variable, uint8_t height, uint8_t width);
    void SetPrintfBufferLen(uint16_t printfBufferLen);
    void SetStreamCursor(uint16_t x, uint16_t y);
    // void FontEnd();
    int printf(uint16_t x, uint16_t y, const char *format, ...);
    bool DrawPixel(uint16_t x, uint16_t y, COLOR color = COLOR::black);
    bool DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, COLOR color = COLOR::black);
    bool DrawStraightLine(bool direction, uint16_t fixed, uint16_t move0, uint16_t move1, uint16_t lineWeidth = 1, COLOR color = COLOR::black);
    bool DrawSolidBox(uint16_t x, uint16_t y, uint16_t weidth, uint16_t height, COLOR color = COLOR::black);
    bool DrawHollowBox(uint16_t x, uint16_t y, uint16_t weidth, uint16_t height, uint16_t lineWeidth = 1);
    bool DrawSoildCircle(uint16_t x, uint16_t y, uint16_t radius, COLOR color = COLOR::black);
    bool DrawHollowCircle(uint16_t x, uint16_t y, uint16_t radius, uint16_t lineWeidth = 1);
    bool DrawImageArr(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *image);
};

template <typename Type>
HSEPD_GUI &operator<<(HSEPD_GUI &epd, Type str)
{
    epd.putstr(&epd._streamCursorX, &epd._streamCursorY, String(str).c_str());
    return epd;
}