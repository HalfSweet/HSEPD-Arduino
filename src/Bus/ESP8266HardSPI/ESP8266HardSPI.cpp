#include "ESP8266HardSPI.h"

#if defined(ARDUINO) && defined(ESP8266)// Arduino平台下
ESP8266HardSPI::ESP8266HardSPI(int8_t dc, int8_t busy, int8_t cs , int32_t speed,SPIClass *spi)
    : HSEPD_BUS(),
      _dc(dc),
      _busy(busy),
      _cs(cs),
      _speed(speed),
      _spi(spi)
{
    if (_cs >= 0)
    {
        pinMode(_cs, OUTPUT);
    }
    pinMode(dc, OUTPUT);
    pinMode(busy, INPUT);

    if(_speed > 0)
    {
        _spi->setFrequency(_speed);
    }
}

ESP8266HardSPI::~ESP8266HardSPI()
{
}

inline void ESP8266HardSPI::_CS_LOW()
{
    if (_cs >= 0)
    {
        digitalWrite(_cs, LOW);
    }
}

inline void ESP8266HardSPI::_CS_HIGH()
{
    if (_cs > 0)
    {
        digitalWrite(_cs, HIGH);
    }
}

inline void ESP8266HardSPI::_DC_LOW()
{
    digitalWrite(_dc, LOW);
}

inline void ESP8266HardSPI::_DC_HIGH()
{
    digitalWrite(_dc, HIGH);
}

inline int ESP8266HardSPI::_BUSY_READ()
{
    return digitalRead(_busy);
}

inline void ESP8266HardSPI::Write(uint8_t value)
{
    _spi->transfer(value);
}

inline void ESP8266HardSPI::WriteCommand(uint8_t command)
{
    _CS_LOW();
    _DC_LOW();
    Write(command);
    _CS_HIGH();
}

inline void ESP8266HardSPI::WriteData(uint8_t data)
{
    _CS_LOW();
    _DC_HIGH();
    Write(data);
    _CS_HIGH();
}

inline void ESP8266HardSPI::WriteBytes(uint8_t *data, uint8_t dataLen)
{
    _spi->transfer(data, dataLen);
}

inline void ESP8266HardSPI::WriteCMDDATA(uint8_t *value, uint8_t dataLen)
{
    _CS_LOW();
    _DC_LOW();
    Write(value[0]);
    _DC_HIGH();
    WriteBytes(value + 1, dataLen - 1);
    _CS_HIGH();
}

uint8_t ESP8266HardSPI::Read()
{
    // TODO
    return 0;
}

uint8_t ESP8266HardSPI::ReadData(uint8_t command)
{
    // TODO
    return 0;
}

void ESP8266HardSPI::ReadData(uint8_t command, uint8_t dataLen, uint8_t *data)
{
    // TODO
}

#endif