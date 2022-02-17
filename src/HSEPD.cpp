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

    default:
        EPD_LOGE("The screen you have selected is not currently supported");
        break;
    }
}

void HSEPD::InitFull()
{
    return epd->InitFull();
}

void HSEPD::InitPart()
{
    return epd->InitPart();
}

void HSEPD::DeepSleep()
{
    return epd->DeepSleep();
}

bool HSEPD::DisplayFull(uint8_t *buffer)
{
    return epd->DisplayFull(buffer);
}