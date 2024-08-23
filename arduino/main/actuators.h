#ifndef ACTUATORS_H
#define ACTUATORS_H

struct Setting {
    const char* key;
    void (*handler)(String);
};

extern Setting settings[];  // settings 배열 선언
extern const int SETTINGS_COUNT;  // 배열의 크기를 선언

void initializeActuators();
void applySettings();
void controlWaterPump();

#endif
