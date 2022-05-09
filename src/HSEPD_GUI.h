#pragma once
#include "HSEPD_BASIC.h"
#include <Arduino.h>

#ifndef NOT_USE_FS
#include <FS.h>
#endif // !NOT_USE_FS

enum class ORIGIN /*屏幕原点位置*/
{
    /* TL,
    BL,
    TR,
    BR, */
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

    uint16_t _streamCursorX = 0; //流输出方式输出的坐标
    uint16_t _streamCursorY = 0;
    uint16_t _streamColor; //流输出方式的字符灰度

    uint16_t _printfBufferLen = 64; // printf的缓冲区大小

    Gray _gray = Gray::Gray2; //屏幕的灰度，默认2灰度

    int8_t UTF8toUNICODE(const uint8_t *utf8, uint16_t *unicode);

    /**
     * @brief 在屏幕缓存中画一个字符
     *
     * @param x 开始的x坐标
     * @param y 开始的y坐标
     * @param ch 打印的字符编码（Unicode，且最大为0xFFFF）
     * @param color 要打印字符的颜色
     * @return int 绘制的该字符的宽度
     */
    int putchar(uint16_t x, uint16_t y, uint16_t ch, int16_t color = -1);

    /**
     * @brief 在屏幕缓存中画一个字符串
     *
     * @param x 开始的x坐标（传入的是指针，因此绘制结束之后光标位置也会改变）
     * @param y 开始的y坐标（传入的是指针，因此绘制结束之后光标位置也会改变）
     * @param str 需要打印的字符串
     * @param nor 控制是否不打印%开头（用于printf中打印一般的字符串）
     * @return int 这个字符串中的uint8_t的个数
     */
    int putstr(uint16_t *x, uint16_t *y, const char *str, int16_t color = -1, bool nor = 0);

    template <typename Type>
    friend HSEPD_GUI &operator<<(HSEPD_GUI &, Type);

   

    bool DrawBWImageArrToGray(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *image, int16_t color);

public:
    uint8_t *DisBuffer;
    HSEPD_GUI(/* args */);
    ~HSEPD_GUI();

    /**
     * @brief 设置屏幕的原点位置
     *
     * @param origin 屏幕的原点位置
     */
    void SetOrigin(ORIGIN origin);

    /**
     * @brief 查询屏幕原点位置
     *
     * @return ORIGIN 屏幕的原点位置
     */
    ORIGIN GetOrigin();

    /**
     * @brief 设置使用的文件系统
     *
     * @param fs 使用文件系统的指针
     */
    void SetFS(fs::FS *fs);

    /**
     * @brief 初始化GUI绘图库
     *
     * @param width 屏幕的宽度分辨率
     * @param height 屏幕的长度分辨率
     * @param origin 屏幕原点位置，默认为左上角
     * @param gary 绘图库使用的灰度，默认为2bpp
     * @return true 初始化成功
     * @return false 初始化失败（一般都是堆内存不够）
     */
    bool GUIBegin(uint16_t width, uint16_t height, ORIGIN origin = ORIGIN::TopLeft, Gray gary = Gray::Gray2);

    /**
     * @brief 关闭GUI绘图库，释放堆内存
     *
     */
    void GUIEnd();

    /**
     * @brief 清除缓存空间
     *
     */
    void ClearBuffer();

    /**
     * @brief 使用字体初始化
     *
     * @param fontIndex 字体在文件系统中的文件名
     * @param variable 是否为等宽模式，0为定宽，1为等宽
     * @param height 字体的高度
     * @param width 字体的宽度（不管是不是等宽字体，都是原始宽度）
     */
    void FontBegin(const char *fontIndex, bool variable, uint8_t height, uint8_t width);

    /**
     * @brief 设置printf函数的缓冲区，默认为64字节
     *
     * @param printfBufferLen 缓冲区大小的字节数
     */
    void SetPrintfBufferLen(uint16_t printfBufferLen);

    /**
     * @brief 设置使用“<<”流输出时的字体坐标
     *
     * @param x x轴坐标
     * @param y y轴坐标
     * @param color 绘制的灰度颜色，0为白色，-1为纯黑，其余的数字对应级别的灰度
     */
    void SetStreamCursor(uint16_t x, uint16_t y, int16_t color = -1);
    // void FontEnd();

    /**
     * @brief 使用printf的方式在缓冲区绘图
     *
     * @param x 绘制的字符的x轴坐标
     * @param y 绘制的字符的y轴坐标
     * @param format 格式化字符串
     * @param ... 可变参数列表
     * @return int 绘制的字符串的字符数
     */
    int printf(uint16_t x, uint16_t y, const char *format, ...);

    /**
     * @brief 使用printf的方式在缓冲区绘图
     *
     * @param x 绘制的字符的x轴坐标
     * @param y 绘制的字符的y轴坐标
     * @param color 绘制的灰度颜色，0为白色，-1为纯黑，其余的数字对应级别的灰度
     * @param format
     * @param ...
     * @return int
     */
    int printf(uint16_t x, uint16_t y, int16_t color, const char *format, ...);

