#include "DEPG0750RWU790F30.h"

DEPG0750RWU790F30::DEPG0750RWU790F30(bool SPIMode, uint8_t CS, uint8_t RST, uint8_t DC, uint8_t BUSY, uint8_t SCK, uint8_t SDA) : HSEPD_BASIC(SPIMode, CS, RST, DC, BUSY, SCK, SDA)
{
}

DEPG0750RWU790F30::~DEPG0750RWU790F30()
{
}

void DEPG0750RWU790F30::BasicInit()
{
    _EPD_RST_LOW;
    DriveDelay(10);
    _EPD_RST_HIGH;
    DriveDelay(10);

    WriteCMD(0x01); // POWER SETTING
    WriteDATA(0x07);
    WriteDATA(0x07); // VGH=20V,VGL=-20V
    WriteDATA(0x3f); // VDH=15V
    WriteDATA(0x3f); // VDL=-15V

    WriteCMD(0x04); // Power on
    WaitBUSY();     // waiting for the electronic paper IC to release the idle signal

    WriteCMD(0X00); // PANNEL SETTING
    // WriteDATA(0x1F);
    WriteDATA(0x77); // KW-3f   KWR-2F BWROTP 0f BWOTP 1f

    WriteCMD(0x61);  // tres
    WriteDATA(0x03); // source 800
    WriteDATA(0x20);
    WriteDATA(0x01); // gate 480
    WriteDATA(0xE0);

    WriteCMD(0X15);
    WriteDATA(0x00);

    WriteCMD(0X50); // VCOM AND DATA INTERVAL SETTING
    WriteDATA(0x10);
    WriteDATA(0x07);

    WriteCMD(0X60); // TCON SETTING
    WriteDATA(0x22);
}

