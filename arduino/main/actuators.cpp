#include <Arduino.h>
#include "actuators.h"
#include "sensors.h"

// 핀 정의
#define FAN_PIN 6
#define WATER_PUMP_PIN 7
#define PH_DOSING_PUMP_PIN 8
#define CONDUCTIVITY_DOSING_PUMP_PIN 9
#define LED_STRIP_PIN 10

// 사용자 설정값
bool pumpActivated = true;
bool pumpOn = false;
unsigned long pumpOnDuration = 300000;  // 펌프 동작 시간 (밀리초)
unsigned long pumpOffDuration = 10000;  // 펌프 멈춤 시간 (밀리초)
unsigned long pumpStartTime = 0;
unsigned long pumpEndTime = 86400000;  // 기본적으로 하루종일 활성화 (밀리초)

bool fanActivated = true;
float fanThresholdTemperature = 25.0;

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

const int SETTINGS_COUNT = sizeof(settings) / sizeof(settings[0]);  // 배열의 크기를 계산

void initializeActuators() {
    pinMode(FAN_PIN, OUTPUT);
    pinMode(WATER_PUMP_PIN, OUTPUT);
    pinMode(PH_DOSING_PUMP_PIN, OUTPUT);
    pinMode(CONDUCTIVITY_DOSING_PUMP_PIN, OUTPUT);
    pinMode(LED_STRIP_PIN, OUTPUT);

    digitalWrite(FAN_PIN, LOW);
    digitalWrite(WATER_PUMP_PIN, LOW);
    digitalWrite(PH_DOSING_PUMP_PIN, LOW);
    digitalWrite(CONDUCTIVITY_DOSING_PUMP_PIN, LOW);
    digitalWrite(LED_STRIP_PIN, LOW);
}

// 설정값에 따른 조치
void applySettings() {
    float currentTemperature = getCurrentTemperature();
    if (fanActivated && currentTemperature >= fanThresholdTemperature) {
        digitalWrite(FAN_PIN, HIGH);
    } else {
        digitalWrite(FAN_PIN, LOW);
    }

    float currentPh = getCurrentPh();
    if (currentPh < phMin) {
        digitalWrite(PH_DOSING_PUMP_PIN, HIGH);
    } else if (currentPh > phMax) {
        digitalWrite(PH_DOSING_PUMP_PIN, LOW);
    }

    float currentConductivity = getCurrentConductivity();
    if (currentConductivity < conductivityMin) {
        digitalWrite(CONDUCTIVITY_DOSING_PUMP_PIN, HIGH);
    } else if (currentConductivity > conductivityMax) {
        digitalWrite(CONDUCTIVITY_DOSING_PUMP_PIN, LOW);
    }

    unsigned long currentMillis = millis();
    if (ledActivated && currentMillis >= ledStartTime && currentMillis <= ledEndTime) {
        digitalWrite(LED_STRIP_PIN, HIGH);
    } else {
        digitalWrite(LED_STRIP_PIN, LOW);
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

                    for (int j = 0; j < sizeof(settings) / sizeof(settings[0]); j++) {
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
