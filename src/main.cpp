#include <Arduino.h>

#define RADAR_SERIAL Serial1
#define RADAR_RX_PIN 20
#define RADAR_TX_PIN 21
#define RADAR_BAUDRATE 57600

#define BUFFER_SIZE 133

int distH = 0, distL = 0, dist = 0;
int cnt = 0;
int delay_length_ms = 1;
unsigned char buffer[BUFFER_SIZE] = {};

void setup() {
  Serial.begin(115200);
  RADAR_SERIAL.begin(RADAR_BAUDRATE, SERIAL_8N1, RADAR_RX_PIN, RADAR_TX_PIN);
//  pinMode(8, OUTPUT);
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