#ifndef AKTUATOR_H
#define AKTUATOR_H

#include <Arduino.h>
#include "Sensor.h"

class Aktuator {
public:
    Aktuator();

    void setupMotorPins();
    void openWindow(bool &windowStatus); // på nuværende tidpunkt har vi to forskellige måder vi styrer viduestatus på.
    void closeWindow(bool &windowStatus);
    void stopMotor();


    void handleSerialCommands(); // burde denne også stå her?
    void decideWindowStatus( SensorData &sensorData); //skal den være her eller i en af de andre filer


     float CO2_OPEN_THRESHOLD = 1000.0;    // ppm
     float CO2_CLOSE_THRESHOLD = 800.0;    // ppm
     float TEMP_OPEN_THRESHOLD = 27.0;     // °C
     float TEMP_CLOSE_THRESHOLD = 23.0;    // °C
     float HUMIDITY_OPEN_THRESHOLD = 70.0; // %
     float HUMIDITY_CLOSE_THRESHOLD = 60.0; // %

     bool overrideMode;
     unsigned long overrideTargetTime; 
private:
    int IN1 = 8;
    int IN2 = 9;
    int ENA = 10;
    int closeWindowSwitch = 11;
    int openWindowSwitch = 12;

};

#endif
