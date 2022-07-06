#ifndef ST7735_ADAPTER_H_
#define ST7735_ADAPTER_H_

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "wifiiot_gpio.h"
#include "wifiiot_gpio_ex.h"
#include "wifiiot_spi.h"
#include "st7735s.h"



//初始化spi，复位lcd，初始化lcd。
void lcd_simple_init(void);











#endif