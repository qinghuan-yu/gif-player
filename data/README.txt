# GIF 播放器示例文件说明

将您的 GIF 文件命名为 `anim.gif` 并放置在 TF 卡根目录。

## GIF 要求
- 格式: GIF (GIF87a 或 GIF89a)
- 尺寸: 建议 ≤ 240x280 像素
- 大小: 建议 ≤ 1MB
- 颜色: 256 色

## 如何准备 GIF

### 在线工具
访问 https://ezgif.com/resize 调整 GIF 大小

### 使用 FFmpeg
```bash
ffmpeg -i input.gif -vf "scale=240:-1:flags=lanczos" anim.gif
```
