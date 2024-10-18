#include "sensors.h"
#include <Arduino.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include "DFRobot_PH.h"
#include "DFRobot_EC.h"

#define DHTPIN A3
#define DHTTYPE DHT11
DHT dhtOuter(DHTPIN, DHTTYPE);

// I2C LCD 설정
LiquidCrystal_I2C lcd(0x27, 16, 2);

// 센서 핀 설정
#define WATER_LEVEL_PIN 2
#define PH_SENSOR_PIN A1
#define CONDUCTIVITY_SENSOR_PIN A2
#define NUTRIENT_TEMP_PIN A4

// pH 및 EC 센서 객체
DFRobot_PH phSensor;
DFRobot_EC ecSensor;

// 전역 변수
float phVoltage, phValue, ecVoltage, ecValue, nutrientTemp = 26.9;

// 센서 초기화 함수
void initializeSensors() {
    dhtOuter.begin();
    lcd.init();
    lcd.backlight();

    // pinMode: 디지털 핀의 입출력 설정
    pinMode(WATER_LEVEL_PIN, INPUT);
    pinMode(PH_SENSOR_PIN, INPUT);
    pinMode(CONDUCTIVITY_SENSOR_PIN, INPUT);
    pinMode(NUTRIENT_TEMP_PIN, INPUT);

    phSensor.begin();  // pH 센서 초기화
    ecSensor.begin();  // 전도도 센서 초기화
}

// 센서 값 읽기 함수
SensorData readSensors() {
    SensorData data;
    
    // 수위 센서 (디지털)
    data.waterLevel = digitalRead(WATER_LEVEL_PIN);
    
    // pH 센서 전압 값 읽기 및 온도 보정 적용
    phValue = getCurrentPh();
    data.phValue = phValue;

    // 전도도 센서 전압 값 읽기 및 온도 보정 적용
    ecValue = getCurrentConductivity();
    data.conductivity = ecValue;

    // 외부 온도 및 습도 값 읽기
    data.outerTemp = getCurrentTemperature();
    data.outerHumidity = dhtOuter.readHumidity();

    // 값이 NaN인지 확인
    if (isnan(data.outerTemp)) {
        data.outerTemp = 0;
        Serial.println("경고: 대기의 온도 센서가 값을 읽지 못했습니다.");
    }

    if (isnan(data.outerHumidity)) {
        data.outerHumidity = 0;
        Serial.println("경고: 대기의 습도 센서가 값을 읽지 못했습니다.");
    }

    if (isnan(data.nutrientTemp)) {
        data.nutrientTemp = 25;  // 기본값 설정
        Serial.println("경고: 양액 온도 센서가 연결되지 않았습니다. 기본값 25°C 사용.");
    }

    if (isnan(data.waterLevel)) {
        data.waterLevel = 0;
        Serial.println("경고: 수위 센서가 값을 읽지 못했습니다.");
    }

    if (isnan(data.phValue)) {
        data.phValue = 0;
        Serial.println("경고: pH 센서가 값을 읽지 못했습니다.");
    }

    if (isnan(data.conductivity)) {
        data.conductivity = 0;
        Serial.println("경고: 전도도 센서가 값을 읽지 못했습니다.");
    }

    return data;
}

float getCurrentTemperature() {
    return dhtOuter.readTemperature();
}

float getCurrentPh() {
    phVoltage = analogRead(PH_SENSOR_PIN) / 1024.0 * 5000;
    return phSensor.readPH(phVoltage, nutrientTemp);
}

float getCurrentConductivity() {
    ecVoltage = analogRead(CONDUCTIVITY_SENSOR_PIN) / 1024.0 * 5000;
    return ecSensor.readEC(ecVoltage, nutrientTemp);
}

void displayDataOnLCD(float temp, float humidity) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temp);
    lcd.print("'C");

    lcd.setCursor(0, 1);
    lcd.print("Humidity: ");
    lcd.print(humidity);
    lcd.print("%");
}
