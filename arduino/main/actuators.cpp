#include <Arduino.h>
#include "actuators.h"
#include "sensors.h"

// 핀 정의
#define FAN_PIN 6
#define WATER_PUMP_PIN 7
#define PH_LOW_DOSING_PUMP_PIN 8
#define PH_HIGH_DOSING_PUMP_PIN 9
#define CONDUCTIVITY_LOW_DOSING_PUMP_PIN 10
#define CONDUCTIVITY_HIGH_DOSING_PUMP_PIN 11
#define LED_STRIP_PIN 12
#define WATER_LEVEL_PIN 2

bool pumpActivated = true;
bool pumpOn = false;
unsigned long pumpOnDuration = 15 * 60 * 1000;  // 펌프 동작 시간 (밀리초)
unsigned long pumpOffDuration = 45 * 60 * 1000;  // 펌프 멈춤 시간 (밀리초)
unsigned long pumpStartTime = 0;
unsigned long pumpEndTime = 86400000;  // 기본적으로 하루종일 활성화 (밀리초)

bool fanActivated = true;
float fanThresholdTemperature = 25.0;

// 사용자 설정값
float tempMin = 20.0;
float tempMax = 30.0;
float phMin = 5.5;
float phMax = 7.5;
float conductivityMin = 1.0;
float conductivityMax = 2.0;

bool ledActivated = true;
bool isLedOn = false;
unsigned long ledStartTime = 0;
unsigned long ledEndTime = 86400000;  // 기본적으로 하루종일 활성화 (밀리초)

// 타이머 관련 변수
unsigned long previousMillis = 0;
bool isPumpOn = false;

// 설정값 처리 함수들
void handlePumpOn(String value) { pumpOnDuration = value.toInt() * 1000; }
void handlePumpOff(String value) { pumpOffDuration = value.toInt() * 1000; }
void handlePumpActivation(String value) { pumpActivated = value.toInt() == 1; }
void handleFanTemp(String value) { fanThresholdTemperature = value.toFloat(); }
void handleFanActivation(String value) { fanActivated = value.toInt() == 1; }
void handleLedActivation(String value) { ledActivated = value.toInt() == 1; }
void handleLedOn(String value) { isLedOn = value.toInt() == 1; }
void handleLedStart(String value) { ledStartTime = value.toInt() * 1000; }
void handleLedEnd(String value) { ledEndTime = value.toInt() * 1000; }

// 설정값 처리 함수들을 배열로 저장
Setting settings[] = {
    {"PUMP_ON", handlePumpOn},
    {"PUMP_OFF", handlePumpOff},
    {"PUMP_ACTIVATION", handlePumpActivation},
    {"FAN_TEMP", handleFanTemp},
    {"FAN_ACTIVATION", handleFanActivation},
    {"LED_ACTIVATION", handleLedActivation},
    {"LED_ON", handleLedOn},
    {"LED_START", handleLedStart},
    {"LED_END", handleLedEnd}
};

const int SETTINGS_COUNT = sizeof(settings) / sizeof(settings[0]);

void initializeActuators() {
    pinMode(FAN_PIN, OUTPUT);
    pinMode(WATER_PUMP_PIN, OUTPUT);
    pinMode(PH_LOW_DOSING_PUMP_PIN, OUTPUT);
    pinMode(PH_HIGH_DOSING_PUMP_PIN, OUTPUT);
    pinMode(CONDUCTIVITY_LOW_DOSING_PUMP_PIN, OUTPUT);
    pinMode(CONDUCTIVITY_HIGH_DOSING_PUMP_PIN, OUTPUT);
    pinMode(LED_STRIP_PIN, OUTPUT);
    pinMode(WATER_LEVEL_PIN, INPUT);

    digitalWrite(FAN_PIN, LOW);
    digitalWrite(WATER_PUMP_PIN, LOW);
    digitalWrite(PH_LOW_DOSING_PUMP_PIN, LOW);
    digitalWrite(PH_HIGH_DOSING_PUMP_PIN, LOW);
    digitalWrite(CONDUCTIVITY_LOW_DOSING_PUMP_PIN, LOW);
    digitalWrite(CONDUCTIVITY_HIGH_DOSING_PUMP_PIN, LOW);
    digitalWrite(LED_STRIP_PIN, LOW);
}

