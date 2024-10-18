#include <Arduino.h>
#include "communication.h"
#include "actuators.h"

// "DEVICE_ID:"의 문자열 길이를 상수로 정의
constexpr int DEVICE_ID_PREFIX_LENGTH = sizeof("DEVICE_ID:") - 1;  // 문자열 길이 계산
const char* deviceId = "3YJ4nOhl8WUwbmm7LPlG";

void sendDataToRaspberryPi(SensorData data) {
    // deviceId 전송
    Serial.print("DEVICE_ID:");
    Serial.print(deviceId);
    
    // 센서 데이터 전송
    Serial.print(",WATER_LEVEL:");
    Serial.print(data.waterLevel ? "1" : "0");  // 1 (HIGH), 0 (LOW)로 변환하여 전송
    Serial.print(",PH:");
    Serial.print(data.phValue);
    Serial.print(",OUTER_TEMP:");
    Serial.print(data.outerTemp);
    Serial.print(",OUTER_HUMIDITY:");
    Serial.print(data.outerHumidity);
    Serial.print(",NUTRIENT_TEMP:");
    Serial.print(data.nutrientTemp);
    Serial.print(",CONDUCTIVITY:");
    Serial.println(data.conductivity);
}

// Client 설정값 통신
bool receiveSettingsFromRaspberryPi() {
    if (Serial.available() > 0) {
        String data = Serial.readStringUntil('\n');
        
        if (data.startsWith("SET:")) {
            data.replace("SET:", "");
            
            // DEVICE_ID 추출
            int deviceIdIndex = data.indexOf("DEVICE_ID:");
            int deviceIdEndIndex = data.indexOf(',', deviceIdIndex);
            String receivedDeviceId = data.substring(deviceIdIndex + DEVICE_ID_PREFIX_LENGTH, deviceIdEndIndex);

            // DEVICE_ID와 미리 정의된 deviceId 비교
            if (receivedDeviceId != deviceId) {
                // deviceId가 일치하지 않으면 설정값 업데이트 중단
                Serial.println("Device ID does not match. Ignoring settings.");
                return false;
            }

            // deviceId를 제거한 나머지 데이터
            data = data.substring(deviceIdEndIndex + 1);

            int paramCount = 0;
            String params[10];

            // 콤마로 분리하여 파라미터를 배열로 저장
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

            // 파라미터 처리 로직
            for (int i = 0; i < paramCount; i++) {
                int delimiterIndex = params[i].indexOf(':');
                if (delimiterIndex > 0) {
                    String key = params[i].substring(0, delimiterIndex);
                    String value = params[i].substring(delimiterIndex + 1);

                    // 설정값을 처리
                    for (int j = 0; j < SETTINGS_COUNT; j++) {
                        if (key == settings[j].key) {
                            settings[j].handler(value);
                            break;
                        }
                    }
                }
            }

            Serial.print("Settings updated for device: ");
            Serial.println(receivedDeviceId);
            return true;
        }
    }
    return false;
}
