#include <Wire.h>
#include <SPI.h>
#include <MKRIMU.h>
#include <Adafruit_BMP3XX.h>
#include <SD.h>


// Create sensor objects
Adafruit_MPU6050 mpu;
Adafruit_BMP085 bmp;

// Variables to store integrated angles
float angleX = 0;
float angleY = 0;
float angleZ = 0;

// Time tracking
unsigned long lastTime;
unsigned long currentTime;
unsigned long deltaTime;
unsigned long stageDeltaTime;

//Sensor Data
double altOffset;
double zedOffset;
double xOffset;
double yOffset;
float altitude;

//Sensor DataLogging
void sensorPowerOn(){
  // Initialize serial communication
  Serial.begin(9600);
  while (!Serial) {
    delay(10); // Wait for serial monitor to open
  }

  // Initialize I2C communication
  Wire.begin();

  // Scan for I2C devices
  Serial.println("Scanning I2C bus...");
  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.println(" !");
    } else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  Serial.println("Done.");

  // Initialize MPU6050
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  // Initialize BMP180
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP180 sensor, check wiring!");
    while (1) {
      delay(10);
    }
  }
  Serial.println("BMP180 Found!");

  // Initialize time tracking
  lastTime = millis();
}



void sensorRead(double altOffset, double zOffset, double xOffset, double yOffset){
  // Get current time
  currentTime = millis();
  float deltaTime = (currentTime - lastTime) / 1000.0; // Convert to seconds
  lastTime = currentTime;

  // Get MPU6050 events
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Integrate gyroscope data to get angles
  angleX += (g.gyro.x * deltaTime * 180.0 / PI)- xOffset;
  angleY += (g.gyro.y * deltaTime * 180.0 / PI) - yOffset;
  angleZ += (g.gyro.z * deltaTime * 180.0 / PI) - zedOffset;

  // Print angles
  Serial.print("Angle X: ");
  Serial.print(angleX);
  Serial.print(", Angle Y: ");
  Serial.print(angleY);
  Serial.print(", Angle Z: ");
  Serial.println(angleZ);
  Serial.println("X Rotation: ");
  Serial.print(g.gyro.x);
  Serial.print(", Y Rotation: ");
  Serial.print(g.gyro.y);
  Serial.print(", X Rotation: ");
  Serial.print(g.gyro.z);

  float altitude = bmp.readAltitude(1013.25) - altOffset; // Adjust sea level pressure if needed

  // Print BMP180 data
  Serial.print(" Pa, Altitude: ");
  Serial.print(altitude);
  Serial.println(" m");

  // Delay between readings
  delay(100);
}

String stageDetermination(){
   // Get current time
  currentTime = millis();
  stageDeltaTime = (currentTime - lastTime) / 1000.0; // Convert to seconds
  lastTime = currentTime;

  // Get MPU6050 events
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  
  // Acceleration delta

  //stage of Flight determination
  if (altitude < 20 && a.acceleration.z < 10){
    return "idle";
  }
  else if (a.acceleration.z > 8){
    return "take off";
  }

}

void onSensorPowerOn(){
   // Get current time
  currentTime = millis();
  stageDeltaTime = (currentTime - lastTime) / 1000.0; // Convert to seconds
  lastTime = currentTime;

  // Get MPU6050 events
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
    
  altOffset= bmp.readAltitude(1013.25);
  zedOffset += g.gyro.z * deltaTime * 180.0/PI;
  xOffset += g.gyro.x * deltaTime * 180.0/PI;
  yOffset +=  g.gyro.y * deltaTime * 180.0/PI;
}




void setup() {
  sensorPowerOn();
  onSensorPowerOn();
}

void loop() {
  sensorRead(altOffset, xOffset, yOffset,zedOffset);
}
