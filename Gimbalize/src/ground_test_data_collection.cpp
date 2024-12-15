#include <MKRIMU.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>

// Define LED pins
#define STATUSLED 13

// Define SD card pin
#define SD_PIN 10

// Define barometer settings
#define BMP_SDA A4 // Adjust for your microcontroller
#define BMP_SCL D12
#define SEALEVELPRESSURE_HPA (1013.25)

// Declare file for SD operations
File myFile;

void setup() {
  // Initialize pins as outputs
  pinMode(STATUSLED, OUTPUT);

  // Start serial communication
  Serial.begin(9600);
  while (!Serial);

  Serial.println("Started");

  // Initialize IMU
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");

    digitalWrite(STATUSLED, HIGH);
    delay(100)
    digitalWrite(STATUSLED, LOW);
    while (1);
  }
  Serial.print("Gyroscope sample rate = ");
  Serial.print(IMU.gyroscopeSampleRate());
  Serial.println(" Hz");
  Serial.println();
  Serial.println("Gyroscope in degrees/second");
  Serial.println("X\tY\tZ");

  // Initialize SD card
  if (!SD.begin(SD_PIN)) {
    Serial.println("SD initialization failed!");

   digitalWrite(STATUSLED, HIGH);
   delay(500);
   digitalWrite(STATUSLED, LOW);

    // RED LED stays on
    digitalWrite(LEDR, HIGH);
    while (1);
  }
  Serial.println("SD card initialized!");

  // Initialize BMP
  if (!bmp.begin_I2C()) {   // hardware I2C mode, can pass in address & alt Wire
    Serial.println("BMP intialiation failed!");
    while (1);
  }
  Serial.println("BMP intiialized!");
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {
  // Variable to store test file number
  int test_number = 1;
  String filename = "Ground_test_" + String(test_number) + ".txt";

  // Increment file number if the file already exists
  while (SD.exists(filename)) {
    test_number++;
    filename = "Ground_test_" + String(test_number) + ".txt";
  }

  // Open file for writing
  myFile = SD.open(filename, FILE_WRITE);
  if (!myFile) {
    Serial.println("Failed to create file!");
    return;
  }

  // Write header to the file
  myFile.println("Gyro_X, Gyro_Y, Gyro_Z, Acc_X, Acc_Y, Acc_Z, Mag_X, Mag_Y, Mag_Z, BMP_Temp, BMP_Preasure, BMP_Alt");
  Serial.println("Gyro_X, Gyro_Y, Gyro_Z, Acc_X, Acc_Y, Acc_Z, Mag_X, Mag_Y, Mag_Z, BMP_Temp, BMP_Preasure, BMP_Alt");

  // Declare variables to hold sensor data
  float gx, gy, gz;
  float ax, ay, az;
  float mx, my, mz;
  

  // Read data from IMU and write to the file
  for (int i = 0; i < 400; i++) {
    if (IMU.gyroscopeAvailable()) {
      IMU.readGyroscope(gx, gy, gz);
      myFile.print(gx);
      myFile.print(", ");
      myFile.print(gy);
      myFile.print(", ");
      myFile.print(gz);
      myFile.print(", ");

      // Print gyroscope data to Serial Monitor
      Serial.print("Gyroscope - X: ");
      Serial.print(gx);
      Serial.print(" Y: ");
      Serial.print(gy);
      Serial.print(" Z: ");
      Serial.println(gz);
    }

    if (IMU.accelerationAvailable()) {
      IMU.readAcceleration(ax, ay, az);
      myFile.print(ax);
      myFile.print(", ");
      myFile.print(ay);
      myFile.print(", ");
      myFile.print(az);
      myFile.print(", ");

      // Print accelerometer data to Serial Monitor
      Serial.print("Acceleration - X: ");
      Serial.print(ax);
      Serial.print(" Y: ");
      Serial.print(ay);
      Serial.print(" Z: ");
      Serial.println(az);
    }

    if (IMU.magneticFieldAvailable()) {
      IMU.readMagneticField(mx, my, mz);
      myFile.print(mx);
      myFile.print(", ");
      myFile.print(my);
      myFile.print(", ");
      myFile.println(mz);

      // Print magnetometer data to Serial Monitor
      Serial.print("Magnetic Field - X: ");
      Serial.print(mx);
      Serial.print(" Y: ");
      Serial.print(my);
      Serial.print(" Z: ");
      Serial.println(mz);
    }

    if(bmp.performReading()) {
      myFile.print(bmp.temperature);
      myFile.print(", ");
      myFile.println(bmp.preasure/100.0);
      myFile.print(", ");
      myFile.print(bmp.readAltitude(SEALEVELPRESSURE_HPA));

      // Print into Serial Monitor
      Serial.print("Temperature: ");
      Serial.print(bmp.temperature);
      Serial.print("BMP Preasure:");
      Serial.println(bmp.preasure/100.0);
      Serial.print("Altitude Estimate: ");
      Serialln.print(bmp.readAltitude(SEALEVELPRESSURE_HPA));
    }
  }



  // Close the file
  myFile.close();
  Serial.println("Data saved successfully!");

}
