# ESP32 GIF Player

A GIF animation player based on ESP32 + ST7789 TFT display, supporting reading GIF files from TF card and looping playback.

## Hardware Requirements

- **ESP32 Development Board** (ESP32-DevKit or similar)
- **ST7789 TFT Display** (240x320)
- **TF Card Module** (MicroSD card reader)
- **TF Card** (to store GIF files)

## Pin Connections

### TFT Display (ST7789)
| TFT Pin | ESP32 Pin |
|---------|----------|
| VCC     | 3.3V     |
| GND     | GND      |
| CS      | GPIO 15  |
| DC      | GPIO 2   |
| RST     | GPIO 4   |
| SCK     | GPIO 18  |
| MOSI    | GPIO 23  |
| BL      | GPIO 21  |

### TF Card Module
| SD Pin | ESP32 Pin |
|--------|----------|
| VCC    | 3.3V      |
| GND    | GND       |
| CS     | GPIO 5    |
| SCK    | GPIO 18   |
| MOSI   | GPIO 23   |
| MISO   | GPIO 19   |

> **Note**: TFT and SD card share the same SPI bus (SCK and MOSI).

## Software Dependencies

This project uses PlatformIO and depends on the following libraries:
- `TFT_eSPI` - TFT display driver library
- `AnimatedGIF` - GIF decoding library
- `SD` - SD card file system (built-in Arduino)

Dependencies are configured in `platformio.ini` and will be downloaded automatically.

## Usage

### 1. Prepare GIF File
- Name your GIF file as `anim.gif`
- Copy it to the root directory of the TF card
- Recommended GIF size: no larger than screen resolution (240x320)
- Recommended file size: within 1MB (larger files may affect playback smoothness)

### 2. Compile and Upload
```bash
# Using PlatformIO CLI
pio run --target upload

# Or click "Upload" button in VS Code
```

### 3. View Serial Output
```bash
pio device monitor
# Or click "Monitor" button in VS Code
```

Serial output will show:
- SD card initialization info
- SD card file list
- GIF file info
- Playback status

## Configuration

### Change GIF File Path
Modify in [src/main.cpp](src/main.cpp#L22):
```cpp
const char* gifPath = "/anim.gif";  // Change to your GIF filename
```

### Adjust Pin Definitions
If your hardware wiring is different, modify:
1. TFT pin definitions in **platformio.ini**
2. SD card pin definitions in [src/main.cpp](src/main.cpp#L13-L16)

### Adjust Screen Orientation
Modify in [src/main.cpp](src/main.cpp#L92):
```cpp
tft.setRotation(1);  // 0-3 correspond to 0°, 90°, 180°, 270°
```

### Adjust Playback Speed
If GIF plays too fast, add delay in [src/main.cpp](src/main.cpp#L208):
```cpp
delay(50);  // Delay 50ms
```

## Troubleshooting

### SD Card Initialization Failure
- Check if the SD card is inserted correctly
- Check if the pin connections are correct
- Try formatting the SD card to FAT32 format
- Confirm the SD card power supply is stable (3.3V)

### Screen Not Displaying or Displaying a Distorted Image
- Check TFT pin connections
- Confirm the screen model is ST7789
- Try changing the screen resolution (240x280 or 240x320)
- Try enabling/disabling color inversion in User_Setup.h

### GIF File Not Found
- Confirm the file is in the SD card root directory
- Confirm the file name is `anim.gif` (lowercase)
- Check the serial port output file list

### GIF Playback Stuttering
- Reduce GIF file size
- Reduce GIF resolution
- Reduce GIF frame rate
- Use a lower quality GIF

## Supported GIF Format

- **Standard GIF** (GIF87a, GIF89a)
- **Maximum Size**: Recommended not to exceed 240x320
- **Transparency**: Single color transparency supported
- **Colors**: 256 color palette
- **Looping**: Loop playback supported

## Example GIF Tools

You can use the following tools to create or convert GIFs:
- [ezgif.com](https://ezgif.com/) - Online GIF editor
- [GIMP](https://www.gimp.org/) - Open source image editor
- [FFmpeg](https://ffmpeg.org/) - Video to GIF conversion

Conversion command examples:
```bash
# Convert from video
ffmpeg -i input.mp4 -vf "fps=10,scale=240:-1:flags=lanczos" -c:v gif output.gif

# Resize GIF
ffmpeg -i input.gif -vf "scale=240:-1:flags=lanczos" output.gif
```

## License

MIT License - Free to use and modify

## References

- [TFT_eSPI Library Documentation](https://github.com/Bodmer/TFT_eSPI)
- [AnimatedGIF Library Documentation](https://github.com/bitbank2/AnimatedGIF)
- [ESP32 Arduino Documentation](https://docs.espressif.com/projects/arduino-esp32/)
