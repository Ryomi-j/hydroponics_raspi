#include <Arduino.h>
#include "actuators.h"
#include "sensors.h"
#include <map>

// 핀 정의
#define FAN_PIN 6
#define WATER_PUMP_PIN 7
#define PH_DOSING_PUMP_PIN 8
#define CONDUCTIVITY_DOSING_PUMP_PIN 9
#define LED_STRIP_PIN 10

bool isFanConnected = true;
bool isWaterPumpConnected = true;
bool isPhPumpConnected = true;
bool isConductivityPumpConnected = true;
bool isLedStripConnected = true;

// 사용자 설정값
bool pumpActivated = true;
bool pumpOn = false;
unsigned long pumpOnDuration = 10000;  // 펌프 동작 시간 (밀리초)
unsigned long pumpOffDuration = 300000;  // 펌프 멈춤 시간 (밀리초)
unsigned long pumpStartTime = 0;
unsigned long pumpEndTime = 86400000;  // 기본적으로 하루종일 활성화 (밀리초)

bool fanActivated = true;
// 연결 상태 플래그
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

// 함수 포인터 타입 정의
typedef void (*SettingHandler)(String);

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

// 설정값 처리 함수들을 저장하는 맵
std::map<String, SettingHandler> settingHandlers = {
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

void initializeActuators() {
    // 핀 모드 설정
    pinMode(FAN_PIN, OUTPUT);
    pinMode(WATER_PUMP_PIN, OUTPUT);
    pinMode(PH_DOSING_PUMP_PIN, OUTPUT);
    pinMode(CONDUCTIVITY_DOSING_PUMP_PIN, OUTPUT);
    pinMode(LED_STRIP_PIN, OUTPUT);

    // 초기 상태 설정
    digitalWrite(FAN_PIN, LOW);
    digitalWrite(WATER_PUMP_PIN, LOW);
    digitalWrite(PH_DOSING_PUMP_PIN, LOW);
    digitalWrite(CONDUCTIVITY_DOSING_PUMP_PIN, LOW);
    digitalWrite(LED_STRIP_PIN, LOW);

    // 예외 처리: 장치 연결 여부 확인
    if (!isFanConnected) Serial.println("경고: 환풍기가 연결되지 않았습니다.");
    if (!isWaterPumpConnected) Serial.println("경고: Water Pump가 연결되지 않았습니다.");
    if (!isPhPumpConnected) Serial.println("경고: pH 도징 펌프가 연결되지 않았습니다.");
    if (!isConductivityPumpConnected) Serial.println("경고: Conductivity 도징 펌프가 연결되지 않았습니다.");
    if (!isLedStripConnected) Serial.println("경고: LED 스트립이 연결되지 않았습니다.");
}

void applySettings() {
    // 환풍기 제어
    float currentTemperature = getCurrentTemperature();  // 온도 센서 값 읽기
    if (fanActivated && currentTemperature >= fanThresholdTemperature && isFanConnected) {
        digitalWrite(FAN_PIN, HIGH);  // 환풍기 켜기
    } else {
        digitalWrite(FAN_PIN, LOW);  // 환풍기 끄기
    }

    // pH 및 전도도 제어
    float currentPh = getCurrentPh();  // pH 센서 값 읽기
    if (isPhPumpConnected) {
        if (currentPh < phMin) {
            digitalWrite(PH_DOSING_PUMP_PIN, HIGH);  // pH를 올리기 위한 도징 펌프 작동
        } else if (currentPh > phMax) {
            digitalWrite(PH_DOSING_PUMP_PIN, LOW);  // pH를 낮추기 위한 도징 펌프 작동
        }
    }

    float currentConductivity = getCurrentConductivity();  // 전도도 센서 값 읽기
    if (isConductivityPumpConnected) {
        if (currentConductivity < conductivityMin) {
            digitalWrite(CONDUCTIVITY_DOSING_PUMP_PIN, HIGH);  // 전도도 올리기 위한 도징 펌프 작동
        } else if (currentConductivity > conductivityMax) {
            digitalWrite(CONDUCTIVITY_DOSING_PUMP_PIN, LOW);  // 전도도 낮추기 위한 도징 펌프 작동
        }
    }

    // LED 제어 (스케줄에 따라)
    unsigned long currentMillis = millis();
    if (ledActivated && isLedStripConnected && currentMillis >= ledStartTime && currentMillis <= ledEndTime) {
        digitalWrite(LED_STRIP_PIN, HIGH);
    } else {
        digitalWrite(LED_STRIP_PIN, LOW);
    }
}

void controlWaterPump() {
    unsigned long currentMillis = millis();

    if (isWaterPumpConnected && pumpActivated) {
        // 펌프가 설정된 시간대에만 동작하도록 제어
        if (currentMillis >= pumpStartTime && currentMillis <= pumpEndTime) {
            if (isPumpOn && (currentMillis - previousMillis >= pumpOnDuration)) {
                // 펌프를 OFF로 전환
                digitalWrite(WATER_PUMP_PIN, LOW);
                isPumpOn = false;
                previousMillis = currentMillis;
            } else if (!isPumpOn && (currentMillis - previousMillis >= pumpOffDuration)) {
                // 펌프를 ON으로 전환
                digitalWrite(WATER_PUMP_PIN, HIGH);
                isPumpOn = true;
                previousMillis = currentMillis;
            }
        } else {
            // 펌프가 시간대 외에 있을 때는 항상 OFF
            digitalWrite(WATER_PUMP_PIN, LOW);
            isPumpOn = false;
        }
    } else {
        Serial.println("워터 펌프가 연결되지 않거나 비활성화 상태입니다.");
    }
}

// 설정값을 시리얼로 수신하여 업데이트하는 로직
void receiveSettings() {
    if (Serial.available()) {
        String data = Serial.readStringUntil('\n');
        if (data.startsWith("SET:")) {
            // 데이터를 파싱하여 설정값을 추출
            data.replace("SET:", "");  // "SET:" 제거

            // 설정값을 콤마로 구분하여 배열로 변환
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

            // 파라미터 파싱 및 적용
            for (int i = 0; i < paramCount; i++) {
                int delimiterIndex = params[i].indexOf(':');
                if (delimiterIndex > 0) {
                    String key = params[i].substring(0, delimiterIndex);
                    String value = params[i].substring(delimiterIndex + 1);

                    // 해당 키가 맵에 있으면 처리
                    if (settingHandlers.find(key) != settingHandlers.end()) {
                        settingHandlers[key](value);  // 설정값 적용
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
