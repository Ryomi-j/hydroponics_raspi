#include "sensors.h"
#include <Arduino.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>

// DHT 설정
#define DHTPIN A3
#define DHTTYPE DHT11
DHT dhtOuter(DHTPIN, DHTTYPE);

// I2C LCD 설정
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define WATER_LEVEL_PIN A0
#define PH_SENSOR_PIN A1
#define CONDUCTIVITY_SENSOR_PIN A2
#define NUTRIENT_TEMP_PIN A4

void initializeSensors() {
    dhtOuter.begin();
    lcd.init();
    lcd.backlight();
    pinMode(WATER_LEVEL_PIN, INPUT);
    pinMode(PH_SENSOR_PIN, INPUT);
    pinMode(CONDUCTIVITY_SENSOR_PIN, INPUT);
    pinMode(NUTRIENT_TEMP_PIN, INPUT);
}

SensorData readSensors() {
    SensorData data;
    data.waterLevel = analogRead(WATER_LEVEL_PIN);
    data.phValue = analogRead(PH_SENSOR_PIN);
    data.conductivity = analogRead(CONDUCTIVITY_SENSOR_PIN);
    data.outerTemp = dhtOuter.readTemperature();
    data.outerHumidity = dhtOuter.readHumidity();
    data.nutrientTemp = analogRead(NUTRIENT_TEMP_PIN);

    return data;
}

float getCurrentTemperature() {
    return dhtOuter.readTemperature();
}

float getCurrentPh() {
    return analogRead(PH_SENSOR_PIN);
}

float getCurrentConductivity() {
    return analogRead(CONDUCTIVITY_SENSOR_PIN);
}

void displayDataOnLCD(float temp, float humidity) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temp);
    lcd.print("C");

    lcd.setCursor(0, 1);
    lcd.print("Humidity: ");
    lcd.print(humidity);
    lcd.print("%");
}
