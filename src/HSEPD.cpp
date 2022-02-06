#include "HSEPD.h"

HSEPD::HSEPD(bool SPIMode, uint8_t CS, uint8_t RST, uint8_t DC, uint8_t BUSY, uint8_t SCK, uint8_t SDA) : HSEPD_BASIC(SPIMode, CS, RST, DC, BUSY, SCK, SDA)
{
}

HSEPD::~HSEPD()
{
}