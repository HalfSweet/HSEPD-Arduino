#ifndef HSEPD_LOG_H
#define HSEPD_LOG_H

#if defined(ARDUINO) //Arduino平台下
#include <Arduino.h>
#endif

#ifndef HSEPD_NOT_LOG_COLORS

#define HSEPD_LOG_COLOR_BLACK "30"
#define HSEPD_LOG_COLOR_RED "31"    // ERROR
#define HSEPD_LOG_COLOR_GREEN "32"  // INFO
#define HSEPD_LOG_COLOR_YELLOW "33" // WARNING
#define HSEPD_LOG_COLOR_BLUE "34"
#define HSEPD_LOG_COLOR_MAGENTA "35"
#define HSEPD_LOG_COLOR_CYAN "36" // DEBUG
#define HSEPD_LOG_COLOR_GRAY "37" // VERBOSE
#define HSEPD_LOG_COLOR_WHITE "38"

#define HSEPD_LOG_COLOR(COLOR) "\033[0;" COLOR "m"
#define HSEPD_LOG_BOLD(COLOR) "\033[1;" COLOR "m"
#define HSEPD_LOG_RESET_COLOR "\033[0m"

#define HSEPD_LOG_COLOR_E HSEPD_LOG_COLOR(HSEPD_LOG_COLOR_RED)
#define HSEPD_LOG_COLOR_W HSEPD_LOG_COLOR(HSEPD_LOG_COLOR_YELLOW)
#define HSEPD_LOG_COLOR_I HSEPD_LOG_COLOR(HSEPD_LOG_COLOR_GREEN)
#define HSEPD_LOG_COLOR_D HSEPD_LOG_COLOR(HSEPD_LOG_COLOR_CYAN)
#define HSEPD_LOG_COLOR_V HSEPD_LOG_COLOR(HSEPD_LOG_COLOR_GRAY)

#else

#define HSEPD_LOG_COLOR_E
#define HSEPD_LOG_COLOR_W
#define HSEPD_LOG_COLOR_I
#define HSEPD_LOG_COLOR_D
#define HSEPD_LOG_COLOR_V
#define HSEPD_LOG_RESET_COLOR
#define HSEPD_LOG_COLOR_PRINT(letter)
#define HSEPD_LOG_COLOR_PRINT_END

#endif

#if defined(ARDUINO)

#if defined(ESP32)

#ifndef CONFIG_ARDUHAL_ESP_LOG
#define CONFIG_ARDUHAL_ESP_LOG 1 // 使用ESP IDF的日志写法，Arduino的日志输出
#endif

#ifndef HSEPD_NOT_LOG_COLORS        // 是否开启HSEPD的终端彩色输出
#define CONFIG_ARDUHAL_LOG_COLORS 1 // 如果开启了就打开ESP32的自带彩色输出
#endif

#include <esp32-hal-log.h>

static const char *EPDTAG = "HSEPD";
#define HSEPD_LOGE(...) ESP_LOGE(EPDTAG, ##__VA_ARGS__)
#define HSEPD_LOGW(...) ESP_LOGW(EPDTAG, ##__VA_ARGS__)
#define HSEPD_LOGI(...) ESP_LOGI(EPDTAG, ##__VA_ARGS__)
#define HSEPD_LOGD(...) ESP_LOGD(EPDTAG, ##__VA_ARGS__)
#define HSEPD_LOGV(...) ESP_LOGV(EPDTAG, ##__VA_ARGS__)

#else
#if defined(ESP8266)
#ifdef DEBUG_ESP_PORT // 判断是哪个Serial输出日志
#define DEBUG_MSG(format, ...) DEBUG_ESP_PORT.printf(format, ##__VA_ARGS__)
#else
#define DEBUG_MSG(...)
#endif
#ifdef DEBUG_ESP_CORE // 判断debug level是不是为CORE

#define HSEPD_SHORT_LOG_FORMAT(letter, format) HSEPD_LOG_COLOR_##letter format HSEPD_LOG_RESET_COLOR "\r\n"
#define HSEPD_LOG_FORMAT(letter, format) HSEPD_LOG_COLOR_##letter "[%6lu][" #letter "][%s:%u] %s(): " format HSEPD_LOG_RESET_COLOR "\r\n", (unsigned long int)millis(), __FILE__, __LINE__, __FUNCTION__

#define HSEPD_LOGE(format, ...)                                  \
    do                                                         \
    {                                                          \
        DEBUG_MSG(HSEPD_LOG_FORMAT(E, format), ##__VA_ARGS__); \
    } while (0)

#define HSEPD_LOGW(format, ...)                                  \
    do                                                         \
    {                                                          \
        DEBUG_MSG(HSEPD_LOG_FORMAT(W, format), ##__VA_ARGS__); \
    } while (0)

#define HSEPD_LOGI(format, ...)                                  \
    do                                                         \
    {                                                          \
        DEBUG_MSG(HSEPD_LOG_FORMAT(I, format), ##__VA_ARGS__); \
    } while (0)

#define HSEPD_LOGD(format, ...)                                  \
    do                                                         \
    {                                                          \
        DEBUG_MSG(HSEPD_LOG_FORMAT(D, format), ##__VA_ARGS__); \
    } while (0)

#define HSEPD_LOGV(format, ...)                                  \
    do                                                         \
    {                                                          \
        DEBUG_MSG(HSEPD_LOG_FORMAT(V, format), ##__VA_ARGS__); \
    } while (0)

#else // 如果不是，就替换为空
#define HSEPD_LOGE(...)
#define HSEPD_LOGW(...)
#define HSEPD_LOGI(...)
#define HSEPD_LOGD(...)
#define HSEPD_LOGV(...)
#endif
#endif
#endif
#endif

#endif
