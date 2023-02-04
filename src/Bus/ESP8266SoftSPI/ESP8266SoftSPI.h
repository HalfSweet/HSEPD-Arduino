#pragma once

#include "../../HSEPD_BUS.h"

#if defined(ARDUINO) && defined(ESP8266) // Arduino平台下

class ESP8266SoftSPI : public HSEPD_BUS
{
private:

    void _CS_LOW();
    void _CS_HIGH();
    void _DC_LOW();
    void _DC_HIGH();
    int _BUSY_READ();

    void _SCK_LOW();
    void _SCK_HIGH();
    void _SDA_LOW();
    void _SDA_HIGH();

    int _SDA_READ();

    void _SetMOSI();
    void _SetMISO();

protected:
    int8_t _dc;
    int8_t _busy;
    int8_t _cs;
    int8_t _sck;
    int8_t _sda;

public:
    ESP8266SoftSPI(int8_t dc, int8_t busy, int8_t cs = HSEPD_NOT_USE_PIN, int8_t sck = PIN_SPI_SCK, int8_t mosi = PIN_SPI_MOSI, int32_t speed = -1);
    ~ESP8266SoftSPI();

    void Write(uint8_t value) override;
    void WriteCommand(uint8_t command) override;
    void WriteData(uint8_t data) override;
    void WriteBytes(uint8_t *data, uint8_t dataLen) override;
    void WriteCMDDATA(uint8_t *value, uint8_t dataLen) override;

    uint8_t Read() override;
    uint8_t ReadData(uint8_t command) override;
    void ReadData(uint8_t command, uint8_t dataLen, uint8_t *data) override;
};

#endif