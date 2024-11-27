#include "ManualControl.h"
#include "Aktuator.h"

// Referer til Aktuator objektet, der er defineret globalt
extern Aktuator aktuator;

extern SensorData sensorData;

void ManuelControl::setupManualControl() {
    pinMode(manualTogglePin, INPUT); // manual open close button
    pinMode(systemTogglePinOn, INPUT);// reading if the System is enabled 
    pinMode(systemTogglePinOff, INPUT);// reading if the System is disabled 
    pinMode(systemLight ,OUTPUT);// setting is as an output to turn on the green light
    pinMode(systemLightOff,OUTPUT);// setting is as an output to turn on the red light

    Serial.println("Manual Control Initialized.");
}
void ManuelControl::handleManualToggle(bool &windowStatus) {
    if (digitalRead(manualTogglePin) == HIGH) { // Switch is toggled ON
        if (windowStatus) {
            Serial.println("Manual toggle: Closing window...");
            aktuator.closeWindow(windowStatus); // Kald til den eksisterende Aktuator-instans
        } else {
            Serial.println("Manual toggle: Opening window...");
            aktuator.openWindow(windowStatus); // Kald til den eksisterende Aktuator-instans
        }

        // Vent for at toggle switchen er frigivet (debounce)
        while (digitalRead(manualTogglePin) == HIGH) {
            delay(50); // Debounce delay
        }
    }
}


bool ManuelControl::handleSystemToggle(){
    
  if(digitalRead(systemTogglePinOn)==HIGH){
    return false;
  }
  return true;
}

// In your Arduino code



void ManuelControl::processThresholdCommand(String input) {
    // Remove the 'T,' prefix
    String commandData = input.substring(2);

    // Split the commandData by commas
    double values[6];
    int index = 0;
    int start = 0;

    while (index < 6 && start < commandData.length()) {
        int commaIndex = commandData.indexOf(',', start);
        String valueStr;

        if (commaIndex != -1) {
            valueStr = commandData.substring(start, commaIndex);
            start = commaIndex + 1;
        } else {
            valueStr = commandData.substring(start);
            start = commandData.length();
        }

        values[index] = valueStr.toFloat();
        index++;
    }

    // Update thresholds if we have all values
    if (index == 6) {
      aktuator.CO2_OPEN_THRESHOLD = values[0];
      aktuator.CO2_CLOSE_THRESHOLD = values[1];
      aktuator.TEMP_OPEN_THRESHOLD = values[2];
      aktuator.TEMP_CLOSE_THRESHOLD = values[3];
      aktuator.HUMIDITY_OPEN_THRESHOLD = values[4];
      aktuator.HUMIDITY_CLOSE_THRESHOLD = values[5];

        Serial.println("Thresholds updated:");
        Serial.print("CO2_OPEN_THRESHOLD: "); Serial.println(aktuator.CO2_OPEN_THRESHOLD);
        Serial.print("CO2_CLOSE_THRESHOLD: "); Serial.println(aktuator.CO2_CLOSE_THRESHOLD);
        Serial.print("TEMP_OPEN_THRESHOLD: "); Serial.println(aktuator.TEMP_OPEN_THRESHOLD);
        Serial.print("TEMP_CLOSE_THRESHOLD: "); Serial.println(aktuator.TEMP_CLOSE_THRESHOLD);
        Serial.print("HUMIDITY_OPEN_THRESHOLD: "); Serial.println(aktuator.HUMIDITY_OPEN_THRESHOLD);
        Serial.print("HUMIDITY_CLOSE_THRESHOLD: "); Serial.println(aktuator.HUMIDITY_CLOSE_THRESHOLD);

        // Send back the updated thresholds immediately
        Serial.print("Thresholds: ");
        Serial.print(aktuator.CO2_OPEN_THRESHOLD); Serial.print(", ");
        Serial.print(aktuator.CO2_CLOSE_THRESHOLD); Serial.print(", ");
        Serial.print(aktuator.TEMP_OPEN_THRESHOLD); Serial.print(", ");
        Serial.print(aktuator.TEMP_CLOSE_THRESHOLD); Serial.print(", ");
        Serial.print(aktuator.HUMIDITY_OPEN_THRESHOLD); Serial.print(", ");
        Serial.println(aktuator.HUMIDITY_CLOSE_THRESHOLD);
    } else {
        Serial.println("Error: Incorrect number of threshold values received.");
    }
}


