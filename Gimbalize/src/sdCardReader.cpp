#include <SPI.h>
#include <SD.h>

#define SD_CS_PIN SDCARD_SS_PIN

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("Initializing SD card...");
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD card initialization failed!");
    while (1);
  }
  Serial.println("SD card initialized!");

  File dataFile = SD.open("data_log.csv");
  if (dataFile) {
    Serial.println("Reading contents of data_log.csv:");
    while (dataFile.available()) {
      Serial.write(dataFile.read());
    }
    dataFile.close();
  } else {
    Serial.println("Failed to open data_log.csv!");
  }
}

void loop() {
}
