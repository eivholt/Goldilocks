#include <LowPower.h>

const int PyroPin = 3;

void wakeUp()
{
    // Just a handler for the pin interrupt.
    // Serial.println("wakeUp");
    // Serial.flush();
    
}

void setup()
{
    pinMode(PyroPin, INPUT);
	Serial.begin(115200);
    Serial.println("Setup complete.");
    Serial.flush();
}

void loop()
{
    // Serial.println("Sleep.");
    // Serial.flush();
	// LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
    // Allow wake up pin to trigger interrupt on low.
    attachInterrupt(digitalPinToInterrupt(PyroPin), wakeUp, FALLING);
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF); 
    // Disable external pin interrupt on wake up pin.
    detachInterrupt(digitalPinToInterrupt(PyroPin)); 
}
