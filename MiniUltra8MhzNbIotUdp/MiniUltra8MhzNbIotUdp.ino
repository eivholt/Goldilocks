#include <TelenorNBIoT.h>
#include <LowPower.h>

#ifdef SERIAL_PORT_HARDWARE_OPEN
/*
 * For Arduino boards with a hardware serial port separate from USB serial.
 * This is usually mapped to Serial1. Check which pins are used for Serial1 on
 * the board you're using.
 */
#define ublox SERIAL_PORT_HARDWARE_OPEN
#else
/*
 * For Arduino boards with only one hardware serial port (like Arduino UNO). It
 * is mapped to USB, so we use SoftwareSerial on pin 10 and 11 instead.
 */
#include <SoftwareSerial.h>
SoftwareSerial ublox(10, 11);
#endif

TelenorNBIoT nbiot("mda.ee", 242, 01);
// The remote IP address to send data packets to
// u-blox SARA N2 does not support DNS
IPAddress remoteIP(172, 16, 15, 14);
unsigned int REMOTE_PORT = 1234;
unsigned long INTERVAL_MS = (unsigned long) 15 * 1000;

const int PyroPin = 3;
unsigned long PyroRead = 0;
unsigned long Sensor_PulseWidth = 198000;
unsigned long Sensor_PulseSpace = 420000;
unsigned long IR_lastEdge = 0;
unsigned int Sensor_Warmup_MS = 30*1000;
// Note: SS-430 has two pulses of 200msec per detection.
// IR_threshold is in microsec (usec), therefore 198msec threshold 

int Detected = LOW;
int IR_sensed = 0;

void wakeUp()
{
    // Just a handler for the pin interrupt.
    // Serial.println("wakeUp");
    // Serial.flush();
}

void powerDownInterrupt()
{
  // Enter power down state with ADC and BOD module disabled.
  // Wake up when wake up pin is rising.
  // Allow wake up pin to trigger interrupt on low.
  attachInterrupt(digitalPinToInterrupt(PyroPin), wakeUp, RISING);
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF); 
  // Disable external pin interrupt on wake up pin.
  detachInterrupt(digitalPinToInterrupt(PyroPin)); 
}

void setup() 
{
  pinMode(PyroPin, INPUT); // IR Sensor connected to 3
  pinMode(LED_BUILTIN, OUTPUT); //LED Connected to Pin 7
  Serial.begin(115200);

  Serial.print(F("Connecting to NB-IoT module...\n"));
  ublox.begin(9600);
  nbiot.begin(ublox);
  nbiot.createSocket();
  Serial.println(F("NB-IoT module connected."));
  Serial.flush();
} 
 
void loop() 
{
  // Wait 30s to let sensor warm up
  if(millis() > Sensor_Warmup_MS)
  {
    //Break after 2 close pulses 
    while (true) 
    { 
      PyroRead = 0; // Reset readings
      IR_lastEdge = 0;
      
      Serial.println(F("Sleeping, ready for input."));
      Serial.flush();
      powerDownInterrupt();
      unsigned long PyroReadStart = micros();
      while(digitalRead(PyroPin) == HIGH)
      {
        // Do nothing, wait for falling edge.
      }

      PyroRead = micros() - PyroReadStart;
      IR_lastEdge = micros();

      //Make sure trigger is over 198msec)
      if (PyroRead > Sensor_PulseWidth) 
      { 
        Serial.print(F("PyroRead1: ")); Serial.println(PyroRead);
        PyroRead = 0;
        //Measure trigger point
        PyroRead = pulseIn(PyroPin, HIGH); //, Sensor_PulseSpace + Sensor_PulseWidth/10);
        if (PyroRead > Sensor_PulseWidth) 
        {
          Serial.print(F("PyroRead2: ")); Serial.println(PyroRead);          
          break;
        }
      }
    }

    // Turn LED OFF if it was previous ON
    if (Detected == HIGH) 
    { 
      Detected = LOW;
      //digitalWrite(LED_BUILTIN, LOW);
    } else {
      Detected = HIGH; // Turn LED ON if it was previous OFF
      //digitalWrite(LED_BUILTIN, HIGH);
    }

    if (nbiot.isConnected()) {
      // Successfully connected to the network
      // Send message to remote server
      Serial.println(F("Sending data"));
      String payload = 
          String(Detected);
      if (true == nbiot.sendString(remoteIP, REMOTE_PORT, payload)) {
        Serial.print(F("Successfully sent data: "));
      } else {
        Serial.print(F("Failed sending data: "));
      }
      Serial.println(payload);
    }
  }
}