#include "HINKE029A01.h"

HINKE029A01::HINKE029A01(bool SPIMode, uint8_t CS, uint8_t RST, uint8_t DC, uint8_t BUSY, uint8_t SCK, uint8_t SDA) : HSEPD_BASIC(SPIMode, CS, RST, DC, BUSY, SCK, SDA)
{
}

HINKE029A01::~HINKE029A01()
{
}

void HINKE029A01::Init()
{
    _EPD_RST_LOW;
    DriveDelay(10);
    _EPD_RST_HIGH;
    DriveDelay(10);
    WriteCMD(0x03);
    WriteDATA(0xEA);

    WriteCMD(0x01);
    WriteDATA(0x27);
    WriteDATA(0x01);
    WriteDATA(0x00);

    WriteCMD(0x0C);
    WriteDATA(0xD7);
    WriteDATA(0xD6);
    WriteDATA(0x9D);

    WriteCMD(0x2C);
    WriteDATA(0xA8);

    WriteCMD(0x3A);
    WriteDATA(0x01);

    WriteCMD(0x3B);
    WriteDATA(0x08);

    WriteCMD(0x11);
    WriteDATA(0x01);

    WriteCMD(0x22);
    WriteDATA(0xC0);

    WriteCMD(0x20);
}

void HINKE029A01::InitFull()
{
    Init();
    WriteCMDDATA((uint8_t *)LUTDefault_full, sizeof(LUTDefault_full));
    EPD_LOGD("Init full finish.");
}

void HINKE029A01::InitPart()
{
    Init();
    WriteCMDDATA((uint8_t *)LUTDefault_part, sizeof(LUTDefault_part));

    WriteCMD(0x22);
    WriteDATA(0xC0);

    WriteCMD(0x20);
    EPD_LOGD("Init part finish.");
}

void HINKE029A01::FullUpdate()
{
    WriteCMD(0x22);
    WriteDATA(0x04);
    WriteCMD(0x20);
}

void HINKE029A01::PartUpdate()
{
    WriteCMD(0x22);
    WriteDATA(0x04);
    WriteCMD(0x20);
}

bool HINKE029A01::WaitBUSY(uint32_t timeOut)
{
    uint32_t currentTime = millis();
    while (1)
    {

        if (_READ_EPD_BUSY == LOW)
        {
            EPD_LOGI("EPD is not busy.");
            return true;
        }
        else if ((millis() - currentTime) > timeOut)
        {
            EPD_LOGW("Waiting has timed out");
            return false;
        }
#ifdef ESP8266

        ESP.wdtFeed();

#endif // ESP8266
    }
}

void HINKE029A01::SetRamPointer(uint16_t addrX, uint8_t addrY, uint8_t addrY1)
{
    WaitBUSY();
    uint8_t RamPointerX[2]; // default (0,0)
    uint8_t RamPointerY[3];
    // Set RAM X address counter
    RamPointerX[0] = 0x4E;
    RamPointerX[1] = addrX;
    // RamPointerX[1] = 0xff;
    // Set RAM Y address counter
    RamPointerY[0] = 0x4F;
    RamPointerY[1] = addrY;
    RamPointerY[2] = addrY1;
    // RamPointerY[1] = 0x2b;
    // RamPointerY[2] = 0x02;

    WriteCMDDATA(RamPointerX, sizeof(RamPointerX));
    WriteCMDDATA(RamPointerY, sizeof(RamPointerY));
    EPD_LOGD("Set ram pointer finish");
}

void HINKE029A01::SetRamArea(uint16_t Xstart, uint16_t Xend, uint8_t Ystart, uint8_t Ystart1, uint8_t Yend, uint8_t Yend1)
{
    WaitBUSY();
    uint8_t RamAreaX[3]; // X start and end
    uint8_t RamAreaY[5]; // Y start and end
    RamAreaX[0] = 0x44;  // command
    RamAreaX[1] = Xstart / 8;
    RamAreaX[2] = Xend / 8;
    RamAreaY[0] = 0x45; // command
    RamAreaY[1] = Ystart;
    RamAreaY[2] = Ystart1;
    RamAreaY[3] = Yend;
    RamAreaY[4] = Yend1;
    WriteCMDDATA(RamAreaX, sizeof(RamAreaX));
    WriteCMDDATA(RamAreaY, sizeof(RamAreaY));
    EPD_LOGD("Set ram area finish");
}

