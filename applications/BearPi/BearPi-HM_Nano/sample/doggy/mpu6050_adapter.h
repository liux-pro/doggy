#ifndef MPU6050_ADAPTER_H_
#define MPU6050_ADAPTER_H_

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

#include "wifiiot_gpio.h"
#include "wifiiot_gpio_ex.h"
#include "wifiiot_i2c.h"
#include "wifiiot_i2c_ex.h"
#include "mpu6050.h"



void I2C_Write(uint8_t devAddr, uint8_t memAddr, uint8_t *dat, uint16_t size);

void I2C_Read(uint8_t devAddr, uint8_t memAddr, uint8_t *buf, uint16_t size);

void I2C_Simple_Init(void);







#endif