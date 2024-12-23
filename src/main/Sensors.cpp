#include "Sensors.h"
#include <MKRIMU.h>
#include <math.h>
#include <Adafruit_BMP3XX.h>
#include <SimpleKalmanFilter.h>

// Constants for BMP sensors
#define BMP1_ADDR 0x76 // Address of the first BMP390
#define BMP2_ADDR 0x77 // Address of the second BMP390
#define SEALEVELPRESSURE_HPA 1013.25 // Standard sea-level pressure in hPa
#define APOGEE_THRESHOLD -0.5 // Apogee detection threshold: Negative rate of climb

// Constants for velocity and motion
#define ACCEL_THRESHOLD 0.1  // Threshold for motion detection
#define RESET_THRESHOLD 0.05 // Velocity threshold to reset on stationary
#define DRAG_FACTOR 0.98     // Drag factor for velocity decay

// Instances for BMP sensors
Adafruit_BMP3XX bmp1;
Adafruit_BMP3XX bmp2;

// Define Kalman filter for altitude
SimpleKalmanFilter altitudeFilter(2.0, 2.0, 0.5); // Measurement error, Estimate error, Process noise

// Variables for altitude and rate of climb
float previousAltitude = 0;
float accelBaselineX = 0, accelBaselineY = 0, accelBaselineZ = GRAVITY;

void initializeSensors() {
    if (!IMU.begin()) {
        Serial.println("Failed to initialize MKRIMU");
        while (1) delay(10); // Halt on failure
    }
    Serial.println("MKRIMU Initialized");

    // Initialize BMP sensors
    if (!bmp1.begin_I2C(BMP1_ADDR)) {
        Serial.println("Sensor 1 not found at address 0x76");
    } else {
        Serial.println("Sensor 1 initialized at address 0x76");
    }

    if (!bmp2.begin_I2C(BMP2_ADDR)) {
        Serial.println("Sensor 2 not found at address 0x77");
    } else {
        Serial.println("Sensor 2 initialized at address 0x77");
    }

    if (!bmp1.begin_I2C(BMP1_ADDR) && !bmp2.begin_I2C(BMP2_ADDR)) {
        Serial.println("Both BMP sensors failed to initialize. Check wiring.");
        while (1);
    }

    // Configure BMP sensors
    if (bmp1.begin_I2C(BMP1_ADDR)) {
        bmp1.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
        bmp1.setPressureOversampling(BMP3_OVERSAMPLING_4X);
        bmp1.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
        bmp1.setOutputDataRate(BMP3_ODR_50_HZ);
    }

    if (bmp2.begin_I2C(BMP2_ADDR)) {
        bmp2.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
        bmp2.setPressureOversampling(BMP3_OVERSAMPLING_4X);
        bmp2.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
        bmp2.setOutputDataRate(BMP3_ODR_50_HZ);
    }

    Serial.println("Initialization complete.");
}

SensorData readSensors(float deltaTime, SensorData& previousData) {
    SensorData data;

    float accelX_raw = 0, accelY_raw = 0, accelZ_raw = 0;
    float gyroX_raw = 0, gyroY_raw = 0, gyroZ_raw = 0;

    // Read gyroscope data
    if (IMU.gyroscopeAvailable()) {
        IMU.readGyroscope(gyroX_raw, gyroY_raw, gyroZ_raw);

        // Convert gyroscope readings to degrees per second
        data.gyroX = gyroX_raw * 180.0 / M_PI;
        data.gyroY = gyroY_raw * 180.0 / M_PI;
        data.gyroZ = gyroZ_raw * 180.0 / M_PI;

        // Update roll, pitch, yaw using gyroscope data
        data.angleX = previousData.angleX + data.gyroX * deltaTime;
        data.angleY = previousData.angleY + data.gyroY * deltaTime;
        data.angleZ = previousData.angleZ + data.gyroZ * deltaTime;
    }

    // Read accelerometer data
    if (IMU.accelerationAvailable()) {
        IMU.readAcceleration(accelX_raw, accelY_raw, accelZ_raw);

        // Subtract baseline and apply threshold
        float accelX_filtered = accelX_raw - accelBaselineX;
        float accelY_filtered = accelY_raw - accelBaselineY;
        float accelZ_filtered = accelZ_raw - accelBaselineZ;

        if (fabs(accelX_filtered) < ACCEL_THRESHOLD) accelX_filtered = 0;
        if (fabs(accelY_filtered) < ACCEL_THRESHOLD) accelY_filtered = 0;
        if (fabs(accelZ_filtered) < ACCEL_THRESHOLD) accelZ_filtered = 0;

        // Update data fields
        data.accelX = accelX_filtered * GRAVITY;
        data.accelY = accelY_filtered * GRAVITY;
        data.accelZ = accelZ_filtered * GRAVITY;

        // Integrate acceleration to velocity
        if (fabs(data.accelX) > RESET_THRESHOLD || fabs(data.accelY) > RESET_THRESHOLD || fabs(data.accelZ) > RESET_THRESHOLD) {
            data.velocityX = previousData.velocityX + data.accelX * deltaTime;
            data.velocityY = previousData.velocityY + data.accelY * deltaTime;
            data.velocityZ = previousData.velocityZ + data.accelZ * deltaTime;
        } else {
            data.velocityX = 0;
            data.velocityY = 0;
            data.velocityZ = 0;
        }

        // Apply drag to reduce drift
        data.velocityX *= DRAG_FACTOR;
        data.velocityY *= DRAG_FACTOR;
        data.velocityZ *= DRAG_FACTOR;

        // Integrate velocity to calculate position
        data.positionX = previousData.positionX + data.velocityX * deltaTime;
        data.positionY = previousData.positionY + data.velocityY * deltaTime;
        data.positionZ = previousData.positionZ + data.velocityZ * deltaTime;

        // Debugging output
        Serial.print("Velocity X: ");
        Serial.println(data.velocityX);
        Serial.print("Position X: ");
        Serial.println(data.positionX);
    }

    // Read altitude from BMP sensor
    float rawAltitude = readAltitudeFromBMP();
    data.altitude = altitudeFilter.updateEstimate(rawAltitude);
    data.rateOfChange = data.altitude - previousAltitude;

    // Apogee detection
    if (data.rateOfChange < APOGEE_THRESHOLD) {
        Serial.println("Apogee detected!");
    }

    // Update previous altitude
    previousAltitude = data.altitude;

    data.timestamp = millis();
    return data;
}

float readAltitudeFromBMP() {
    if (bmp1.performReading()) {
        return bmp1.readAltitude(SEALEVELPRESSURE_HPA);
    } else if (bmp2.performReading()) {
        return bmp2.readAltitude(SEALEVELPRESSURE_HPA);
    } else {
        Serial.println("Failed to read altitude from BMP sensors.");
        return 0; // Default value in case of failure
    }
}