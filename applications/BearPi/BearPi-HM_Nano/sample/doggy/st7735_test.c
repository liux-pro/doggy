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
#include "st7735s.h"

#define PIN_RESET WIFI_IOT_IO_NAME_GPIO_7
#define PIN_COMMUNICATION_MODE WIFI_IOT_IO_NAME_GPIO_2
/* ######################## HAVE TO BE IMPLEMENTED ########################## */
/* These three functions HAVE TO BE LINKED to the code from the st7735s.c file.
 * They can be implemented in another file than this one.
 */

void lcd_delay(unsigned long int milliseconds)
{
  usleep(milliseconds * 1000);
}

void lcd_digitalWrite(unsigned short int pin, unsigned char value)
{
  GpioSetOutputVal(pin, value);
}

void lcd_spiWrite(unsigned char *buffer, size_t length)
{
  /* ALREADY TESTED INSIDE THE LIBRARY:
   * 1. if(buffer == NULL)
   * 2. if(length <= 0)
   * You do not need to test this.
   */
  // Serial.printf("%x\n",buffer[i]);

  SpiHostWrite(WIFI_IOT_SPI_ID_0, (char *)buffer, length);

  /* code */

  // wiringPiSPIDataRW(WPI_SPI_CHANNEL, buffer, length);
}
/* ################## END - HAVE TO BE IMPLEMENTED - END #################### */

