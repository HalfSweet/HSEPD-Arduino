#include "HINKE029A01.h"

HINKE029A01::HINKE029A01(bool SPIMode, uint8_t CS, uint8_t RST, uint8_t DC, uint8_t BUSY, uint8_t SCK, uint8_t SDA) : HSEPD_BASIC(SPIMode, CS, RST, DC, BUSY, SCK, SDA)
{
}

HINKE029A01::~HINKE029A01()
{
}

void HINKE029A01::BasicInit()
{
    _EPD_RST_LOW;
    DriveDelay(10);
    _EPD_RST_HIGH;
    DriveDelay(10);

    WriteCMD(0x12);
    WaitBUSY();

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
    // Set RAM Y address counter
    RamPointerY[0] = 0x4F;
    RamPointerY[1] = addrY;
    RamPointerY[2] = addrY1;

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
    RamAreaX[2] = Xend / 8 - 1;
    RamAreaY[0] = 0x45; // command
    RamAreaY[1] = Ystart;
    RamAreaY[2] = Ystart1;
    RamAreaY[3] = Yend;
    RamAreaY[4] = Yend1;
    WriteCMDDATA(RamAreaX, sizeof(RamAreaX));
    WriteCMDDATA(RamAreaY, sizeof(RamAreaY));
    EPD_LOGD("Set ram area finish");
}

