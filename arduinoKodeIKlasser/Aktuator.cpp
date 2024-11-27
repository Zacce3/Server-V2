#include "Aktuator.h"
#include "ManualControl.h"
#include "Sensor.h"

extern ManuelControl manuelControl;
extern Sensor sensor;
extern SensorData sensorData;

Aktuator::Aktuator() {
     overrideMode = false;
     overrideTargetTime = 0; 
}

void Aktuator::setupMotorPins() {
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(ENA, OUTPUT);
    pinMode(openWindowSwitch, INPUT_PULLUP);
    pinMode(closeWindowSwitch, INPUT_PULLUP);
}

void Aktuator::openWindow(bool &windowStatus) {
    Serial.println("Opening window...");
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(ENA, 255); // Full speed

    while (digitalRead(openWindowSwitch) == LOW) {
        //manuelControl.handleThresholdUpdate();       // Check for threshold update commands from ManualControl
        //andleWindowCommands();
         handleSerialCommands();        // Check for window commands
        manuelControl.handleManualToggle(windowStatus); // Check for manual toggle from ManualControl // er dette nødvendigt
        delay(10); // Small delay to prevent CPU overuse
    }

    stopMotor();
    windowStatus = true;
    Serial.println("Window fully opened.");
}

void Aktuator::closeWindow(bool &windowStatus) {
    Serial.println("Closing window...");
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, 255); // Full speed

    while (digitalRead(closeWindowSwitch) == LOW) {
        //manuelControl.handleThresholdUpdate();       // Check for threshold update commands from ManualControl
        //handleWindowCommands(); //er dette også nødvendigt
        handleSerialCommands();        // Check for window commands
        manuelControl.handleManualToggle(windowStatus); // Check for manual toggle from ManualControl. // er dette nødvendigt
        delay(10); // Small delay to prevent CPU overuse
    }

    stopMotor();
    windowStatus = false;
    Serial.println("Window fully closed.");
}


void Aktuator::handleSerialCommands() {
    static String inputString = ""; // A String to hold incoming data
    while (Serial.available() > 0) {
        char inChar = (char)Serial.read();  // Read a character
        inputString += inChar;              // Append to inputString

        if (inChar == '\n') {               // If the incoming character is a newline
            inputString.trim();             // Remove leading and trailing whitespace

            if (inputString.startsWith("T,")) {
                manuelControl.processThresholdCommand(inputString);
            } else if (inputString.equalsIgnoreCase("O")) {
                openWindow(sensorData.windowStatus);
            } else if (inputString.equalsIgnoreCase("C")) {
                closeWindow(sensorData.windowStatus);
            } else if (inputString.startsWith("Timer,")) {
                // Process timer command
                String durationStr = inputString.substring(6); // Get the duration part
                int durationSeconds = durationStr.toInt();     // Convert to integer
                if (durationSeconds > 0) {
                    overrideMode = true;
                    overrideTargetTime = millis() + durationSeconds * 1000UL;
                    Serial.print("Timer set: closing window in ");
                    Serial.print(durationSeconds);
                    Serial.println(" seconds.");
                } else {
                    Serial.println("Invalid timer duration.");
                }
            } else if (inputString.equalsIgnoreCase("CancelOverride")) {
                overrideMode = false;
                Serial.println("Override cancelled. Resuming automatic control.");
            } else {
                Serial.println("Invalid command.");
            }
            inputString = ""; // Clear the string for the next command
        }
    }
}



void Aktuator::stopMotor() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, 0);  // Stop motor
    Serial.println("Motor stopped.");
}

void Aktuator::handleWindowCommands() {
    if (Serial.available() > 0) {
        char command = Serial.read();
        if (command == 'O' || command == 'o') {
            openWindow(sensorData.windowStatus);
        } else if (command == 'C' || command == 'c') {
            closeWindow(sensorData.windowStatus);
        } else {
            Serial.println("Invalid command. Use 'O' for open, 'C' for close.");
        }
    }
}

void Aktuator::decideWindowStatus( SensorData &sensorData) {
    // Validate sensor data
    if (sensorData.co2 == -1 || sensorData.temperature == -1 || sensorData.humidity == -1) {
        Serial.println("Invalid sensor readings. No action will be taken.");
        return;
    }

    // Open window conditions
    bool shouldOpen = 
        (sensorData.co2 > CO2_OPEN_THRESHOLD) ||
        (sensorData.temperature > TEMP_OPEN_THRESHOLD) ||
        (sensorData.humidity > HUMIDITY_OPEN_THRESHOLD);

    // Close window conditions
    bool shouldClose = 
        (sensorData.co2 < CO2_CLOSE_THRESHOLD) &&
        (sensorData.temperature < TEMP_CLOSE_THRESHOLD) &&
        (sensorData.humidity < HUMIDITY_CLOSE_THRESHOLD);

    // Perform the action based on conditions and current state
    if (shouldOpen && !sensorData.windowStatus) {
        openWindow(sensorData.windowStatus);
    } else if (shouldClose && sensorData.windowStatus) {
        closeWindow(sensorData.windowStatus);
    } else {
        Serial.println("Action: No change to window state.");
    }
}
