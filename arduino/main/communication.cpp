#include <Arduino.h>
#include "communication.h"
#include "actuators.h"

void sendDataToRaspberryPi(SensorData data) {
    Serial.print("WATER_LEVEL:");
    Serial.print(data.waterLevel);
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

bool receiveSettingsFromRaspberryPi() {
    if (Serial.available() > 0) {
        String data = Serial.readStringUntil('\n');
        if (data.startsWith("SET:")) {
            // "SET:" 제거
            data.replace("SET:", "");

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

            // 예외 처리: 설정값이 최소한 하나는 있어야 함
            if (paramCount == 0) {
                Serial.println("설정값 수신 오류: 데이터가 비어 있습니다.");
                return false;
            }

            // 파라미터 파싱 및 적용
            for (int i = 0; i < paramCount; i++) {
                int delimiterIndex = params[i].indexOf(':');
                if (delimiterIndex > 0) {
                    String key = params[i].substring(0, delimiterIndex);
                    String value = params[i].substring(delimiterIndex + 1);

                    // 예외 처리: 키와 값이 유효한지 확인
                    if (key.length() == 0 || value.length() == 0) {
                        Serial.println("설정값 수신 오류: 키 또는 값이 유효하지 않습니다.");
                        return false;
                    }

                    // 설정값 적용
                    if (settingHandlers.find(key) != settingHandlers.end()) {
                        settingHandlers[key](value);  // 설정값 적용
                    } else {
                        Serial.print("설정값 수신 오류: 알 수 없는 키 ");
                        Serial.println(key);
                        return false;
                    }
                } else {
                    Serial.println("설정값 수신 오류: 잘못된 형식입니다.");
                    return false;
                }
            }

            // 설정값이 성공적으로 업데이트됨을 확인
            Serial.println("설정값이 성공적으로 업데이트되었습니다.");
            return true;
        }
    }
    return false;
}