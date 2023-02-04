#include "ESP8266SoftSPI.h"

#if defined(ARDUINO) && defined(ESP8266) // Arduino平台下

ESP8266SoftSPI::ESP8266SoftSPI(int8_t dc, int8_t busy, int8_t cs, int8_t sck, int8_t sda)
    : HSEPD_BUS(),
      _dc(dc),
      _busy(busy),
      _cs(cs),
      _sck(sck),
      _sda(sda),
      _speed(-1)
{
    if (_cs >= 0)
    {
        pinMode(_cs, OUTPUT);
    }
    pinMode(dc, OUTPUT);
    pinMode(busy, INPUT);
    pinMode(sck, OUTPUT);
    pinMode(sda, OUTPUT);
}

ESP8266SoftSPI::~ESP8266SoftSPI()
{
}

inline void ESP8266SoftSPI::_CS_LOW()
{
    if (_cs >= 0)
    {
        digitalWrite(_cs, LOW);
    }
}

inline void ESP8266SoftSPI::_CS_HIGH()
{
    if (_cs > 0)
    {
        digitalWrite(_cs, HIGH);
    }
}

inline void ESP8266SoftSPI::_DC_LOW()
{
    digitalWrite(_dc, LOW);
}

inline void ESP8266SoftSPI::_DC_HIGH()
{
    digitalWrite(_dc, HIGH);
}

inline int ESP8266SoftSPI::_BUSY_READ()
{
    return digitalRead(_busy);
}

inline void ESP8266SoftSPI::_SCK_HIGH()
{
    digitalWrite(_sck, HIGH);
}

inline void ESP8266SoftSPI::_SCK_LOW()
{
    digitalWrite(_sck, LOW);
}

inline void ESP8266SoftSPI::_SDA_HIGH()
{
    digitalWrite(_sda, HIGH);
}

int ESP8266SoftSPI::_SDA_READ()
{
    return digitalRead(_sda);
}

inline void ESP8266SoftSPI::_SetMOSI()
{
    pinMode(_SDA, OUTPUT);
}

inline void ESP8266SoftSPI::_SetMISO()
{
    pinMode(_SDA, INPUT);
}

inline void ESP8266SoftSPI::_SDA_LOW()
{
    digitalWrite(_sda, LOW);
}

void ESP8266SoftSPI::Write(uint8_t value)
{
    _SCK_LOW();
    // delayMicroseconds(1);
    for (int i = 0; i < 8; i++)
    {
        // 高位在前发送方式    根据升级器件特性定
        if ((value & 0x80) == 0x80)
        {
            _SDA_HIGH();
        }
        else
        {
            _SDA_LOW();
        }
        // delayMicroseconds(1);      //等待数据稳定  根据实际时钟调整
        _SCK_HIGH(); // 上升沿发送数据
        // delayMicroseconds(1);//CLK高电平保持一段时间 这个可以不需要 根据具体的spi时钟来确定
        _SCK_LOW();         // 把时钟拉低实现为下一次上升沿发送数据做准备
        value = value << 1; // 发送数据的位向前移动一位
    }
}

inline void ESP8266SoftSPI::WriteCommand(uint8_t command)
{
    _CS_LOW();
    _DC_LOW();
    Write(command);
    _CS_HIGH();
}

inline void ESP8266SoftSPI::WriteData(uint8_t data)
{
    _CS_LOW();
    _DC_HIGH();
    Write(data);
    _CS_HIGH();
}

void ESP8266SoftSPI::WriteBytes(uint8_t *data, uint8_t dataLen)
{
    for (i = 0; i < Datalen; i++)
    {
        Write(*data);
        ptemp++;
    }
}

inline void ESP8266SoftSPI::WriteCMDDATA(uint8_t *value, uint8_t dataLen)
{
    _CS_LOW();
    _DC_LOW();
    Write(value[0]);
    _DC_HIGH();
    WriteBytes(value + 1, dataLen - 1);
    _CS_HIGH();
}

uint8_t ESP8266SoftSPI::Read()
{
    uint8_t readData = 0;
    for (int i = 0; i < 8; i++)
    {
        _SCK_HIGH;
        readData <<= 1;
        if (_SDA_READ == HIGH)
        {
            readData++;
        }
        _SCK_LOW;
    }
    return readData;
}

uint8_t ESP8266SoftSPI::ReadData(uint8_t command)
{
    uint8_t readData = 0;
    _CS_LOW();
    _DC_LOW();
    Write(command);
    _DC_HIGH();
    readData = Read();
    _CS_HIGH();
    return readData;
}

void ESP8266SoftSPI::ReadData(uint8_t command, uint8_t dataLen, uint8_t *data)
{
    _CS_LOW();
    _DC_LOW();
    Write(command);
    _DC_HIGH();
    for (int i = 0; i < dataLen; i++)
    {
        data[i] = Read();
    }
    _CS_HIGH();
}

#endif