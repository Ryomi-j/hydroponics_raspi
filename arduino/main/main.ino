#include "sensors.h"
#include "actuators.h"
#include "communication.h"

void setup() {
    Serial.begin(9600);
    initializeSensors();
    initializeActuators();
}

void loop() {
    unsigned long currentMillis = millis();

    // 1분마다 센서 데이터 전송 및 설정값 적용
    if (currentMillis - previousMillis >= 60000) {
        previousMillis = currentMillis;

        // 센서 데이터 수집
        SensorData data = readSensors();

        // 라즈베리파이로 데이터 전송
        sendDataToRaspberryPi(data);

        // 라즈베리파이로부터 설정값 수신 및 제어
        if (receiveSettingsFromRaspberryPi()) {
            applySettings();
        }

        // 워터 펌프 제어
        controlWaterPump();
    }
}
