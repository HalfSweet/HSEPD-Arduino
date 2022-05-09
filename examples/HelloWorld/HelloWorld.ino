#include <Arduino.h>
#include <HSEPD.h>
//包含你需要使用的文件系统,例如：
#include <LittleFS.h>

#define BAUD_SPEED 74880 //串口调试的波特率，可自行修改

#define CS 15
#define RST 2
#define DC 0
#define BUSY 4
#define CLK 14
#define DIN 13
HSEPD EPD(0, CS, RST, DC, BUSY, CLK, DIN); //驱动库的实例化，此处为使用软件SPI
const uint8_t city_icon[24] = {
    /* 0X01,0X01,0X0C,0X00,0X0C,0X00, */
    0X00, 0X00, 0X1C, 0X00, 0X77, 0X00, 0X41, 0X80, 0X9C, 0X60, 0XA2, 0X30, 0XA2, 0X30, 0X9C, 0XC0,
    0X41, 0X80, 0X77, 0X00, 0X1C, 0X00, 0X00, 0X00,
    /*  */
};

void setup()
{
    Serial.begin(BAUD_SPEED);
    EPD.SetEpaperType(EPDType::HINKE029A01);
    LittleFS.begin();
    EPD.SetFS(&LittleFS);
    EPD.Init();
    EPD.GUIBegin(296, 128, ORIGIN::BottomLeft);
    EPD.ClearBuffer();
    EPD.FontBegin("/font12", 1, 12, 12);
    EPD.printf(0, 0, "%s", "Hello,World");
    EPD.SetStreamCursor(14, 14, -1);
    EPD << "流输出示例，"
        << "整数：" << 114514 << "，浮点数：" << 191.9810;
    EPD.DrawImageArr(10, 30, 12, 12, (uint8_t *)city_icon);
    EPD.DisplayFull(EPD.DisBuffer);
    EPD.DeepSleep();

    delay(500);
}

void loop()
{
    delay(1);
}