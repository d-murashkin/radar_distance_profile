#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_ST7789.h>
//#include <Adafruit_ST7735.h>
#include <Adafruit_GFX.h>

#define TFT_WIDTH 128
#define TFT_HEIGHT 128
//#define TFT_CS 7
//#define TFT_MOSI 6
//#define TFT_MISO 5
//#define TFT_SCLK 4
//#define TFT_RST 3
//#define TFT_DC 2
#define TFT_CS 10
#define TFT_MOSI 7
#define TFT_MISO 2
#define TFT_SCLK 6
#define TFT_RST 3
#define TFT_DC 1

//#define TFT_SPI_MODE 0
//#define USE_SOFTWARE_SPI


#define RADAR_SERIAL Serial1
#define RADAR_RX_PIN 20
#define RADAR_TX_PIN 21
#define RADAR_BAUDRATE 57600
#define BUFFER_SIZE 133

int distH = 0, distL = 0, dist = 0;
int cnt = 0;
int delay_length_ms = 1;
unsigned char buffer[BUFFER_SIZE] = {};

uint8_t image_array[TFT_HEIGHT][TFT_WIDTH] = {0};

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST, TFT_MOSI, TFT_SCLK);
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

/*
uint8_t get_color(uint8_t intensity) {
  uint8_t r, g, b;
  uint8_t intensity_threshold = 14;
  if (intensity < intensity_threshold){
    r = 0;
    g = (intensity * 10);
    b = 255;
  } else if (intensity < intensity_threshold * 2) {
    r = 0;
    g = 255;
    b = 255 - ((intensity - intensity_threshold) * 10);
  } else {
    r = (intensity - intensity_threshold * 2) * 10;
    g = 255 - ((intensity - intensity_threshold * 2) * 10);
    b = 0;
  }
  return tft.color565(r, g, b);
}
  */

void setup() {
  Serial.begin(115200);
  RADAR_SERIAL.begin(RADAR_BAUDRATE, SERIAL_8N1, RADAR_RX_PIN, RADAR_TX_PIN);
  pinMode(3, OUTPUT);
    delay(100);
    digitalWrite(3, HIGH);
    delay(100);
    digitalWrite(3, LOW);
  tft.init(TFT_HEIGHT, TFT_WIDTH);
  tft.setSPISpeed(1000000);
//  tft.initR(INITR_MINI160x80);
  tft.fillScreen(ST77XX_BLUE);
//  tft.setRotation(0);
  delay(5000);
  Serial.println("TFT initialized");
  delay(1000);
  tft.drawRect(10, 10, 50, 50, ST77XX_RED);
  Serial.println("Rectangle drawn");
  /*
  for (int y = 0; y < TFT_HEIGHT; y++) {
    for (int x = 0; x < TFT_WIDTH; x++) {
////      uint16_t color = get_color(image_array[y][x]);
//      uint16_t color = tft.color565(128, 0, 0);
//      tft.drawPixel(x, y, color);
      tft.drawPixel(x, y, ST77XX_RED);
    }
  }
  */
  tft.setTextColor(ST77XX_GREEN);
  Serial.println("Text color set");
  tft.setTextSize(2);
  Serial.println("Text size set");
  tft.setCursor(10, 20);
  Serial.println("Cursor set");
  tft.println("Hello!");
  Serial.println("Text printed");
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
        }
        Serial.println();
        delay(delay_length_ms);
      }
  }
}
delay(1);
}