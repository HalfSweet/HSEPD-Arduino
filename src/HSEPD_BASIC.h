#pragma once

#include <Arduino.h>
#ifndef NOT_USE_HARD_SPI
#include <SPI.h>
#endif // !NOT_USE_HARD_SPI

#define DEBUG_ESP_PORT Serial //调试用，记得删
#define DEBUG_ESP_CORE

#if defined(ESP32)
#define CONFIG_ARDUHAL_ESP_LOG //使用ESP IDF的日志写法，Arduino的日志输出
#include <esp32-hal-log.h>
static const char *EPDTAG = "HSEPD";
#define EPD_LOGE(...) ESP_LOGE(EPDTAG, ##__VA_ARGS__)
#define EPD_LOGW(...) ESP_LOGW(EPDTAG, ##__VA_ARGS__)
#define EPD_LOGI(...) ESP_LOGI(EPDTAG, ##__VA_ARGS__)
#define EPD_LOGD(...) ESP_LOGD(EPDTAG, ##__VA_ARGS__)
#define EPD_LOGV(...) ESP_LOGV(EPDTAG, ##__VA_ARGS__)

#else
#if defined(ESP8266)
#ifdef DEBUG_ESP_PORT //判断是哪个Serial输出日志
#define DEBUG_MSG(...) DEBUG_ESP_PORT.printf(__VA_ARGS__)
#else
#define DEBUG_MSG(...)
#endif
#ifdef DEBUG_ESP_CORE //判断debug level是不是为CORE
#define EPD_LOGE(...)                                                                    \
    do                                                                                   \
    {                                                                                    \
        DEBUG_MSG("[%6lu][E][%s:%u] %s():", millis(), __FILE__, __LINE__, __FUNCTION__); \
        DEBUG_MSG(__VA_ARGS__);                                                          \
        DEBUG_MSG("\n");                                                                 \
    } while (0)
#define EPD_LOGW(...)                                                                    \
    do                                                                                   \
    {                                                                                    \
        DEBUG_MSG("[%6lu][W][%s:%u] %s():", millis(), __FILE__, __LINE__, __FUNCTION__); \
        DEBUG_MSG(__VA_ARGS__);                                                          \
        DEBUG_MSG("\n");                                                                 \
    } while (0)
#define EPD_LOGI(...)                                                                    \
    do                                                                                   \
    {                                                                                    \
        DEBUG_MSG("[%6lu][I][%s:%u] %s():", millis(), __FILE__, __LINE__, __FUNCTION__); \
        DEBUG_MSG(__VA_ARGS__);                                                          \
        DEBUG_MSG("\n");                                                                 \
    } while (0)
#define EPD_LOGD(...)                                                                    \
    do                                                                                   \
    {                                                                                    \
        DEBUG_MSG("[%6lu][D][%s:%u] %s():", millis(), __FILE__, __LINE__, __FUNCTION__); \
        DEBUG_MSG(__VA_ARGS__);                                                          \
        DEBUG_MSG("\n");                                                                 \
    } while (0)
#define EPD_LOGV(...)                                                                    \
    do                                                                                   \
    {                                                                                    \
        DEBUG_MSG("[%6lu][V][%s:%u] %s():", millis(), __FILE__, __LINE__, __FUNCTION__); \
        DEBUG_MSG(__VA_ARGS__);                                                          \
        DEBUG_MSG("\n");                                                                 \
    } while (0)
#else //如果不是，就替换为空
#define EPD_LOGE(...)
#define EPD_LOGW(...)
#define EPD_LOGI(...)
#define EPD_LOGD(...)
#define EPD_LOGV(...)
#endif
#endif
#endif

#define _EPD_CS_LOW digitalWrite(_CS, LOW)
#define _EPD_CS_HIGH digitalWrite(_CS, HIGH)

#define _EPD_RST_LOW digitalWrite(_RST, LOW)
#define _EPD_RST_HIGH digitalWrite(_RST, HIGH)

#define _EPD_DC_LOW digitalWrite(_DC, LOW)
#define _EPD_DC_HIGH digitalWrite(_DC, HIGH)

#define _READ_EPD_BUSY digitalRead(_BUSY)

#define _EPD_SCK_LOW digitalWrite(_SCK, LOW)
#define _EPD_SCK_HIGH digitalWrite(_SCK, HIGH)

#define _EPD_SDA_LOW digitalWrite(_SDA, LOW)
#define _EPD_SDA_HIGH digitalWrite(_SDA, HIGH)
#define _READ_EPD_SDA digitalRead(_SDA)

class HSEPD_BASIC
{
protected:
    uint8_t _CS;
    uint8_t _RST;
    uint8_t _DC;
    uint8_t _BUSY;
    uint8_t _SCK;
    uint8_t _SDA;
    bool _SPIMode;

#ifndef NOT_USE_HARD_SPI
    SPIClass *EPDSPI;
#endif // !NOT_USE_HARD_SPI

    void DriveDelay(uint32_t ms);

    void SetMOSI();
    void SetMISO();
    void SPIWrite(uint8_t value);
    uint8_t SPIRead();
    void WriteCMD(uint8_t cmd);
    void WriteDATA(uint8_t data);
    void WriteCMDDATA(uint8_t *value, uint8_t Datalen);

    virtual bool WaitBUSY(uint32_t timeOut) = 0;
    virtual void SetRamPointer(uint16_t addrX, uint8_t addrY, uint8_t addrY1) = 0;
    virtual void SetRamArea(uint16_t Xstart, uint16_t Xend, uint8_t Ystart, uint8_t Ystart1, uint8_t Yend, uint8_t Yend1) = 0;
    virtual void WriteDispRam(uint16_t XSize, uint16_t YSize, uint8_t *Dispbuff, uint16_t offset) = 0;

public:
    HSEPD_BASIC(bool SPIMode, uint8_t CS, uint8_t RST, uint8_t DC, uint8_t BUSY, uint8_t SCK, uint8_t SDA);
    ~HSEPD_BASIC();
    void SetHardSPI(SPIClass *spi);
    /********带virtual的均为在每个屏幕的类中实现的函数*********/
    virtual void InitFull() = 0;
    virtual void InitPart() = 0;
    virtual void DeepSleep() = 0;
    virtual bool DisplayFull(uint8_t *buffer) = 0;
    virtual void FullUpdate() = 0;
};

enum class EPDType
{
    HINKE029A01 = 0,
};
