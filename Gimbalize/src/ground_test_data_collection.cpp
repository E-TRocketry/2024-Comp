#include <Arduino_LSM9DS1.h>
#include <SPI.h>
#include <SD.h>

// Define LED pins
#define LEDR 2
#define LEDG 3
#define LEDB 4

// Define SD card pin
#define SD_PIN 10

// Declare file for SD operations
File myFile;

void setup() {
  // Initialize pins as outputs
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);

  // Start serial communication
  Serial.begin(9600);
  while (!Serial);

  Serial.println("Started");

  // Initialize IMU
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");

    // RED LED to indicate error
    digitalWrite(LEDR, LOW);
    digitalWrite(LEDG, HIGH);
    digitalWrite(LEDB, HIGH);
    delay(1000);

    // RED LED stays on
    digitalWrite(LEDR, HIGH);
    while (1);
  }
  Serial.println("IMU initialized!");

  // Initialize SD card
  if (!SD.begin(SD_PIN)) {
    Serial.println("SD initialization failed!");

    // RED LED to indicate error
    digitalWrite(LEDR, LOW);
    digitalWrite(LEDG, HIGH);
    digitalWrite(LEDB, HIGH);
    delay(1000);

    // RED LED stays on
    digitalWrite(LEDR, HIGH);
    while (1);
  }
  Serial.println("SD card initialized!");
}

void loop() {
  // Variable to store test file number
  int test_number = 1;
  String filename = "test_" + String(test_number) + ".txt";

  // Increment file number if the file already exists
  while (SD.exists(filename)) {
    test_number++;
    filename = "test_" + String(test_number) + ".txt";
  }

  // Open file for writing
  myFile = SD.open(filename, FILE_WRITE);
  if (!myFile) {
    Serial.println("Failed to create file!");
    return;
  }

  // Write header to the file
  myFile.println("Gyro_X, Gyro_Y, Gyro_Z, Acc_X, Acc_Y, Acc_Z, Mag_X, Mag_Y, Mag_Z");
  Serial.println("Gyro_X, Gyro_Y, Gyro_Z, Acc_X, Acc_Y, Acc_Z, Mag_X, Mag_Y, Mag_Z");

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

    delay(100);
  }

  // Close the file
  myFile.close();
  Serial.println("Data saved successfully!");

  // GREEN LED to indicate success
  digitalWrite(LEDR, HIGH);
  digitalWrite(LEDG, LOW);
  digitalWrite(LEDB, HIGH);
  delay(5000);

  // Turn off LEDs
  digitalWrite(LEDR, HIGH);
  digitalWrite(LEDG, HIGH);
  digitalWrite(LEDB, HIGH);
}
