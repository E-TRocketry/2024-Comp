#ifndef STAGE_DETECTION_H
#define STAGE_DETECTION_H

#include <Wire.h>
#include <MPU6050.h>
#include <BMP085.h>

// Define thresholds and other parameters
const float launch_accel_threshold;
const float coast_accel_threshold;
const float apogee_altitude_threshold;

enum FlightPhase {
    GROUND,
    POWERED_ASCENT,
    COASTING,
    APOGEE,
    DESCENT,
    LANDING
};

FlightPhase current_stage; 


// Function prototypes
void initializeSensors();
void readSensorData(float &accel_x, float &accel_y, float &accel_z, float &altitude);
void detectFlightPhase(float accel_x, float accel_y, float accel_z, float altitude, float current_pressure, float last_pressure, float current_phase);


#endif