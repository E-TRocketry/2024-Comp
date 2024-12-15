#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>

Adafruit_MPU6050 mpu;
Adafruit_BMP085 bmp;

File dataFile;

float angleX = 0;
float angleY = 0;
float angleZ = 0;

unsigned long lastTime;
unsigned long currentTime;

#define SD_CS_PIN SDCARD_SS_PIN

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Wire.begin();

  Serial.println("Scanning I2C bus...");
  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
    } else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  Serial.println("Done.");

  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  if (!bmp.begin()) {
    Serial.println("Failed to find BMP180 sensor");
    while (1) {
      delay(10);
    }
  }
  Serial.println("BMP180 Found!");

  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD card initialization failed!");
    while (1) {
      delay(10);
    }
  }
  Serial.println("SD card initialized!");

  dataFile = SD.open("data_log.csv", FILE_WRITE);
  if (dataFile) {
    dataFile.println("Time (ms), Angle X (deg), Angle Y (deg), Angle Z (deg), Temperature (C), Pressure (Pa), Altitude (m)");
    dataFile.close();
  } else {
    Serial.println("Error opening file!");
  }

  lastTime = millis();
}

void loop() {
  currentTime = millis();
  float deltaTime = (currentTime - lastTime) / 1000.0;
  lastTime = currentTime;

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  angleX = g.gyro.x * deltaTime * 180.0 / PI;
  angleY = g.gyro.y * deltaTime * 180.0 / PI;
  angleZ = g.gyro.z * deltaTime * 180.0 / PI;

  float temperature = bmp.readTemperature();
  float pressure = bmp.readPressure();
  float altitude = bmp.readAltitude(1013.25);

  Serial.print("Angle X: ");
  Serial.print(angleX);
  Serial.print(", Angle Y: ");
  Serial.print(angleY);
  Serial.print(", Angle Z: ");
  Serial.print(angleZ);
  Serial.print(", Temperature: ");
  Serial.print(temperature);
  Serial.print(" C, Pressure: ");
  Serial.print(pressure);
  Serial.print(" Pa, Altitude: ");
  Serial.print(altitude);
  Serial.println(" m");

  dataFile = SD.open("data_log.csv", FILE_WRITE);
  if (dataFile) {
    dataFile.print(currentTime);
    dataFile.print(", ");
    dataFile.print(angleX);
    dataFile.print(", ");
    dataFile.print(angleY);
    dataFile.print(", ");
    dataFile.print(angleZ);
    dataFile.print(", ");
    dataFile.print(temperature);
    dataFile.print(", ");
    dataFile.print(pressure);
    dataFile.print(", ");
    dataFile.println(altitude);
    dataFile.close();
  } else {
    Serial.println("Error opening file for writing!");
  }

  delay(100);
}
