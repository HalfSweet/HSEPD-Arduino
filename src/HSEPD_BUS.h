#pragma once

#if defined(ARDUINO) // Arduino平台下
#include <Arduino.h>
#endif

class HSEPD_BUS // 在这个类中仅仅实现数据总线的接口
{
private:
protected:
    int32_t _speed; // 通信的速度

public:
    HSEPD_BUS();
    ~HSEPD_BUS();

    /**
     * @brief 写1个字节的数据
     *
     * @param value 需要写入的数据
     */
    virtual void Write(uint8_t value) = 0;

    /**
     * @brief 写1个字节的命令
     *
     * @param command 需要写入的命令
     */
    virtual void WriteCommand(uint8_t command) = 0;

    /**
     * @brief 写1个字节的数据
     *
     * @param data 需要写入的数据
     */
    virtual void WriteData(uint8_t data) = 0;

    /**
     * @brief 写1个字节的命令，以及Datalen - 1个字节的数据
     *
     * @param value 需要写入的数组首地址
     * @param dataLen 需要写入的数组的总长度
     */
    virtual void WriteCMDDATA(uint8_t *value, uint8_t dataLen) = 0;

    /**
     * @brief 读取1个字节的数据
     *
     * @return uint8_t 读取的数据
     */
    virtual uint8_t Read() = 0;

    /**
     * @brief 写入1个字节的命令，并读取1个字节的数据
     *
     * @param command 需要写入的命令
     * @return uint8_t 读取到的数据
     */
    virtual uint8_t ReadData(uint8_t command) = 0;

    /**
     * @brief 写入1个字节的命令，并读取dataLen - 1字节的数据
     *
     * @param command 需要写入的命令
     * @param dataLen  需要读取的数据长度
     * @param data 需要读取的数据数组指针
     */
    virtual void ReadData(uint8_t command, uint8_t dataLen, uint8_t *data) = 0;
};
