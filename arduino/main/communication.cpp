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

                    for (int j = 0; j < SETTINGS_COUNT; j++) {  // 배열 크기를 SETTINGS_COUNT로 참조
                        if (key == settings[j].key) {
                            settings[j].handler(value);
                            break;
                        }
                    }
                }
            }

            Serial.println("Settings updated.");
            return true;
        }
    }
    return false;
}