// 설정값에 따른 조치
void applySettings() {
    float currentTemperature = getCurrentTemperature();
    float currentPh = getCurrentPh();
    float currentConductivity = getCurrentConductivity();

    // 환풍기 제어
    if (currentTemperature < tempMin || currentTemperature > tempMax) {
        digitalWrite(FAN_PIN, HIGH);  // 온도가 범위 밖이면 환풍기 작동
    } else {
        digitalWrite(FAN_PIN, LOW);
    }

    // pH 도징 펌프 제어
    if (currentPh < phMin) {
        digitalWrite(PH_LOW_DOSING_PUMP_PIN, HIGH);  // pH를 올리기 위한 펌프 작동
        digitalWrite(PH_HIGH_DOSING_PUMP_PIN, LOW);
    } else if (currentPh > phMax) {
        digitalWrite(PH_HIGH_DOSING_PUMP_PIN, HIGH);  // pH를 낮추기 위한 펌프 작동
        digitalWrite(PH_LOW_DOSING_PUMP_PIN, LOW);
    } else {
        digitalWrite(PH_LOW_DOSING_PUMP_PIN, LOW);
        digitalWrite(PH_HIGH_DOSING_PUMP_PIN, LOW);
    }

    // 전도도 도징 펌프 제어
    if (currentConductivity < conductivityMin) {
        digitalWrite(CONDUCTIVITY_LOW_DOSING_PUMP_PIN, HIGH);  // 전도도를 올리기 위한 펌프 작동
        digitalWrite(CONDUCTIVITY_HIGH_DOSING_PUMP_PIN, LOW);
    } else if (currentConductivity > conductivityMax) {
        digitalWrite(CONDUCTIVITY_HIGH_DOSING_PUMP_PIN, HIGH);  // 전도도를 낮추기 위한 펌프 작동
        digitalWrite(CONDUCTIVITY_LOW_DOSING_PUMP_PIN, LOW);
    } else {
        digitalWrite(CONDUCTIVITY_LOW_DOSING_PUMP_PIN, LOW);
        digitalWrite(CONDUCTIVITY_HIGH_DOSING_PUMP_PIN, LOW);
    }

    // 수위 센서에 따른 제어
    if (digitalRead(WATER_LEVEL_PIN) == LOW) {
        // 수위가 낮으면 두 개의 도징 펌프 작동
        digitalWrite(CONDUCTIVITY_LOW_DOSING_PUMP_PIN, HIGH);  // 물 펌프
        digitalWrite(CONDUCTIVITY_HIGH_DOSING_PUMP_PIN, HIGH); // 고장액 펌프
    } else {
        // 수위가 높으면 두 펌프를 멈춤
        digitalWrite(CONDUCTIVITY_LOW_DOSING_PUMP_PIN, LOW);
        digitalWrite(CONDUCTIVITY_HIGH_DOSING_PUMP_PIN, LOW);
    }

    // 수위 센서 상태에 따른 도징 펌프 제어
    if (digitalRead(WATER_LEVEL_PIN) == LOW) {  // 수위가 낮을 때
        digitalWrite(CONDUCTIVITY_LOW_DOSING_PUMP_PIN, HIGH);  // 물 공급
        digitalWrite(CONDUCTIVITY_HIGH_DOSING_PUMP_PIN, HIGH);  // 양액 공급
    } else {  // 수위가 충분할 때
        digitalWrite(CONDUCTIVITY_LOW_DOSING_PUMP_PIN, LOW);
        digitalWrite(CONDUCTIVITY_HIGH_DOSING_PUMP_PIN, LOW);
    }
}

void controlWaterPump() {
    unsigned long currentMillis = millis();

    if (pumpActivated) {
        if (currentMillis >= pumpStartTime && currentMillis <= pumpEndTime) {
            if (isPumpOn && (currentMillis - previousMillis >= pumpOnDuration)) {
                digitalWrite(WATER_PUMP_PIN, LOW);
                isPumpOn = false;
                previousMillis = currentMillis;
            } else if (!isPumpOn && (currentMillis - previousMillis >= pumpOffDuration)) {
                digitalWrite(WATER_PUMP_PIN, HIGH);
                isPumpOn = true;
                previousMillis = currentMillis;
            }
        } else {
            digitalWrite(WATER_PUMP_PIN, LOW);
            isPumpOn = false;
        }
    }
}

// 설정값을 시리얼로 수신하여 업데이트하는 로직
void receiveSettings() {
    if (Serial.available()) {
        String data = Serial.readStringUntil('\n');
        
        if (data.startsWith("SET:")) {
            data.replace("SET:", "");
            int paramCount = 0;
            String params[10];

            while (data.length() > 0 && paramCount < 10) {
                int index = data.indexOf(',');
                if (index == -1) {
                    params[paramCount++] = data;
                    break;
                } else {
                    params[paramCount++] = data.substring(0, index);
                    data = data.substring(index + 1);
                }
            }

            for (int i = 0; i < paramCount; i++) {
                int delimiterIndex = params[i].indexOf(':');
                if (delimiterIndex > 0) {
                    String key = params[i].substring(0, delimiterIndex);
                    String value = params[i].substring(delimiterIndex + 1);

                    for (int j = 0; j < SETTINGS_COUNT; j++) {
                        if (key == settings[j].key) {
                            settings[j].handler(value);
                            break;
                        }
                    }
                }
            }

            // 설정값 업데이트 확인
            Serial.println("Settings updated:");
            Serial.print("PUMP_ON: "); Serial.println(pumpOnDuration / 1000);  // 초 단위로 출력
            Serial.print("PUMP_OFF: "); Serial.println(pumpOffDuration / 1000);
            Serial.print("PUMP_ACTIVATION: "); Serial.println(pumpActivated ? "ON" : "OFF");
            Serial.print("FAN_TEMP: "); Serial.println(fanThresholdTemperature);
            Serial.print("FAN_ACTIVATION: "); Serial.println(fanActivated ? "ON" : "OFF");
            Serial.print("LED_ACTIVATION: "); Serial.println(ledActivated ? "ON" : "OFF");
            Serial.print("LED_ON: "); Serial.println(isLedOn ? "ON" : "OFF");
            Serial.print("LED_START: "); Serial.println(ledStartTime / 1000);
            Serial.print("LED_END: "); Serial.println(ledEndTime / 1000);
        }
    }
}
