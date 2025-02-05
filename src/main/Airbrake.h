#ifndef AIRBRAKE_H
#define AIRBRAKE_H

#include <Servo.h>
#include <Sensors.cpp>

void initializeAirbrake();
void openAirbrake(bool airbrakeDeployed);
void closeAirbrake(bool airbrakeDeployed);

#endif