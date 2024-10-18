#ifndef ACTUATORS_H
#define ACTUATORS_H

struct Setting {
    const char* key;
    void (*handler)(String);
};

extern Setting settings[];  // settings 배열 선언
extern const int SETTINGS_COUNT;  // 배열의 크기를 선언
extern unsigned long previousMillis; // 이전 시간을 저장하는 변수 선언

void initializeActuators();
void applySettings();
void controlWaterPump();

#endif
