#include "HSEPD_BASIC.h"

HSEPD_BASIC::HSEPD_BASIC(bool SPIMode, uint8_t CS, uint8_t RST, uint8_t DC, uint8_t BUSY, uint8_t SCK, uint8_t SDA)
{
    _CS = CS;
    _RST = RST;
    _DC = DC;
    _BUSY = BUSY;
    _SCK = SCK;
    _SDA = SDA;
    _SPIMode = SPIMode;
    if (SPIMode == 0)
    {
        pinMode(CS, OUTPUT); // io初始化
        pinMode(DC, OUTPUT);
        pinMode(RST, OUTPUT);
        pinMode(BUSY, INPUT);
        pinMode(SCK, OUTPUT);
        pinMode(SDA, OUTPUT);
    }
    else
    {
        pinMode(CS, OUTPUT);
        pinMode(DC, OUTPUT);
        pinMode(RST, OUTPUT);
        pinMode(BUSY, INPUT);
    }
    EPD_LOGV("Welcome to use HSEPD");
}

HSEPD_BASIC::~HSEPD_BASIC()
{
}

void HSEPD_BASIC::DriveDelay(uint32_t ms)
{
    delay(ms);
}

void HSEPD_BASIC::SetMOSI()
{
    if (_SPIMode == 1)
    {
        EPDSPI->begin();
    }
    else
    {
        pinMode(_SDA, OUTPUT);
        pinMode(_SCK, OUTPUT);
    }
    EPD_LOGD("SetMOSI over");
}
void HSEPD_BASIC::SetMISO()
{
    if (_SPIMode == 1)
    {
        EPDSPI->end();
    }
    pinMode(_SDA, INPUT);
    pinMode(_SCK, OUTPUT);
    EPD_LOGD("SetMISO over");
}

void HSEPD_BASIC::SPIWrite(uint8_t value)
{
    if (_SPIMode == 1)
    {
        EPDSPI->transfer(value);
    }
    else
    {
        _EPD_SCK_LOW;
        // delayMicroseconds(1);
        for (int i = 0; i < 8; i++)
        {
            //高位在前发送方式    根据升级器件特性定
            if ((value & 0x80) == 0x80)
            {
                _EPD_SDA_HIGH;
            }
            else
            {
                _EPD_SDA_LOW;
            }
            // delayMicroseconds(1);      //等待数据稳定  根据实际时钟调整
            _EPD_SCK_HIGH; //上升沿发送数据
            // delayMicroseconds(1);//CLK高电平保持一段时间 这个可以不需要 根据具体的spi时钟来确定
            _EPD_SCK_LOW;       //把时钟拉低实现为下一次上升沿发送数据做准备
            value = value << 1; //发送数据的位向前移动一位
        }
    }
}

uint8_t HSEPD_BASIC::SPIRead()
{
    uint8_t readData = 0;
    for (int i = 0; i < 8; i++)
    {
        _EPD_SCK_HIGH;
        readData <<= 1;
        if (_READ_EPD_SDA == HIGH)
        {
            readData++;
        }
        _EPD_SCK_LOW;
    }
    return readData;
}

void HSEPD_BASIC::WriteCMD(uint8_t cmd)
{
    _EPD_CS_LOW;
    _EPD_DC_LOW;
    SPIWrite(cmd);
    _EPD_CS_HIGH;
}

void HSEPD_BASIC::WriteDATA(uint8_t data)
{
    _EPD_CS_LOW;
    _EPD_DC_HIGH;
    SPIWrite(data);
    _EPD_CS_HIGH;
}

void HSEPD_BASIC::WriteCMDDATA(uint8_t *value, uint8_t Datalen)
{
    uint8_t i = 0;
    uint8_t *ptemp;
    ptemp = value;

    _EPD_CS_LOW;
    _EPD_DC_LOW;      // When DC is 0, write command
    SPIWrite(*ptemp); // The first uint8_t is written with the command value
    ptemp++;
    _EPD_DC_HIGH; // When DC is 1, write Data
    for (i = 0; i < Datalen - 1; i++)
    { // sub the Data
        SPIWrite(*ptemp);
        ptemp++;
    }
    // WaitBUSY();
    _EPD_CS_HIGH;
}

void HSEPD_BASIC::SetHardSPI(SPIClass *spi)
{
    EPDSPI = spi;
}

void HSEPD_BASIC::GrayArrConvert(uint32_t pixel, uint8_t *gary, uint8_t *data, uint8_t level, uint8_t grayLevel)
{
    grayLevel = 8 / grayLevel;
    for (uint32_t i = 0; i < pixel; i++)
    {
        data[i] = 0x00; //若不初始化，则数据可能随机
        for (int j = 0; j < 8; j++)
        {
            data[i] += (GetBit(gary[(8 / grayLevel) * i + (j / grayLevel)], 7 - ((j - (j / grayLevel) * grayLevel) * (8 / grayLevel)) - level)) << (7 - j);
        }
    }
}