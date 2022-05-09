#include "HSEPD.h"

HSEPD::HSEPD(bool SPIMode, uint8_t CS, uint8_t RST, uint8_t DC, uint8_t BUSY, uint8_t SCK, uint8_t SDA)
{
    _CS = CS;
    _RST = RST;
    _DC = DC;
    _BUSY = BUSY;
    _SCK = SCK;
    _SDA = SDA;
    _SPIMode = SPIMode;
}

HSEPD::~HSEPD()
{
}

void HSEPD::SetEpaperType(EPDType type)
{
    switch (type)
    {
    case EPDType::HINKE029A01:
        epd = new HINKE029A01(_SPIMode, _CS, _RST, _DC, _BUSY, _SCK, _SDA);
        EPD_LOGD("You selected HINKE029A01");
        break;

    case EPDType::DEPG0750RWU790F30:
        epd = new DEPG0750RWU790F30(_SPIMode, _CS, _RST, _DC, _BUSY, _SCK, _SDA);
        EPD_LOGD("You selected DEPG0750RWU790F30");
        break;

    default:
        EPD_LOGE("The screen you have selected is not currently supported");
        break;
    }
}

void HSEPD::SetHardSPI(SPIClass *spi)
{
    return epd->SetHardSPI(spi);
}

void HSEPD::Init(DisMode disMode)
{
    return epd->Init(disMode);
}

void HSEPD::DeepSleep()
{
    return epd->DeepSleep();
}

bool HSEPD::Display(uint8_t *buffer, uint16_t xStart, uint16_t xEnd, uint16_t yStart, uint16_t yEnd)
{
    uint16_t realXStart = 0;
    uint16_t realYStart = 0;
    uint16_t realXEnd = 0;
    uint16_t realYEnd = 0;
    ToRealPixel(xStart, yStart, realXStart, realYStart);
    ToRealPixel(xEnd, yEnd, realXEnd, realYEnd);
    EPD_LOGV("Display: xStart: %d, yStart: %d, xEnd: %d, yEnd: %d", realXStart, realYStart, realXEnd, realYEnd);
    return epd->Display(buffer, min(realXStart,realXEnd), max(realXStart,realXEnd), min(realYStart,realYEnd), max(realYStart,realYEnd));
}