void HINKE029A01::WriteDispRam(uint16_t XSize, uint16_t YSize, uint8_t *buffer, uint16_t offset)
{
    WaitBUSY();
    WriteCMD(0x24);
    uint8_t *buf = buffer;
    buf += offset;
    _EPD_CS_LOW;
    _EPD_DC_HIGH;
    for (int i = 0; i < YSize; i++)
    {
        for (int j = 0; j < XSize; j++)
        {
            SPIWrite(*buf);
            //SPIWrite(0x11);
            buf++;
        }
        buf += xDot / 8 - XSize;
    }
    WaitBUSY();
    _EPD_CS_HIGH;
}

void HINKE029A01::DeepSleep()
{
    WaitBUSY(2000);
    WriteCMD(0x10);
    WriteDATA(0x01);
}

bool HINKE029A01::DisplayFull(uint8_t *buffer)
{
    uint16_t yStart = 0;
    uint16_t yEnd = yDot - 1;
    uint16_t xStart = 0;
    uint16_t xEnd = xDot - 1;
    uint32_t temp = yStart;

    yStart = yDot - 1 - yEnd;
    yEnd = yDot - 1 - temp;

    
    SetRamPointer(xStart / 8, yEnd % 256, yEnd / 256);
    SetRamArea(xStart, xEnd, yEnd % 256, yEnd / 256, yStart % 256, yStart / 256);

    WriteDispRam(xDot / 8, yDot, (uint8_t *)buffer, 0);
    //WriteDispRam(xDot / 8, yDot, (uint8_t *)buffer, 0);
    uint32_t updateTime = millis();
    FullUpdate();
    if (WaitBUSY(20 * 1000ul) == true)
    {
        EPD_LOGI("Dispaly full finish,takes %ldms", millis() - updateTime);
        return true;
    }
    else
    {
        EPD_LOGI("Dispaly full is timeout,takes %ldms", millis() - updateTime);
        return false;
    }
    SetRamPointer(xStart / 8, yEnd % 256, yEnd / 256);
    WriteDispRam(xDot / 8, yDot, (uint8_t *)buffer, 0);
}

bool HINKE029A01::DisplayPart(uint16_t xStart, uint16_t xEnd, uint16_t yStart, uint16_t yEnd,uint8_t *buffer)
{
    uint16_t Xsize = xEnd - xStart;
    uint16_t Ysize = yEnd - yStart + 1;
    if (Xsize % 8 != 0)
    {
        Xsize = Xsize + (8 - Xsize % 8);
    }
    Xsize = Xsize / 8;
    uint16_t offset = yStart * xDot / 8 + xStart / 8;

    uint32_t temp = yStart;
    yStart = yDot - 1 - yEnd;
    yEnd = yDot - 1 - temp;

    SetRamArea(xStart, xEnd, yEnd % 256, yEnd / 256, yStart % 256, yStart / 256);
    SetRamPointer(xStart / 8, yEnd % 256, yEnd / 256);

    WriteDispRam(Xsize, Ysize, (uint8_t *)buffer, offset);

    uint32_t updateTime = millis();
    PartUpdate();
    if (WaitBUSY(20 * 1000ul) == true)
    {
        EPD_LOGI("Dispaly full finish,takes %ldms", millis() - updateTime);
        return true;
    }
    else
    {
        EPD_LOGI("Dispaly full is timeout,takes %ldms", millis() - updateTime);
        return false;
    }
    SetRamPointer(xStart / 8, yEnd % 256, yEnd / 256);
    WriteDispRam(Xsize, Ysize, (uint8_t *)buffer, offset);
}