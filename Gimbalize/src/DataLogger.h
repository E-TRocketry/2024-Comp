#ifndef DATALOGGER_H
#define DATALOGGER_H

#include "Sensors.h"

void initializeSDCard();
void createLogFile();
void logData(const SensorData& data);

#endif