bool DEPG0750RWU790F30::WaitBUSY(uint32_t timeOut)
{
    uint32_t currentTime = millis();
    while (1)
    {
        if (_READ_EPD_BUSY == HIGH)
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

void DEPG0750RWU790F30::SetRamArea(uint16_t Xstart, uint16_t Xend, uint8_t Ystart, uint8_t Ystart1, uint8_t Yend, uint8_t Yend1)
{
    WaitBUSY();
    WriteCMD(0x91); // enter partial refresh mode
    WriteCMD(0x90);
    WriteDATA(Xstart / 256);
    WriteDATA(Xstart % 256);
    WriteDATA(Xend / 256);
    WriteDATA(Xend % 256);
    WriteDATA(Ystart);
    WriteDATA(Ystart1);
    WriteDATA(Yend);
    WriteDATA(Yend1);
    WriteDATA(0x00);
    EPD_LOGD("Set ram area finish");
}

void DEPG0750RWU790F30::WriteDispRam(uint16_t XSize, uint16_t YSize, uint8_t *buffer, uint16_t offset, uint8_t address)
{
    WaitBUSY();
    WriteCMD(address);
    uint8_t *buf = buffer;
    buf += offset;
    _EPD_CS_LOW;
    _EPD_DC_HIGH;
    for (uint16_t i = 0; i < YSize; i++)
    {
        for (uint16_t j = 0; j < XSize; j++)
        {
            SPIWrite(~*buf); //之所以要取反，是因为SSD系列芯片是0xFF为白色，0x00为黑色，保持兼容
            buf++;
        }
        buf += xDot / 8 - XSize;
    }
    WaitBUSY();
    _EPD_CS_HIGH;
}

void DEPG0750RWU790F30::DeepSleep()
{
    WaitBUSY(2000);
    WriteCMD(0x50);
    WriteDATA(0xF7); // border floating
    WriteCMD(0x02);  // power off
    WaitBUSY(2000);
    WriteCMD(0x07); // sleep
    WriteDATA(0xA5);
}

bool DEPG0750RWU790F30::DisplayFull(uint8_t *buffer)
{
    WriteDispRam(xDot / 8, yDot, buffer, 0);
    uint32_t updateTime = millis();
    Update();
    if (WaitBUSY(20 * 1000ul) == true)
    {
        WriteDispRam(xDot / 8, yDot, buffer, 0, 0x10);
        EPD_LOGD("Dispaly full finish,takes %ldms", millis() - updateTime);
        return true;
    }
    else
    {
        EPD_LOGE("Dispaly full is timeout,takes %ldms", millis() - updateTime);
        return false;
    }
}

bool DEPG0750RWU790F30::DisplayPart(uint16_t xStart, uint16_t xEnd, uint16_t yStart, uint16_t yEnd, uint8_t *buffer)
{
    uint16_t Xsize = xEnd - xStart;
    uint16_t Ysize = yEnd - yStart + 1;
    if (Xsize % 8 != 0)
    {
        Xsize = Xsize + (8 - Xsize % 8);
    }
    Xsize = Xsize / 8;
    uint16_t offset = yStart * xDot / 8 + xStart / 8;
    SetRamArea(xStart, xEnd, yStart / 256, yStart % 256, yEnd / 256, yEnd % 256);
    WaitBUSY(2000);
    EPD_LOGV("Xsize = %d,Ysize = %d,offset = %d", Xsize, Ysize, offset);
    WriteDispRam(Xsize, Ysize, buffer, offset);
    uint32_t updateTime = millis();
    Update();
    if (WaitBUSY(20 * 1000ul) == true)
    {
        WriteDispRam(xDot / 8, yDot, buffer, 0, 0x10);
        EPD_LOGD("Dispaly part finish,takes %ldms", millis() - updateTime);
        return true;
    }
    else
    {
        EPD_LOGE("Dispaly part is timeout,takes %ldms", millis() - updateTime);
        return false;
    }
}

bool DEPG0750RWU790F30::Display4GrayFull(uint8_t *buffer)
{
    ;
}

bool DEPG0750RWU790F30::Display16GrayFull(uint8_t *buffer)
{
    ;
}

void DEPG0750RWU790F30::Init(DisMode disMode)
{
    _disMode = disMode;
    BasicInit();
    switch (disMode)
    {
    case DisMode::Full:
        WriteCMDDATA((uint8_t *)DEPG0750RWU790F30_LUT::LUT_Full_Vcom, sizeof(DEPG0750RWU790F30_LUT::LUT_Full_Vcom));
        WriteCMDDATA((uint8_t *)DEPG0750RWU790F30_LUT::LUT_Full_WW, sizeof(DEPG0750RWU790F30_LUT::LUT_Full_WW));
        WriteCMDDATA((uint8_t *)DEPG0750RWU790F30_LUT::LUT_Full_BW, sizeof(DEPG0750RWU790F30_LUT::LUT_Full_BW));
        WriteCMDDATA((uint8_t *)DEPG0750RWU790F30_LUT::LUT_Full_WB, sizeof(DEPG0750RWU790F30_LUT::LUT_Full_WB));
        WriteCMDDATA((uint8_t *)DEPG0750RWU790F30_LUT::LUT_Full_BB, sizeof(DEPG0750RWU790F30_LUT::LUT_Full_BB));
        EPD_LOGD("Full init finish.");
        break;

    case DisMode::Part:
        WriteCMDDATA((uint8_t *)DEPG0750RWU790F30_LUT::LUT_Part_Vcom, sizeof(DEPG0750RWU790F30_LUT::LUT_Part_Vcom));
        WriteCMDDATA((uint8_t *)DEPG0750RWU790F30_LUT::LUT_Part_WW, sizeof(DEPG0750RWU790F30_LUT::LUT_Part_WW));
        WriteCMDDATA((uint8_t *)DEPG0750RWU790F30_LUT::LUT_Part_BW, sizeof(DEPG0750RWU790F30_LUT::LUT_Part_BW));
        WriteCMDDATA((uint8_t *)DEPG0750RWU790F30_LUT::LUT_Part_WB, sizeof(DEPG0750RWU790F30_LUT::LUT_Part_WB));
        WriteCMDDATA((uint8_t *)DEPG0750RWU790F30_LUT::LUT_Part_BB, sizeof(DEPG0750RWU790F30_LUT::LUT_Part_BB));
        EPD_LOGD("Part init finish.");
        break;

    case DisMode::Gray4AloneFull:
        // WriteCMDDATA((uint8_t *)DEPG0750RWU790F30LUT::LUT_4Gray_Alone, sizeof(DEPG0750RWU790F30LUT::LUT_4Gray_Alone));
        EPD_LOGD("4gray alone init finish.");
        break;

    case DisMode::Gray4OverlapFull:
        // WriteCMDDATA((uint8_t *)DEPG0750RWU790F30LUT::LUT_4Gray_Overlap, sizeof(DEPG0750RWU790F30LUT::LUT_4Gray_Overlap));
        EPD_LOGD("4gray overlap init finish.");
        break;

    case DisMode::Gray16Full:
        /* 16灰等刷屏的时候再写波形 */
        return;
        break;

    default:
        break;
    }

    /* WriteCMD(0x22);
    WriteDATA(0xC0);
    WriteCMD(0x20); */
}

bool DEPG0750RWU790F30::Display(uint8_t *buffer, uint16_t xStart, uint16_t xEnd, uint16_t yStart, uint16_t yEnd)
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

void DEPG0750RWU790F30::Update()
{
    // WriteCMD(0x22);
    switch (_disMode)
    {
    case DisMode::Full:
    case DisMode::Gray4AloneFull:
    case DisMode::Gray4OverlapFull:
    case DisMode::Gray16Full:
        WriteCMD(0x12);
        break;

    case DisMode::Part:
        WriteCMD(0x91);
        WriteCMD(0x12);
        break;
    }
    WriteCMD(0x20);
}
