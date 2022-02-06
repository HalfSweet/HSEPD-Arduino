#pragma once

#include <Arduino.h>
#include "HSEPD_BASIC.h"

class HSEPD : protected HSEPD_BASIC
{
private:
public:
    HSEPD(bool SPIMode, uint8_t CS, uint8_t RST, uint8_t DC, uint8_t BUSY, uint8_t SCK = 255, uint8_t SDA = 255);
    ~HSEPD();
};
