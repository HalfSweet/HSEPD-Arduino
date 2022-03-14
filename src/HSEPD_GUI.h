#pragma once
#include "HSEPD_BASIC.h"
#include <Arduino.h>

#ifndef NOT_USE_FS
#include <FS.h>
#endif // !NOT_USE_FS

#define SetBit(Bit, Num) (Bit |= (1 << (Num)))         //将对应的bit位置1
#define ResBit(Bit, Num) (Bit &= ((1 << (Num)) ^ 255)) //将对应的bit位置0

enum class ORIGIN /*屏幕原点位置*/
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

enum class Gray
{
    Gray2 = 0,  //仅黑白
    Gray4 = 1,  // 4灰度
    Gray16 = 2, // 16灰度
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

    uint8_t _fontWidth;     //字体宽度
    uint8_t _fontHeight;    //字体高度
    bool _fontVariable;     //是否为等宽字体，0为定宽，1为等宽
    const char *_fontIndex; //字体在文件系统中的名称

    uint16_t _streamCursorX = 0; //<<方式输出的坐标
    uint16_t _streamCursorY = 0;

    uint16_t _printfBufferLen = 64; // printf的缓冲区大小

    Gray _gray = Gray::Gray2; //屏幕的灰度，默认2灰度

    

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
    bool GUIBegin(uint16_t width, uint16_t height, ORIGIN origin = ORIGIN::TL, Gray gary = Gray::Gray2); //默认原点在左上,2灰度
    void GUIEnd();
    void ClearBuffer();
    void FontBegin(const char *fontIndex, bool variable, uint8_t height, uint8_t width);
    void SetPrintfBufferLen(uint16_t printfBufferLen);
    void SetStreamCursor(uint16_t x, uint16_t y);
    // void FontEnd();
    int printf(uint16_t x, uint16_t y, const char *format, ...);
    bool DrawPixel(uint16_t x, uint16_t y, int16_t color = -1); //白色为0，黑色为-1，对应的灰度则为其数字，注意，最大灰度与-1的黑色等价
    bool DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, int16_t color = -1);
    bool DrawStraightLine(bool direction, uint16_t fixed, uint16_t move0, uint16_t move1, uint16_t lineWeidth = 1, int16_t color = -1);
    bool DrawSolidBox(uint16_t x, uint16_t y, uint16_t weidth, uint16_t height, int16_t color = -1);
    bool DrawHollowBox(uint16_t x, uint16_t y, uint16_t weidth, uint16_t height, int16_t color = -1, uint16_t lineWeidth = 1);
    bool DrawSoildCircle(uint16_t x, uint16_t y, uint16_t radius, int16_t color = -1);
    bool DrawHollowCircle(uint16_t x, uint16_t y, uint16_t radius, int16_t color = -1, uint16_t lineWeidth = 1);
    bool DrawImageArr(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *image);

    bool DrawAbsolutePixel(uint16_t x, uint16_t y, int16_t color = -1);
};

template <typename Type>
HSEPD_GUI &operator<<(HSEPD_GUI &epd, Type str)
{
    epd.putstr(&epd._streamCursorX, &epd._streamCursorY, String(str).c_str());
    return epd;
}