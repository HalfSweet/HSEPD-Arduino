#pragma once

#if defined(Arduino)
#include <Arduino.h>
#endif

#include "HSEPD_LOG.h"

#include "HSEPD_BUS.h"
#include "./EPaper/HINKE029A01/HINKE029A01.h"
#include "./EPaper/HINKE029A10/HINKE029A10.h"
#include "./EPaper/DEPG0750RWU790F30/DEPG0750RWU790F30.h"

enum class DisMode
{
    Full = 0,
    Part,
    Gray4AloneFull,
    Gray4OverlapFull,
    Gray16AloneFull,
};

class HSEPD
{
private:
public:
    HSEPD(int16_t width, int16_t height);
    ~HSEPD();

    virtual void Init(DisMode mode = DisMode::Full) = 0;
    virtual void Display(const uint8_t *image) = 0;
    virtual void DeepSleep() = 0;
};
