#pragma once

#include <Arduino.h>
#include "HSEPD_GUI.h"
#include "./EPaper/HINKE029A01/HINKE029A01.h"
#include "./EPaper/HINKE029A10/HINKE029A10.h"
#include "./EPaper/DEPG0750RWU790F30/DEPG0750RWU790F30.h"

class HSEPD : public HSEPD_GUI
{
private:
    uint8_t _CS;
    uint8_t _RST;
    uint8_t _DC;
    uint8_t _BUSY;
    uint8_t _SCK;
    uint8_t _SDA;
    bool _SPIMode;

    HSEPD_BASIC *epd;

public:
    /**
     * @brief HSEPD的构造函数，确定软硬SPI以及引脚（硬件SPI的情况下可不指定SCK和SDA引脚）
     * 
     * @param SPIMode SPI模式，0为软件SPI，1为硬件SPI
     * @param CS CS引脚
     * @param RST RST引脚
     * @param DC DC引脚
     * @param BUSY BUSY引脚
     * @param SCK SCK引脚
     * @param SDA SDA引脚
     */
    HSEPD(bool SPIMode, uint8_t CS, uint8_t RST, uint8_t DC, uint8_t BUSY, uint8_t SCK = 255, uint8_t SDA = 255);
    ~HSEPD();

    /**
     * @brief 设置屏幕的型号
     * 
     * @param type 屏幕的型号
     */
    void SetEpaperType(EPDType type);

    /**
     * @brief 设置硬件SPI
     * 
     * @param spi 对应的硬件SPI类的指针
     */
    void SetHardSPI(SPIClass *spi);

    /**
     * @brief 墨水屏的初始化，默认为全刷
     * 
     * @param disMode 初始化的类型
     */
    void Init(DisMode disMode = DisMode::Full);

    /**
     * @brief 墨水屏的深度睡眠
     * 
     */
    void DeepSleep();

    /**
     * @brief 让墨水屏显示缓存中的内容
     * 
     * @param buffer 缓存buffer
     * @param xStart 要显示的窗口起始的x轴坐标
     * @param xEnd 要显示的窗口结束的x轴坐标
     * @param yStart 要显示的窗口起始的y轴坐标
     * @param yEnd 要显示的窗口结束的y轴坐标
     * @return true true为成功显示
     * @return false false为显示失败
     */
    bool Display(uint8_t *buffer, uint16_t xStart = 0, uint16_t xEnd = 0, uint16_t yStart = 0, uint16_t yEnd = 0);

};
