#include "st7735_adapter.h"

// 接线  换端口的话要改端口复用设置 即下边IoSetFunc函数的调用
// lcd RST -- hi3861 gpio 7
// lcd DC  -- hi3861 gpio 2
// lcd SDA -- hi3861 gpio9 spi0 mosi
// lcd CLK -- hi3861 gpio10 spi0 clk
#define PIN_RESET WIFI_IOT_IO_NAME_GPIO_7
#define PIN_COMMUNICATION_MODE WIFI_IOT_IO_NAME_GPIO_2

#define LCD_SPI_INDEX WIFI_IOT_SPI_ID_0
#define LCD_SPI_SPEED 40*1000000
//这三个方法被st7735的通用驱动（https://github.com/michal037/driver-ST7735S）以extern的方式引用，事实上，驱动只需要这三个函数
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

  SpiHostWrite(LCD_SPI_INDEX, (char *)buffer, length);
}
/* ################## END - HAVE TO BE IMPLEMENTED - END #################### */

// just in case
void fail_handler(void)
{
  printf("error\n");
}

//初始化spi，复位lcd，初始化lcd。
void lcd_simple_init(void)
{
  //初始化GPIO,貌似不初始化也没事
  // GpioInit();

  IoSetFunc(WIFI_IOT_IO_NAME_GPIO_10, WIFI_IOT_IO_FUNC_GPIO_10_SPI0_CK);
  IoSetFunc(WIFI_IOT_IO_NAME_GPIO_9, WIFI_IOT_IO_FUNC_GPIO_9_SPI0_TXD);

  //spi 的使用参考https://blog.csdn.net/includereg51c/article/details/124358815
  int ret;
  WifiIotSpiCfgBasicInfo param = {0};
  WifiIotSpiCfgInitParam initParam = {0};
  initParam.isSlave = 0; //主设备
  initParam.pad = 0;
  param.cpha = WIFI_IOT_SPI_CFG_CLOCK_CPOL_1;           // CPOL=0，CPHA=0 时钟信号idle状态为低电平，第一个时钟边沿采样数据,CPOL=0，CPHA=1 时钟信号idle状态为低电平，第二个时钟边沿采样数据。
  param.cpol = WIFI_IOT_SPI_CFG_CLOCK_CPHA_0;           // CPOL=1，CPHA=0 时钟信号idle状态为高电平，第一个时钟边沿采样数据,CPOL=1，CPHA=1 时钟信号idle状态为高电平，第二个时钟边沿采样数据。
  param.dataWidth = WIFI_IOT_SPI_CFG_DATA_WIDTH_E_8BIT; //传送数据的大小
  param.endian = WIFI_IOT_SPI_CFG_ENDIAN_LITTLE;        // 数据传输是大端模式还是小端模式，这里选择小端模式
  param.framMode = WIFI_IOT_SPI_CFG_FRAM_MODE_MOTOROLA; //通信协议
  param.freq = LCD_SPI_SPEED;
  param.pad = 0;
  ret = SpiInit(LCD_SPI_INDEX, initParam, &param); // SPI初始化

  printf("!!!lcd spi init status:%d!!!\n", ret);

  //设置GPIO_2的复用功能为普通GPIO
  IoSetFunc(WIFI_IOT_IO_NAME_GPIO_7, WIFI_IOT_IO_FUNC_GPIO_7_GPIO);
  GpioSetDir(WIFI_IOT_GPIO_IDX_7, WIFI_IOT_GPIO_DIR_OUT);

  IoSetFunc(WIFI_IOT_IO_NAME_GPIO_2, WIFI_IOT_IO_FUNC_GPIO_2_GPIO);
  GpioSetDir(WIFI_IOT_GPIO_IDX_2, WIFI_IOT_GPIO_DIR_OUT);

  //////////////////////////////////////////////////////
  lcd_ptr_t my_lcd_settings = NULL;

  //根据屏幕实际分辨率修改
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
  printf("!!!lcd should  already init!!!\n");
  /////////////////////////////////////////////////////
}