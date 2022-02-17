#pragma once

#include "../../HSEPD_BASIC.h"
#include "WAVEFORM_SETTING_LUT.h"
#include <Arduino.h>

class HINKE029A01 : public HSEPD_BASIC
{
private:
    const uint16_t xDot = 128;
    const uint16_t yDot = 296;

    bool WaitBUSY(uint32_t timeOut = 200) override;
    void Init();
    void SetRamPointer(uint16_t addrX, uint8_t addrY, uint8_t addrY1) override;
    void SetRamArea(uint16_t Xstart, uint16_t Xend, uint8_t Ystart, uint8_t Ystart1, uint8_t Yend, uint8_t Yend1) override;
    void WriteDispRam(uint16_t XSize, uint16_t YSize, uint8_t *Dispbuff, uint16_t offset) override;

public:
    HINKE029A01(bool SPIMode, uint8_t CS, uint8_t RST, uint8_t DC, uint8_t BUSY, uint8_t SCK = 255, uint8_t SDA = 255);
    ~HINKE029A01();

    void InitFull() override;
    void InitPart() override;
    void FullUpdate() override;
    void PartUpdate();
    void DeepSleep() override;
    bool DisplayFull(uint8_t *buffer) override;
    bool DisplayPart(uint16_t xStart, uint16_t xEnd, uint16_t yStart, uint16_t yEnd,uint8_t *buffer);
};
