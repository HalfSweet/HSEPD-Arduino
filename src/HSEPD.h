#pragma once

#include <Arduino.h>
#include "HSEPD_GUI.h"
#include "./EPaper/HINKE029A01/HINKE029A01.h"
#include "./EPaper/HINKE029A10/HINKE029A10.h"

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
    HSEPD(bool SPIMode, uint8_t CS, uint8_t RST, uint8_t DC, uint8_t BUSY, uint8_t SCK = 255, uint8_t SDA = 255);
    ~HSEPD();

    void SetEpaperType(EPDType type);
    void SetHardSPI(SPIClass *spi);
    void Init(DisMode disMode = DisMode::Full);
    void DeepSleep();
    bool Display(uint8_t *buffer, uint16_t xStart = 0, uint16_t xEnd = 0, uint16_t yStart = 0, uint16_t yEnd = 0);

};