    /**
     * @brief 在指定坐标画一个像素点
     *
     * @param x 绘制的像素点的x轴坐标
     * @param y 绘制的像素点的y轴坐标
     * @param color 绘制的灰度颜色，0为白色，-1为纯黑，其余的数字对应级别的灰度
     * @return true 绘制成功
     * @return false 绘制失败
     */
    bool DrawPixel(uint16_t x, uint16_t y, int16_t color = -1);

    /**
     * @brief 在屏幕缓冲区画一条线
     *
     * @param x0 起始点x轴的坐标
     * @param y0 起始点y轴的坐标
     * @param x1 结束点x轴的坐标
     * @param y1 结束点y轴的坐标
     * @param color 绘制的灰度颜色，0为白色，-1为纯黑，其余的数字对应级别的灰度
     * @return true 绘制成功
     * @return false 绘制失败
     */
    bool DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, int16_t color = -1);

    /**
     * @brief 在屏幕缓冲区画一条直线
     *
     * @param direction 选择是画横线还是竖线，0为横线，1为竖线
     * @param fixed 固定的坐标（比如横线就是y轴，竖线就是x轴）
     * @param move0 起始点的坐标
     * @param move1 结束点的坐标
     * @param lineWeidth 线宽
     * @param color 绘制的灰度颜色，0为白色，-1为纯黑，其余的数字对应级别的灰度
     * @return true 绘制成功
     * @return false 绘制失败
     */
    bool DrawStraightLine(bool direction, uint16_t fixed, uint16_t move0, uint16_t move1, uint16_t lineWeidth = 1, int16_t color = -1);

    /**
     * @brief 在屏幕缓冲区画实心矩形
     *
     * @param x 矩形x轴的坐标
     * @param y 矩形y轴的坐标
     * @param weidth 矩形宽度
     * @param height 矩形高度
     * @param color 绘制的灰度颜色，0为白色，-1为纯黑，其余的数字对应级别的灰度
     * @return true 绘制成功
     * @return false 绘制失败
     */
    bool DrawSolidBox(uint16_t x, uint16_t y, uint16_t weidth, uint16_t height, int16_t color = -1);

    /**
     * @brief 在屏幕缓冲区画空心矩形
     *
     * @param x 矩形x轴的坐标
     * @param y 矩形y轴的坐标
     * @param weidth 矩形宽度
     * @param height 矩形高度
     * @param color 绘制的灰度颜色，0为白色，-1为纯黑，其余的数字对应级别的灰度
     * @param lineWeidth 绘制的线宽
     * @return true 绘制成功
     * @return false 绘制失败
     */
    bool DrawHollowBox(uint16_t x, uint16_t y, uint16_t weidth, uint16_t height, int16_t color = -1, uint16_t lineWeidth = 1);

    /**
     * @brief 在屏幕缓冲区画实心圆
     *
     * @param x 圆心x轴坐标
     * @param y 圆心y轴坐标
     * @param radius 圆的半径
     * @param color 绘制的灰度颜色，0为白色，-1为纯黑，其余的数字对应级别的灰度
     * @return true 绘制成功
     * @return false 绘制失败
     */
    bool DrawSoildCircle(uint16_t x, uint16_t y, uint16_t radius, int16_t color = -1);

    /**
     * @brief 在屏幕缓冲区画空心圆
     *
     * @param x 圆心x轴坐标
     * @param y 圆心y轴坐标
     * @param radius 圆的半径
     * @param color 绘制的灰度颜色，0为白色，-1为纯黑，其余的数字对应级别的灰度
     * @param lineWeidth 空心圆的线宽
     * @return true 绘制成功
     * @return false 绘制失败
     */
    bool DrawHollowCircle(uint16_t x, uint16_t y, uint16_t radius, int16_t color = -1, uint16_t lineWeidth = 1);

    /**
     * @brief 在屏幕缓冲区画图片数组
     *
     * @param x 图片的x轴起始坐标
     * @param y 图片的y轴起始坐标
     * @param width 图片的宽度
     * @param height 图片的高度
     * @param image 图片数组退化的指针
     * @return true 绘制成功
     * @return false 绘制失败
     */
    bool DrawImageArr(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *image);

     bool DrawAbsolutePixel(uint16_t x, uint16_t y, int16_t color = -1);
};

/**
 * @brief 使用流输出的方式在屏幕缓冲区绘制字符串
 *
 * @tparam Type 需要显示的字符串的原始类型
 * @param epd GUI库的类引用
 * @param str 需要显示的内容
 * @return HSEPD_GUI& GUI库的类引用
 */
template <typename Type>
HSEPD_GUI &operator<<(HSEPD_GUI &epd, Type str)
{
    epd.putstr(&epd._streamCursorX, &epd._streamCursorY, String(str).c_str(), epd._streamColor);
    return epd;
}