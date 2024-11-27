  #ifndef SENSOR_H
  #define SENSOR_H

  #include <Arduino.h>
  #include "SCD30.h"

  class SensorData{
    public:
    SensorData();
      float co2;
      float temperature;
      float humidity;
      bool windowStatus;
  };

  class Sensor {
  public:
      void readAndUpdateSensorData(SensorData &sensorData); //fungere som vores reqData()
      void motionDetected(bool &windowStatus); //man burde endda update den her så den sender et bool, ud fra denne skal man opdatere vinduesstatus. 
      void printSensorData(const SensorData &sensorData); //kan fjernes
      float distance;
  private:
      const int distanceSensorPin = A0; // Pin for the distance sensor
      

  };

  #endif
