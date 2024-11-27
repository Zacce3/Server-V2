#include "Sensor.h"
#include "Aktuator.h"
extern Aktuator aktuator;
extern SensorData sensorData;
extern Sensor sensor;

SensorData::SensorData(){
     windowStatus=false;
}
void Sensor::readAndUpdateSensorData( SensorData &sensorData) {
    float result[3] = {0};

    // Use the default SCD30 object from the library
    if (scd30.isAvailable()) {
        scd30.getCarbonDioxideConcentration(result);
        sensorData.co2 = result[0];
        sensorData.temperature = result[1];
        sensorData.humidity = result[2];
    } else {
        sensorData.co2 = -1;
        sensorData.temperature = -1;
        sensorData.humidity = -1;
    }

}

void Sensor::motionDetected(bool &windowStatus){
  int sensorValue = analogRead(distanceSensorPin);
    float voltage = sensorValue * (5.0 / 1023.0);
    if (voltage >= 0.4 && voltage <= 2.8) {
        distance = 30 - (voltage - 0.4) * (30 - 4) / (2.8 - 0.4);
    } else if (voltage > 2.8) {
        distance = 4;
    } else {
        distance = 30;
    }

    windowStatus = (distance > 10);
}


void Sensor::printSensorData(const SensorData &sensorData) {
    Serial.print("CO2: ");
    Serial.print(sensorData.co2);
    Serial.print(" ppm, ");
    Serial.print("Temperature: ");
    Serial.print(sensorData.temperature);
    Serial.print(" °C, ");
    Serial.print("Humidity: ");
    Serial.print(sensorData.humidity);
    Serial.print(" %, ");
    Serial.print("Distance: ");
    Serial.print(sensor.distance);
    Serial.print(" cm, ");
    Serial.print("Window: ");
    Serial.println(sensorData.windowStatus ? "Open" : "Closed");

    // Send current threshold values
    Serial.print("Thresholds: ");
    Serial.print(aktuator.CO2_OPEN_THRESHOLD); Serial.print(", ");
    Serial.print(aktuator.CO2_CLOSE_THRESHOLD); Serial.print(", ");
    Serial.print(aktuator.TEMP_OPEN_THRESHOLD); Serial.print(", ");
    Serial.print(aktuator.TEMP_CLOSE_THRESHOLD); Serial.print(", ");
    Serial.print(aktuator.HUMIDITY_OPEN_THRESHOLD); Serial.print(", ");
    Serial.println(aktuator.HUMIDITY_CLOSE_THRESHOLD);
}
