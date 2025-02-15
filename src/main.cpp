#include <Arduino.h>
#include <SPI.h>
//#include <Adafruit_ST7789.h>
#include <Adafruit_ST7735.h>
#include <Adafruit_GFX.h>

#define ST7789_DRIVER
//#define TFT_WIDTH 128
//#define TFT_HEIGHT 128
#define TFT_WIDTH 180
#define TFT_HEIGHT 180

#define LOAD_GLCD
//#define TOUCH_CS -1

#define TFT_CS 0
#define TFT_MOSI 6
#define TFT_MISO 5
#define TFT_SCLK 4
#define TFT_RST 3
#define TFT_DC 1

#define RADAR_SERIAL Serial1
#define RADAR_RX_PIN 20
#define RADAR_TX_PIN 21
#define RADAR_BAUDRATE 57600
#define BUFFER_SIZE 133

int distH = 0, distL = 0, dist = 0;
int cnt = 0;
int line_cnt = 0;
int delay_length_ms = 1;
unsigned char buffer[BUFFER_SIZE] = {};

uint8_t image_array[TFT_HEIGHT][TFT_WIDTH] = {0};

//Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST, TFT_MOSI, TFT_SCLK);
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Convert 24-bit RGB to 16-bit RGB565
uint16_t rgbToRGB565(uint8_t r, uint8_t g, uint8_t b) {
    return ((b >> 3) << 11) | ((g >> 2) << 5) | (r >> 3);
}

uint16_t plasmaColorMap[] = {
    rgbToRGB565(13, 8, 135), rgbToRGB565(17, 11, 140), rgbToRGB565(22, 15, 146),
    rgbToRGB565(26, 19, 152), rgbToRGB565(31, 22, 158), rgbToRGB565(35, 26, 163),
    rgbToRGB565(40, 30, 169), rgbToRGB565(45, 34, 175), rgbToRGB565(49, 38, 181),
    rgbToRGB565(54, 42, 186), rgbToRGB565(58, 46, 192), rgbToRGB565(63, 50, 197),
    rgbToRGB565(67, 54, 203), rgbToRGB565(71, 58, 208), rgbToRGB565(76, 62, 214),
    rgbToRGB565(80, 66, 219), rgbToRGB565(85, 70, 224), rgbToRGB565(89, 74, 229),
    rgbToRGB565(93, 77, 234), rgbToRGB565(97, 81, 239), rgbToRGB565(102, 85, 244),
    rgbToRGB565(106, 89, 248), rgbToRGB565(110, 92, 253), rgbToRGB565(114, 96, 255),
    rgbToRGB565(118, 99, 255), rgbToRGB565(122, 103, 255), rgbToRGB565(126, 106, 255),
    rgbToRGB565(130, 109, 255), rgbToRGB565(134, 112, 255), rgbToRGB565(138, 115, 255),
    rgbToRGB565(142, 118, 255), rgbToRGB565(146, 121, 255), rgbToRGB565(150, 124, 255),
    rgbToRGB565(154, 127, 255), rgbToRGB565(158, 130, 255), rgbToRGB565(162, 133, 255),
    rgbToRGB565(166, 136, 255), rgbToRGB565(170, 139, 255), rgbToRGB565(174, 142, 255),
    rgbToRGB565(178, 145, 255), rgbToRGB565(182, 148, 255), rgbToRGB565(186, 151, 255),
    rgbToRGB565(190, 154, 255), rgbToRGB565(194, 157, 255), rgbToRGB565(198, 160, 255)
};

uint16_t mapValueToPlasmaColor(int value) {
    if (value < 1) value = 1;
    if (value > 44) value = 44;
    return plasmaColorMap[value - 1];
}

void setup() {
  Serial.begin(115200);
//  tft.init(TFT_HEIGHT, TFT_WIDTH);
  tft.initR(INITR_MINI160x80);
  tft.fillScreen(ST77XX_BLUE);
  tft.setRotation(3);
  delay(3000);
  Serial.println("TFT initialized");
  tft.drawRect(0, 0, 126, 80, ST77XX_RED);
  Serial.println("Rectangle drawn");
  tft.setTextColor(ST77XX_GREEN);
  Serial.println("Text color set");
  tft.setTextSize(2);
  Serial.println("Text size set");
  tft.setCursor(10, 20);
  Serial.println("Cursor set");
  tft.println("Hello!");
  Serial.println("Text printed");

  RADAR_SERIAL.begin(RADAR_BAUDRATE, SERIAL_8N1, RADAR_RX_PIN, RADAR_TX_PIN);
}

void loop() {
  cnt++;
  if (RADAR_SERIAL.available()) {
  if (RADAR_SERIAL.read() == 0xff) {
    int bytesRead = RADAR_SERIAL.readBytes(buffer, BUFFER_SIZE);
    if (bytesRead != BUFFER_SIZE) {
      delay_length_ms++;
    }
    Serial.println(bytesRead);

        for (int i = 4; i < 131; i++) {
          Serial.print(buffer[i]);
          Serial.print(" ");
        }
        Serial.println();

      if ((buffer[0] == 0xff) && (buffer[1] == 0xff) && (buffer[131] == 0x00) && (buffer[132] == 0x00) && (buffer[133] == 0x00)){
        RADAR_SERIAL.flush();
        distH = buffer[2];
        distL = buffer[3];
        dist = (distH << 8) + distL;
        Serial.print(cnt);
        Serial.write("|Delay: ");
        Serial.print(delay_length_ms);
        Serial.write("|Distance: ");
        Serial.println(dist);
        for (int i = 4; i < 131; i++) {
          Serial.print(buffer[i]);
          Serial.print(" ");
          tft.drawPixel(i - 4, line_cnt, mapValueToPlasmaColor(buffer[i]));
        }
        line_cnt++;
        if (line_cnt >= 80) {
          line_cnt = 0;
          tft.fillScreen(ST77XX_BLACK);
        }
        Serial.println();
        delay(delay_length_ms);
      }
  }
}
delay(1);
}