void fail_handler(void)
{
  printf("error\n");
}
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
static void LedTask(void)
{
  //初始化GPIO
  GpioInit();

  IoSetFunc(WIFI_IOT_IO_NAME_GPIO_10, WIFI_IOT_IO_FUNC_GPIO_10_SPI0_CK);
  IoSetFunc(WIFI_IOT_IO_NAME_GPIO_9, WIFI_IOT_IO_FUNC_GPIO_9_SPI0_TXD);

  int ret;
  WifiIotSpiCfgBasicInfo param = {0};
  WifiIotSpiCfgInitParam initParam = {0};
  initParam.isSlave = 0;                                //主设备
  initParam.pad = 0;                                    //不理解
  param.cpha = WIFI_IOT_SPI_CFG_CLOCK_CPOL_1;           // CPOL=0，CPHA=0 时钟信号idle状态为低电平，第一个时钟边沿采样数据,CPOL=0，CPHA=1 时钟信号idle状态为低电平，第二个时钟边沿采样数据。
  param.cpol = WIFI_IOT_SPI_CFG_CLOCK_CPHA_0;           // CPOL=1，CPHA=0 时钟信号idle状态为高电平，第一个时钟边沿采样数据,CPOL=1，CPHA=1 时钟信号idle状态为高电平，第二个时钟边沿采样数据。
  param.dataWidth = WIFI_IOT_SPI_CFG_DATA_WIDTH_E_8BIT; //传送数据的大小
  param.endian = WIFI_IOT_SPI_CFG_ENDIAN_LITTLE;        // 数据传输是大端模式还是小端模式，这里选择小端模式
  param.framMode = WIFI_IOT_SPI_CFG_FRAM_MODE_MOTOROLA; //通信协议
  param.freq = 40 * 1000000;
  param.pad = 0;                                       //不理解
  ret = SpiInit(WIFI_IOT_SPI_ID_0, initParam, &param); // SPI初始化

  printf("!!!%d!!!", ret);

  //设置GPIO_2的复用功能为普通GPIO
  IoSetFunc(WIFI_IOT_IO_NAME_GPIO_7, WIFI_IOT_IO_FUNC_GPIO_7_GPIO);
  GpioSetDir(WIFI_IOT_GPIO_IDX_7, WIFI_IOT_GPIO_DIR_OUT);

  IoSetFunc(WIFI_IOT_IO_NAME_GPIO_2, WIFI_IOT_IO_FUNC_GPIO_2_GPIO);
  GpioSetDir(WIFI_IOT_GPIO_IDX_2, WIFI_IOT_GPIO_DIR_OUT);

  //////////////////////////////////////////////////////
  lcd_ptr_t my_lcd_settings = NULL;

  my_lcd_settings = lcd_createSettings(
      128,                    /* width */
      160,                    /* height */
      0,                      /* width_offset */
      0,                      /* height_offset */
      PIN_COMMUNICATION_MODE, /* Wiring Pi pin numbering */
      PIN_RESET);

  /* lcd_createSettings will return NULL if memory runs out */
  if (my_lcd_settings == NULL)
  {
    fail_handler();
  }

  /* This is where you set the settings as active for the library.
   * The library will use them to handle the driver.
   * You can swap it at any time for a different display module.
   * If set to NULL, library functions will return LCD_FAIL.
   */
  lcd_setSettingsActive(my_lcd_settings);

  /* Display initialization. It HAVE TO be done for each display separately.
   * The library will do this for the appropriate display module,
   * based on the active settings.
   */
  if (lcd_initialize() < LCD_OK)
  {
    fail_handler();
  }

  /* To start drawing, you HAVE TO:
   * step 1: turn off sleep mode
   * step 2: set Memory Access Control  - required by lcd_createSettings()
   * step 3: set Interface Pixel Format - required by lcd_createSettings()
   * step 4: turn on the display
   * After that, you can draw.
   *
   * It is best to make the optional settings between steps 1 and 4.
   */

  /* turn off sleep mode; required to draw */
  if (lcd_setSleepMode(LCD_SLEEP_OUT) < LCD_OK)
  {
    fail_handler();
  }

  /* set Memory Access Control; refresh - required by lcd_createSettings() */
  if (lcd_setMemoryAccessControl(LCD_MADCTL_DEFAULT) < LCD_OK)
  {
    fail_handler();
  }

  /* set Interface Pixel Format; refresh - required by lcd_createSettings() */
  if (lcd_setInterfacePixelFormat(LCD_PIXEL_FORMAT_565) < LCD_OK)
  {
    fail_handler();
  }

  /* set Predefined Gamma; optional reset */
  if (lcd_setGammaPredefined(LCD_GAMMA_PREDEFINED_3) < LCD_OK)
  {
    fail_handler();
  }

  /* set Display Inversion; optional reset */
  if (lcd_setDisplayInversion(LCD_INVERSION_OFF) < LCD_OK)
  {
    fail_handler();
  }

  /* set Tearing Effect Line; optional reset */
  if (lcd_setTearingEffectLine(LCD_TEARING_OFF) < LCD_OK)
  {
    fail_handler();
  }

  /* turn on the display; required to draw */
  if (lcd_setDisplayMode(LCD_DISPLAY_ON) < LCD_OK)
  {
    fail_handler();
  }

  /* ready to draw */

  // /* demo; HIGH MEMORY AND CPU CONSUMPTION */
  if (demo() < LCD_OK)
  {
    fail_handler();
  }
  /* deselect display module; deactivation of communication */
  /////////////////////////////////////////////////////
  while (1)
  {
    // //设置GPIO_2输出高电平点亮LED灯
    // GpioSetOutputVal(WIFI_IOT_GPIO_IDX_2, 1);

    // //延时1s
    // usleep(1000000);

    // //设置GPIO_2输出低电平熄灭LED灯
    // GpioSetOutputVal(WIFI_IOT_GPIO_IDX_2, 0);

    // //延时1s
    // usleep(1000000);
  }
}

static void LedExampleEntry(void)
{
  osThreadAttr_t attr;

  attr.name = "LedTask";
  attr.attr_bits = 0U;
  attr.cb_mem = NULL;
  attr.cb_size = 0U;
  attr.stack_mem = NULL;
  attr.stack_size = 100 * 1024;
  attr.priority = 25;

  if (osThreadNew((osThreadFunc_t)LedTask, NULL, &attr) == NULL)
  {
    printf("Falied to create LedTask!\n");
  }
}

APP_FEATURE_INIT(LedExampleEntry);