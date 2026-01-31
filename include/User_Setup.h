/**
 * TFT_eSPI 用户配置文件
 * 这个文件作为备用配置，主要配置已在 platformio.ini 中定义
 * 如果需要更详细的配置，可以修改此文件
 */

// 驱动配置
#define ST7789_DRIVER       // ST7789 驱动

// 屏幕尺寸
#define TFT_WIDTH  240
#define TFT_HEIGHT 320      // 常见的 240x280 或 240x320

// 引脚定义
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS   15
#define TFT_DC   2
#define TFT_RST  4
#define TFT_BL   21         // 背光引脚

// 字体
#define LOAD_GLCD   
#define LOAD_FONT2  
#define LOAD_FONT4  
#define LOAD_FONT6  
#define LOAD_FONT7  
#define LOAD_FONT8  
#define LOAD_GFXFF  
#define SMOOTH_FONT

// SPI 频率
#define SPI_FREQUENCY       40000000    // 40MHz
#define SPI_READ_FREQUENCY  20000000    // 20MHz

// 颜色反转（如果屏幕颜色不正常，取消注释）
// #define TFT_INVERSION_ON
// #define TFT_INVERSION_OFF
