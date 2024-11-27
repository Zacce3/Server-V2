#ifndef MANUALCONTROL_H
#define MANUALCONTROL_H
#include "Aktuator.h"
#include "Sensor.h"

#include <Arduino.h>

class ManuelControl {
public:
    void setupManualControl();
    void handleManualToggle(bool &windowStatus);
    bool handleSystemToggle();

//Måske lave en helt ny class til disse
    void handleThresholdUpdate();
    void processThresholdCommand(String input);

    int systemLight = 5; //Indicated with a green light
    int systemLightOff = 6; //Indicated with a red light
    int manualTogglePin = 2; // Pin for the manual toggle switch
    int systemTogglePinOn = 3; //Toggle the system on
    int systemTogglePinOff = 4; //Toggle the system off
private:
};

#endif
