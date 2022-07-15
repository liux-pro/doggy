#include "sound.h"
#define TEST_I2S_SIZE 8
#define TEST_I2S_TIMEOUT 1000

unsigned char writeBuf[] = "I2sDemoTest";
unsigned char readBuf[TEST_I2S_SIZE] = "";
void sound_init(void)
{
    unsigned int ret;

    IoSetFunc(WIFI_IOT_IO_NAME_GPIO_8, WIFI_IOT_IO_FUNC_GPIO_8_I2S0_WS);
    IoSetFunc(WIFI_IOT_IO_NAME_GPIO_5, WIFI_IOT_IO_FUNC_GPIO_5_I2S0_MCLK);
    IoSetFunc(WIFI_IOT_IO_NAME_GPIO_6, WIFI_IOT_IO_FUNC_GPIO_6_I2S0_TX);

    GpioSetDir(WIFI_IOT_IO_NAME_GPIO_11, WIFI_IOT_GPIO_DIR_IN);
    IoSetFunc(WIFI_IOT_IO_NAME_GPIO_11, WIFI_IOT_IO_FUNC_GPIO_11_I2S0_RX);

    IoSetFunc(WIFI_IOT_IO_NAME_GPIO_12, WIFI_IOT_IO_FUNC_GPIO_12_I2S0_BCLK);

    WifiIotI2sAttribute i2s_cfg;
    i2s_cfg.sampleRate = WIFI_IOT_I2S_SAMPLE_RATE_8K;
    i2s_cfg.resolution = WIFI_IOT_I2S_RESOLUTION_24BIT;

    ret = I2sDeinit();
    printf("i2s deinit:%d\n", ret);
    ret = I2sInit(&i2s_cfg);
    printf("i2s init:%d\n", ret);

    // ret = I2sWrite(writeBuf, sizeof(writeBuf), TEST_I2S_TIMEOUT);
    // printf("i2sssss%d", ret);

    // ret = I2sRead(readBuf, sizeof(readBuf), TEST_I2S_TIMEOUT);
    // printf("i2sssss%d", ret);
}