void HINKE029A01::WriteDispRam(uint16_t XSize, uint16_t YSize, uint8_t *buffer, uint16_t offset, uint8_t address)
{
    WaitBUSY();
    WriteCMD(address);
    uint8_t *buf = buffer;
    buf += offset;
    _EPD_CS_LOW;
    _EPD_DC_HIGH;
    for (int i = 0; i < YSize; i++)
    {
        for (int j = 0; j < XSize; j++)
        {
            SPIWrite(*buf);
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
    uint16_t xEnd = xDot;
    uint32_t temp = yStart;

    yStart = yDot - 1 - yEnd;
    yEnd = yDot - 1 - temp;

    SetRamPointer(xStart / 8, yEnd % 256, yEnd / 256);
    SetRamArea(xStart, xEnd, yEnd % 256, yEnd / 256, yStart % 256, yStart / 256);

    WriteDispRam(xDot / 8, yDot, buffer, 0);
    // WriteDispRam(xDot / 8, yDot, (uint8_t *)buffer, 0);
    uint32_t updateTime = millis();
    Update();
    if (WaitBUSY(20 * 1000ul) == true)
    {
        SetRamPointer(xStart / 8, yEnd % 256, yEnd / 256);
        WriteDispRam(xDot / 8, yDot, buffer, 0);
        EPD_LOGD("Dispaly full finish,takes %ldms", millis() - updateTime);
        return true;
    }
    else
    {
        EPD_LOGE("Dispaly full is timeout,takes %ldms", millis() - updateTime);
        return false;
    }
}

bool HINKE029A01::DisplayPart(uint16_t xStart, uint16_t xEnd, uint16_t yStart, uint16_t yEnd, uint8_t *buffer)
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

    EPD_LOGV("Xsize is %d", Xsize);
    WriteDispRam(Xsize, Ysize, buffer, offset);

    uint32_t updateTime = millis();
    Update();
    if (WaitBUSY(20 * 1000ul) == true)
    {
        EPD_LOGD("Dispaly part finish,takes %ldms", millis() - updateTime);
        return true;
    }
    else
    {
        EPD_LOGE("Dispaly part is timeout,takes %ldms", millis() - updateTime);
        return false;
    }
    SetRamPointer(xStart / 8, yEnd % 256, yEnd / 256);
    WriteDispRam(Xsize, Ysize, buffer, offset);
}

bool HINKE029A01::Display4GrayFull(uint8_t *buffer)
{
    uint16_t yStart = 0;
    uint16_t yEnd = yDot - 1;
    uint16_t xStart = 0;
    uint16_t xEnd = xDot;
    uint32_t temp = yStart;

    yStart = yDot - 1 - yEnd;
    yEnd = yDot - 1 - temp;

    uint8_t *buf = new (std::nothrow) uint8_t[xDot / 8 * yDot];
    if (buf == nullptr)
    {
        EPD_LOGE("You don't have enough memory.");
        return false;
    }

    SetRamPointer(xStart / 8, yEnd % 256, yEnd / 256);
    SetRamArea(xStart, xEnd, yEnd % 256, yEnd / 256, yStart % 256, yStart / 256);
    GrayArrConvert(xDot / 8 * yDot, buffer, buf, 0);
    for (uint32_t i = 0; i < xDot / 8 * yDot; i++)
    {
        buf[i] = ~buf[i];
    }
    WriteDispRam(xDot / 8, yDot, buf, 0, 0x24);
    GrayArrConvert(xDot / 8 * yDot, buffer, buf, 1);
    for (uint32_t i = 0; i < xDot / 8 * yDot; i++)
    {
        buf[i] = ~buf[i];
    }
    WriteDispRam(xDot / 8, yDot, buf, 0, 0x26);
    delete[] buf;

    uint32_t updateTime = millis();
    Update();
    if (WaitBUSY(20 * 1000ul) == true)
    {
        EPD_LOGD("Dispaly 4 gray finish,takes %ldms", millis() - updateTime);
        return true;
    }
    else
    {
        EPD_LOGE("Dispaly 4 gray is timeout,takes %ldms", millis() - updateTime);
        return false;
    }
}

bool HINKE029A01::Display16GrayFull(uint8_t *buffer)
{
    uint16_t yStart = 0;
    uint16_t yEnd = yDot - 1;
    uint16_t xStart = 0;
    uint16_t xEnd = xDot;
    uint32_t temp = yStart;

    yStart = yDot - 1 - yEnd;
    yEnd = yDot - 1 - temp;

    uint8_t *buf = new (std::nothrow) uint8_t[xDot / 8 * yDot];
    if (buf == nullptr)
    {
        EPD_LOGE("You don't have enough memory.");
        return false;
    }

    /* 写入第一个LUT，刷第一次 */
    WriteCMDDATA((uint8_t *)HINKE029A01_LUT::LUT_16Gray_0, sizeof(HINKE029A01_LUT::LUT_16Gray_0));
    SetRamPointer(xStart / 8, yEnd % 256, yEnd / 256);
    SetRamArea(xStart, xEnd, yEnd % 256, yEnd / 256, yStart % 256, yStart / 256);
    GrayArrConvert(xDot / 8 * yDot, buffer, buf, 0, 4);
    for (uint32_t i = 0; i < xDot / 8 * yDot; i++)
    {
        buf[i] = ~buf[i];
    }
    WriteDispRam(xDot / 8, yDot, buf, 0, 0x24);
    GrayArrConvert(xDot / 8 * yDot, buffer, buf, 1, 4);
    for (uint32_t i = 0; i < xDot / 8 * yDot; i++)
    {
        buf[i] = ~buf[i];
    }
    WriteDispRam(xDot / 8, yDot, buf, 0, 0x26);
    uint32_t updateTime = millis();
    Update(); //第一次刷新
    if (WaitBUSY(20 * 1000ul) == false)
    {
        EPD_LOGE("Dispaly 4 gray is timeout,takes %ldms", millis() - updateTime);
        return false;
    }

    WriteCMDDATA((uint8_t *)HINKE029A01_LUT::LUT_16Gray_1, sizeof(HINKE029A01_LUT::LUT_16Gray_1));
    SetRamPointer(xStart / 8, yEnd % 256, yEnd / 256);
    SetRamArea(xStart, xEnd, yEnd % 256, yEnd / 256, yStart % 256, yStart / 256);
    GrayArrConvert(xDot / 8 * yDot, buffer, buf, 2, 4);
    for (uint32_t i = 0; i < xDot / 8 * yDot; i++)
    {
        buf[i] = ~buf[i];
    }
    WriteDispRam(xDot / 8, yDot, buf, 0, 0x24);
    GrayArrConvert(xDot / 8 * yDot, buffer, buf, 3, 4);
    for (uint32_t i = 0; i < xDot / 8 * yDot; i++)
    {
        buf[i] = ~buf[i];
    }
    WriteDispRam(xDot / 8, yDot, buf, 0, 0x26);
    delete[] buf;

    Update(); //第二次刷新
    if (WaitBUSY(20 * 1000ul) == true)
    {
        EPD_LOGD("Dispaly 16 gray finish,takes %ldms", millis() - updateTime);
        return true;
    }
    else
    {
        EPD_LOGE("Dispaly 4 gray is timeout,takes %ldms", millis() - updateTime);
        return false;
    }
    return true;
}

void HINKE029A01::Init(DisMode disMode)
{
    _disMode = disMode;
    BasicInit();
    switch (disMode)
    {
    case DisMode::Full:
        WriteCMDDATA((uint8_t *)HINKE029A01_LUT::LUT_Full, sizeof(HINKE029A01_LUT::LUT_Full));
        EPD_LOGD("Full init finish.");
        break;

    case DisMode::Part:
        WriteCMDDATA((uint8_t *)HINKE029A01_LUT::LUT_Part, sizeof(HINKE029A01_LUT::LUT_Part));
        EPD_LOGD("Part init finish.");
        break;

    case DisMode::Gray4AloneFull:
        WriteCMDDATA((uint8_t *)HINKE029A01_LUT::LUT_4Gray_Alone, sizeof(HINKE029A01_LUT::LUT_4Gray_Alone));
        EPD_LOGD("4gray alone init finish.");
        break;

    case DisMode::Gray4OverlapFull:
        WriteCMDDATA((uint8_t *)HINKE029A01_LUT::LUT_4Gray_Overlap, sizeof(HINKE029A01_LUT::LUT_4Gray_Overlap));
        EPD_LOGD("4gray overlap init finish.");
        break;

    case DisMode::Gray16Full:
        /* 16灰等刷屏的时候再写波形 */
        return;
        break;

    default:
        break;
    }

    WriteCMD(0x22);
    WriteDATA(0xC0);
    WriteCMD(0x20);
}

bool HINKE029A01::Display(uint8_t *buffer, uint16_t xStart, uint16_t xEnd, uint16_t yStart, uint16_t yEnd)
{
    switch (_disMode)
    {
    case DisMode::Full:
        return DisplayFull(buffer);
        break;

    case DisMode::Part:
        return DisplayPart(xStart, xEnd, yStart, yEnd, buffer);
        break;

    case DisMode::Gray4AloneFull:
    case DisMode::Gray4OverlapFull:
        return Display4GrayFull(buffer);
        break;

    case DisMode::Gray16Full:
        return Display16GrayFull(buffer);

    default:
        return false;
        break;
    }
}

void HINKE029A01::Update()
{
    WriteCMD(0x22);
    switch (_disMode)
    {
    case DisMode::Full:
    case DisMode::Gray4AloneFull:
    case DisMode::Gray4OverlapFull:
    case DisMode::Gray16Full:
        WriteDATA(0xC7);
        break;

    case DisMode::Part:
        WriteDATA(0x04);
        break;
    }
    WriteCMD(0x20);
}