#pragma once

#if defined(ARDUINO) // Arduino平台下
#include <Arduino.h>
#include <SPI.h> //因为是硬件SPI，所以加上SPI库
#endif

#include "../../HSEPD_BUS.h"

const int32_t ESP8266_HARD_SPI_SPEED = 100000; // 通信的速度

class ESP8266HardSPI : public HSEPD_BUS
{
private:
    SPIClass *_spi;

    void _CS_LOW();
    void _CS_HIGH();
    void _DC_LOW();
    void _DC_HIGH();
    int _BUSY_READ();

protected:
    int8_t _dc;
    int8_t _busy;
    int8_t _cs;

public:
    ESP8266HardSPI(int8_t dc, int8_t busy, int8_t cs = HSEPD_NOT_USE_PIN, int32_t speed = -1, SPIClass *spi = &SPI);
    ~ESP8266HardSPI();

    void Write(uint8_t value) override;
    void WriteCommand(uint8_t command) override;
    void WriteData(uint8_t data) override;
    void WriteBytes(uint8_t *data, uint8_t dataLen) override;
    void WriteCMDDATA(uint8_t *value, uint8_t dataLen) override;

    uint8_t Read() override;
    uint8_t ReadData(uint8_t command) override;
    void ReadData(uint8_t command, uint8_t dataLen, uint8_t *data) override;
};
