#include <Arduino.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include "sensors.h"

// DHT 설정 (대기의 온습도 측정, A3 핀 사용)
#define DHTPIN A3
#define DHTTYPE DHT11
DHT dhtOuter(DHTPIN, DHTTYPE);

// I2C LCD 설정
LiquidCrystal_I2C lcd(0x27, 16, 2);

// 센서 핀 설정
#define WATER_LEVEL_PIN A0
#define PH_SENSOR_PIN A1
#define CONDUCTIVITY_SENSOR_PIN A2
#define NUTRIENT_TEMP_PIN A4  // 양액 온도 센서

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

    // 센서 데이터 읽기
    data.waterLevel = analogRead(WATER_LEVEL_PIN);
    data.phValue = analogRead(PH_SENSOR_PIN);
    data.conductivity = analogRead(CONDUCTIVITY_SENSOR_PIN);
    data.outerTemp = dhtOuter.readTemperature();
    data.outerHumidity = dhtOuter.readHumidity();
    data.nutrientTemp = analogRead(NUTRIENT_TEMP_PIN);

    // 예외 처리: 연결되지 않은 센서의 값을 0으로 처리
    if (isnan(data.outerTemp)) {
        data.outerTemp = 0;
        Serial.println("경고: 대기의 온도 센서가 연결되지 않았습니다.");
    }

    if (isnan(data.outerHumidity)) {
        data.outerHumidity = 0;
        Serial.println("경고: 대기의 습도 센서가 연결되지 않았습니다.");
    }

    if (isnan(data.phValue)) {
        data.phValue = 0;
        Serial.println("경고: pH 센서가 연결되지 않았습니다.");
    }

    if (isnan(data.conductivity)) {
        data.conductivity = 0;
        Serial.println("경고: Conductivity 센서가 연결되지 않았습니다.");
    }

    if (isnan(data.nutrientTemp)) {
        data.nutrientTemp = 0;
        Serial.println("경고: 양액 온도 센서가 연결되지 않았습니다.");
    }

    if (isnan(data.waterLevel)) {
        data.waterLevel = 0;
        Serial.println("경고: 수위 센서가 연결되지 않았습니다.");
    }

    // LCD에 대기의 온도 및 습도 표시
    displayDataOnLCD(data.outerTemp, data.outerHumidity);

    return data;
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
