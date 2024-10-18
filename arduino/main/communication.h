#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "sensors.h"

void sendDataToRaspberryPi(SensorData data);
bool receiveSettingsFromRaspberryPi();

#endif
