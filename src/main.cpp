/**
 * ESP32 GIF 播放器
 * 硬件: ESP32 + ST7789 TFT 屏幕 + TF卡模块
 * 功能: 从 TF 卡读取 GIF 文件并在屏幕上循环播放
 */

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <TFT_eSPI.h>
#include <AnimatedGIF.h>

// ===== 引脚定义 =====
#define SD_CS    5    // TF卡 CS 引脚
#define SD_SCK   18   // TF卡 SCK 引脚 (与 TFT 共用)
#define SD_MISO  19   // TF卡 MISO 引脚
#define SD_MOSI  23   // TF卡 MOSI 引脚 (与 TFT 共用)

// ===== 全局对象 =====
TFT_eSPI tft = TFT_eSPI();
AnimatedGIF gif;

// ===== 全局变量 =====
File gifFile;
int16_t screenW, screenH;
const char* gifPath = "/anim.gif";  // GIF 文件路径，可修改

// ===== GIF 文件操作回调函数 =====

// 打开文件
bool GIFOpenFile(const char *fname, int32_t *pSize) {
  gifFile = SD.open(fname, FILE_READ);
  if (!gifFile) {
    Serial.printf("无法打开文件: %s\n", fname);
    return false;
  }
  *pSize = gifFile.size();
  Serial.printf("打开文件成功: %s (大小: %d 字节)\n", fname, *pSize);
  return true;
}

// 关闭文件
void GIFCloseFile() {
  if (gifFile) {
    gifFile.close();
  }
}

// 读取文件数据
int32_t GIFReadFile(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen) {
  if (!gifFile) return 0;
  int32_t bytesRead = gifFile.read(pBuf, iLen);
  return bytesRead;
}

// 文件定位
int32_t GIFSeekFile(GIFFILE *pFile, int32_t iPosition) {
  if (!gifFile) return 0;
  gifFile.seek(iPosition);
  return iPosition;
}

// ===== GIF 绘制回调函数 =====
void GIFDraw(GIFDRAW *pDraw) {
  uint8_t *s = pDraw->pPixels;
  uint16_t *pPalette = (uint16_t *)pDraw->pPalette;
  int16_t x = pDraw->iX;
  int16_t y = pDraw->iY + pDraw->y;  // 当前行的 Y 坐标
  int16_t w = pDraw->iWidth;
  
  // 边界检查
  if (y < 0 || y >= screenH) return;
  if (x < 0 || x >= screenW) return;
  
  // 限制宽度
  if (x + w > screenW) w = screenW - x;

  // 使用 TFT_eSPI 的快速写入方法
  tft.startWrite();
  tft.setAddrWindow(x, y, w, 1);
  
  for (int i = 0; i < w; i++) {
    uint8_t idx = *s++;
    uint16_t color;
    
    if (pDraw->ucHasTransparency && idx == pDraw->ucTransparent) {
      // 透明像素处理：可以选择跳过或使用背景色
      color = TFT_BLACK;  // 使用黑色作为透明背景
    } else {
      color = pPalette[idx];
    }
    
    tft.writeColor(color, 1);
  }
  
  tft.endWrite();
}

// ===== 初始化 TFT 屏幕 =====
void initTFT() {
  Serial.println("初始化 TFT 屏幕...");
  tft.init();
  tft.setRotation(1);  // 横屏模式，根据需要调整 (0-3)
  tft.fillScreen(TFT_BLACK);
  
  screenW = tft.width();
  screenH = tft.height();
  
  Serial.printf("屏幕尺寸: %d x %d\n", screenW, screenH);
  
  // 显示欢迎信息
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("ESP32 GIF Player", screenW/2, screenH/2, 4);
  delay(2000);
  tft.fillScreen(TFT_BLACK);
}

// ===== 初始化 SD 卡 =====
bool initSD() {
  Serial.println("初始化 SD 卡...");
  
  // 初始化 SPI 总线
  SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
  
  if (!SD.begin(SD_CS)) {
    Serial.println("SD 卡初始化失败!");
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("SD Card Failed!", screenW/2, screenH/2, 4);
    return false;
  }
  
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("未检测到 SD 卡!");
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("No SD Card!", screenW/2, screenH/2, 4);
    return false;
  }
  
  Serial.print("SD 卡类型: ");
  if (cardType == CARD_MMC) Serial.println("MMC");
  else if (cardType == CARD_SD) Serial.println("SDSC");
  else if (cardType == CARD_SDHC) Serial.println("SDHC");
  else Serial.println("Unknown");
  
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD 卡容量: %llu MB\n", cardSize);
  
  return true;
}

// ===== 初始化 GIF 播放器 =====
bool initGIF() {
  Serial.println("初始化 GIF 播放器...");
  
  gif.begin(LITTLE_ENDIAN_PIXELS);
  
  // 尝试打开 GIF 文件
  if (!gif.open(gifPath, GIFOpenFile, GIFCloseFile, GIFReadFile, GIFSeekFile, GIFDraw)) {
    Serial.printf("无法打开 GIF 文件: %s\n", gifPath);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("GIF File Not Found!", screenW/2, screenH/2 - 20, 2);
    tft.drawString(gifPath, screenW/2, screenH/2 + 20, 2);
    return false;
  }
  
  // 显示 GIF 信息
  Serial.printf("GIF 尺寸: %d x %d\n", gif.getCanvasWidth(), gif.getCanvasHeight());
  Serial.printf("GIF 循环次数: %d\n", gif.getLoopCount());
  
  return true;
}

// ===== 列出 SD 卡文件 =====
void listSDFiles() {
  Serial.println("\n===== SD 卡文件列表 =====");
  File root = SD.open("/");
  if (!root) {
    Serial.println("无法打开根目录");
    return;
  }
  
  File file = root.openNextFile();
  while (file) {
    if (!file.isDirectory()) {
      Serial.printf("  %s (%d bytes)\n", file.name(), file.size());
    }
    file = root.openNextFile();
  }
  Serial.println("========================\n");
}

// ===== Arduino Setup =====
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n===== ESP32 GIF 播放器启动 =====");
  
  // 初始化屏幕
  initTFT();
  
  // 初始化 SD 卡
  if (!initSD()) {
    while (1) delay(100);  // 如果 SD 卡初始化失败，停止运行
  }
  
  // 列出文件
  listSDFiles();
  
  // 初始化 GIF 播放器
  if (!initGIF()) {
    while (1) delay(100);  // 如果 GIF 初始化失败，停止运行
  }
  
  Serial.println("开始播放 GIF...");
  tft.fillScreen(TFT_BLACK);
}

// ===== Arduino Loop =====
void loop() {
  // 播放一帧
  int result = gif.playFrame(true, NULL);
  
  if (result == 0) {
    // GIF 播放完成，重新开始
    Serial.println("GIF 播放完成，重新开始...");
    gif.reset();
    delay(100);  // 短暂延迟
  } else if (result < 0) {
    // 播放出错
    Serial.printf("播放错误: %d\n", result);
    delay(1000);
    
    // 重新打开 GIF
    gif.close();
    if (!initGIF()) {
      while (1) delay(100);
    }
  }
  
  // 可选：添加帧延迟控制
  // delay(50);  // 如果播放太快，可以添加延迟
}
