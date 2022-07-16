#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

#include "wifiiot_gpio.h"
#include "wifiiot_gpio_ex.h"
#include "wifiiot_i2c.h"
#include "wifiiot_i2c_ex.h"
#include "mpu6050_adapter.h"


// 接线  
// GPIO 13 I2C0 SDA
// GPIO 14 I2C0 SCL

#define MPU6050_I2C_INDEX WIFI_IOT_I2C_IDX_0

void I2C_Write(uint8_t devAddr, uint8_t memAddr, uint8_t *dat, uint16_t size)
{
    uint8_t send_buffer[20] = {0};

    WifiIotI2cData i2cData = {0};
    send_buffer[0] = memAddr;

    memcpy(send_buffer + 1, dat, size);
    i2cData.sendBuf = send_buffer;
    i2cData.sendLen = size + 1;
    I2cWrite(MPU6050_I2C_INDEX, (devAddr << 1) | 0x00, &i2cData);
}

void I2C_Read(uint8_t devAddr, uint8_t memAddr, uint8_t *buf, uint16_t size)
{
    WifiIotI2cData i2cData = {0};

    uint8_t memAddr_warp[1] = {memAddr};
    i2cData.sendBuf = memAddr_warp;
    i2cData.sendLen = 1;

    i2cData.receiveBuf = buf;
    i2cData.receiveLen = size;

    I2cWriteread(MPU6050_I2C_INDEX, (devAddr << 1) | 0x00, &i2cData);
}

void I2C_Simple_Init(void)
{
    //配置端口复用
    IoSetFunc(WIFI_IOT_IO_NAME_GPIO_13, WIFI_IOT_IO_FUNC_GPIO_13_I2C0_SDA);
    IoSetFunc(WIFI_IOT_IO_NAME_GPIO_14, WIFI_IOT_IO_FUNC_GPIO_14_I2C0_SCL);
    I2cInit(MPU6050_I2C_INDEX, 400000);
    I2cSetBaudrate(MPU6050_I2C_INDEX, 400000);
}