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
unsigned long IR_threshold = 198000; 
// Note: SS-430 has two pulses of 200msec per detection.
// IR_threshold is in microsec (usec), therefore 198msec threshold 

int Detected = LOW;
int IR_sensed = 0;

void setup() {
  pinMode(PyroPin, INPUT);
  pinMode (LED_BUILTIN, OUTPUT); //LED Connected to Pin 7
  pinMode (PyroPin,INPUT); // IR Sensor connected to A1
  Serial.begin(115200);

  Serial.print("Connecting to NB-IoT module...\n");
  ublox.begin(9600);
  nbiot.begin(ublox);
  nbiot.createSocket();
} 
 
void loop() {
  if (nbiot.isConnected()) {
    // Successfully connected to the network
    // Send message to remote server

    String payload = 
        String(Detected);
    if (true == nbiot.sendString(remoteIP, REMOTE_PORT, payload)) {
      Serial.print("Successfully sent data: ");
    } else {
      Serial.print("Failed sending data: ");
    }
    Serial.println(payload);
  }
  
  //Break after 2 good triggers 
  while ((IR_sensed < 2)) 
  { 
    PyroRead = pulseIn(PyroPin, HIGH); //Measure trigger point

    if (PyroRead > IR_threshold) { //Make sure trigger is over 198msec)
      Serial.print("PyroRead: "); Serial.println(PyroRead);
      IR_sensed++; //Mark as a good trigger
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
  PyroRead = 0; // Reset readings
  IR_sensed = 0;
  
  delay(INTERVAL_MS); // Accept triggers after a second
}