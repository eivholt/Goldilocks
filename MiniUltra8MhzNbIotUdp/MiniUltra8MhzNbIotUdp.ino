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
int REMOTE_PORT = 1234;
unsigned long INTERVAL_MS = (unsigned long) 15 * 1000;

const int PyroPin = 3;
unsigned long PyroRead = 0;
unsigned long Sensor_PulseWidth = 198000;
unsigned long Sensor_PulseSpace = 420000;
unsigned long IR_lastEdge = 0;
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

void setup() {
  pinMode(PyroPin, INPUT); // IR Sensor connected to 3
  pinMode(LED_BUILTIN, OUTPUT); //LED Connected to Pin 7
  Serial.begin(115200);

  Serial.print("Connecting to NB-IoT module...\n");
  ublox.begin(9600);
  nbiot.begin(ublox);
  nbiot.createSocket();
  Serial.println("NB-IoT module connected.");
} 
 
void loop() {
  
  // Wait 30s to let sensor warm up
  if(millis() > 30*1000)
  {
    //Break after 2 good triggers 
    while (true) 
    { 
      PyroRead = 0; // Reset readings
      IR_lastEdge = 0;
      // Enter power down state with ADC and BOD module disabled.
      // Wake up when wake up pin is low.
      Serial.println("Sleep.");
      Serial.flush();
      //LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
      // Allow wake up pin to trigger interrupt on low.
      attachInterrupt(digitalPinToInterrupt(PyroPin), wakeUp, RISING);
      LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF); 
      // Disable external pin interrupt on wake up pin.
      detachInterrupt(digitalPinToInterrupt(PyroPin)); 
      
      unsigned long PyroReadStart = micros();
      while(digitalRead(PyroPin) == HIGH)
      {
        // Do nothing, wait for falling edge.
      }
      PyroRead = micros() - PyroReadStart;
      IR_lastEdge = micros();

      //Make sure trigger is over 198msec)
      //Serial.print("PyroRead1: "); Serial.println(PyroRead);
      if (PyroRead > Sensor_PulseWidth) 
      { 
        Serial.print("PyroRead1: "); Serial.println(PyroRead);
        //IR_sensed++; //Mark as a good trigger

        PyroRead = 0;
        //Measure trigger point
        PyroRead = pulseIn(PyroPin, HIGH); //, Sensor_PulseSpace + Sensor_PulseWidth/10);
        Serial.print("PyroRead2: "); Serial.println(PyroRead);
        if (PyroRead > Sensor_PulseWidth) 
        {
          Serial.print("PyroRead2: "); Serial.println(PyroRead);
          //IR_sensed++; //Mark as a good trigger
          
          break;
        }

        // while((micros() - IR_lastEdge) < Sensor_PulseSpace)
        // {
        //   PyroRead = 0;
        //   //Measure trigger point
        //   PyroRead = pulseIn(PyroPin, HIGH, Sensor_PulseSpace);
        //   Serial.print("PyroRead2: "); Serial.println(PyroRead);
        //   if (PyroRead > Sensor_PulseWidth/2) 
        //   {
        //     Serial.print("PyroRead2: "); Serial.println(PyroRead);
        //     IR_sensed++; //Mark as a good trigger
            
        //     break;
        //   }
        // }
      }
    }

    PyroRead = 0; // Reset readings
    IR_sensed = 0;

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
      Serial.println("Sending data");
      String payload = 
          String(Detected);
      if (true == nbiot.sendString(remoteIP, REMOTE_PORT, payload)) {
        Serial.print("Successfully sent data: ");
      } else {
        Serial.print("Failed sending data: ");
      }
      Serial.println(payload);
    }
  }
}