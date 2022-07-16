/*
 * Copyright (c) 2020 Nanjing Xiaoxiongpai Intelligent Technology Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "wifiiot_gpio.h"
#include "wifiiot_gpio_ex.h"
#include "wifiiot_spi.h"
#include "st7735_adapter.h"
#include "wifiiot_i2s.h"
#include "wifiiot_i2c.h"
#include "wifiiot_i2c_ex.h"
#include "mpu6050_adapter.h"
#include "wifiiot_watchdog.h"

int call_c(void);
lcd_status_t demo(void)
{

  unsigned short int width, height;

  /* Prevent a bug of NULL pointer. 'lcd_settings' is required. */
  if (lcd_settings == NULL)
  {
    return LCD_FAIL;
  }

  /* get width and height */
  width = lcd_settings->width;
  height = lcd_settings->height;

  bool flag = 1;

  unsigned char buf[2 * 128 * 160] = {0};

  while (1)
  {
    /* set window position */
    if (lcd_setWindowPosition(0, 0, width - 1, height - 1) < LCD_OK)
    {
      return LCD_FAIL;
    }

    /* activate memory write */
    if (lcd_activateMemoryWrite() < LCD_OK)
    {

      return LCD_FAIL;
    }

    if (flag)
    {
      for (size_t i = 0; i < sizeof(buf); i++)
      {
        buf[i] = 0;
      }

      lcd_framebuffer_send(buf, sizeof(buf), 1024);
    }
    else
    {
      for (size_t i = 0; i < sizeof(buf); i++)
      {
        buf[i] = 0xff;
      }

      lcd_framebuffer_send(buf, sizeof(buf), 1024);
    }
    flag = !flag;
    usleep(1);
  }

  return LCD_OK;
}
static void MainTask(void)
{
  // lcd_simple_init();
  // demo();

  // int a = 0;

  // while (1)
  // {
  // ret = I2sRead(readBuf, sizeof(readBuf), TEST_I2S_TIMEOUT);
  // // printf("%2x%2x%2x%2x\n", readBuf[7], readBuf[6], readBuf[5], readBuf[4]);
  // int x = (readBuf[7] << 24 | readBuf[6] << 16 | readBuf[5] << 8) >> 8;
  // printf("%d,500000,-500000\n", x);
  // WatchDogKick();
  // usleep(0);

  // }

  I2C_Simple_Init();
  MPU6050_Init();
  uint16_t buf[7];

  while (1)
  {
    for (int i = 0; i < 100; i++)
    {
      // if (i == 0)
      // {
      //   MPU6050_EnableLowPowerMode(MPU6050_Wakeup_Freq_1p25Hz);
      // }
      // else if (i == 50)
      // {
      //   MPU6050_DisableLowPowerMode();
      // }
      MPU6050_ReadAll(buf);
      printf("ax:%6d, ay:%6d, az:%6d, tp:%6d, gx:%6d, gy:%6d, gz:%6d\r\n",
             (int16_t)buf[0], (int16_t)buf[1], (int16_t)buf[2], (int16_t)buf[3] / 34 + 365, (int16_t)buf[4], (int16_t)buf[5], (int16_t)buf[6]);
      usleep(100 * 1000);
    }
  }
}

static void MainTaskEntry(void)
{
  osThreadAttr_t attr;

  attr.name = "MainTask";
  attr.attr_bits = 0U;
  attr.cb_mem = NULL;
  attr.cb_size = 0U;
  attr.stack_mem = NULL;
  attr.stack_size = 110 * 1024;
  attr.priority = 25;

  if (osThreadNew((osThreadFunc_t)MainTask, NULL, &attr) == NULL)
  {
    printf("Falied to create MainTask!\n");
  }
}

APP_FEATURE_INIT(MainTaskEntry);