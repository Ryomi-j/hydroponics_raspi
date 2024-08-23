#ifndef SENSORS_H
#define SENSORS_H

struct SensorData {
    float waterLevel;
    float phValue;
    float conductivity;
    float outerTemp;
    float outerHumidity;
    float nutrientTemp;
};

void initializeSensors();
SensorData readSensors();
void displayDataOnLCD(float temp, float humidity);

float getCurrentTemperature();
float getCurrentPh();
float getCurrentConductivity();

#endif
