#include "Sensor.h"
#include "Aktuator.h"
#include "ManualControl.h"
#include "SCD30.h"


Sensor sensor;
SensorData sensorData;
Aktuator aktuator;
ManuelControl manuelControl;

void setup() {
    Wire.begin();               // Initialize I2C communication
    Serial.begin(9600);         // Initialize Serial communication

    aktuator.setupMotorPins();           // Setup motor pins and microswitches
    manuelControl.setupManualControl();       // Initialize manual control logic

    Serial.println("SCD30 Raw Data");
    scd30.initialize();         // Initialize SCD30 sensor
    Serial.println("SCD30 initialized.");
    Serial.println("System Ready. Type 'O' for open, 'C' for close, or toggle switch to control manually.");
   
}

void loop() {
   static unsigned long lastUpdateTime = 0;  // For timing control
    const unsigned long updateInterval = 2000;  // Update interval in milliseconds
    // Handle system toggle
    if(manuelControl.handleSystemToggle()==true){
        manuelControl.handleManualToggle(sensorData.windowStatus);
        digitalWrite(manuelControl.systemLight,HIGH);
        digitalWrite(manuelControl.systemLightOff,LOW);
      
        

    } else {
        digitalWrite(manuelControl.systemLight,LOW);
        digitalWrite(manuelControl.systemLightOff,HIGH);

        // Handle manual toggle
        manuelControl.handleManualToggle(sensorData.windowStatus);
       
        

        // Check override mode
        if (aktuator.overrideMode) {
            // Check if target time reached
           
            if (millis() >= aktuator.overrideTargetTime) {
                // Close the window
               
                if (sensorData.windowStatus) {
                    aktuator.closeWindow(sensorData.windowStatus); 
                    Serial.println("Window closed due to timer override.");
                  
                }
                // Keep overrideMode active to prevent automatic control
            }
            // Bypass automatic control
        } else {
          aktuator.decideWindowStatus(sensorData); 
          delay(30);
        
            // Automatic control logic
          
        

           
        }
    }
     if (millis() - lastUpdateTime >= updateInterval) {
                lastUpdateTime = millis();

                // Read and update sensor data
                sensor.readAndUpdateSensorData(sensorData);
                sensor.motionDetected(sensorData.windowStatus);

                // Print sensor data to Serial
                sensor.printSensorData(sensorData);

                // Automatically decide window state
                
            }


    aktuator.handleSerialCommands();; // Handle serial commands
}
