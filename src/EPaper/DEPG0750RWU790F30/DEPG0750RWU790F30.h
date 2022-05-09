#pragma once

#include "../../HSEPD_BASIC.h"
#include "WAVEFORM_SETTING_LUT.h"
#include <Arduino.h>

class DEPG0750RWU790F30 : public HSEPD_BASIC
{
private:
    const uint16_t xDot = 800;
    const uint16_t yDot = 480;

    DisMode _disMode;

    void BasicInit();
    bool DisplayFull(uint8_t *buffer);
    bool DisplayPart(uint16_t xStart, uint16_t xEnd, uint16_t yStart, uint16_t yEnd, uint8_t *buffer);
    bool Display4GrayFull(uint8_t *buffer); //两种原生四灰均适用
    bool Display16GrayFull(uint8_t *buffer);

    bool WaitBUSY(uint32_t timeOut = 200);
    void SetRamPointer(uint16_t addrX, uint8_t addrY, uint8_t addrY1);
    void SetRamArea(uint16_t Xstart, uint16_t Xend, uint8_t Ystart, uint8_t Ystart1, uint8_t Yend, uint8_t Yend1);
    void WriteDispRam(uint16_t XSize, uint16_t YSize, uint8_t *Dispbuff, uint16_t offset, uint8_t address = 0x13);

public:
    DEPG0750RWU790F30(bool SPIMode, uint8_t CS, uint8_t RST, uint8_t DC, uint8_t BUSY, uint8_t SCK = 255, uint8_t SDA = 255);
    ~DEPG0750RWU790F30();

    void Init(DisMode disMode) override;
    bool Display(uint8_t *buffer, uint16_t xStart, uint16_t xEnd, uint16_t yStart, uint16_t yEnd ) override;
    void Update() override;
    void DeepSleep() override;
};