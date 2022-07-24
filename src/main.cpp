#include <Arduino.h>

const int analogInPin = A0;  // Analog input pin that the potentiometer is attached to
const int pumpPin = 12;

const long unsigned int onTime = 15000;
const long unsigned int offTime = 120000;

int sensorValue = 0;        // value read from the pot
int outputValue = 0;        // value output to the PWM (analog out)
bool pump = true;
long unsigned int timeLastUpdated= 0;

void setup() {
   // Configure Timer 1 for PWM @ 25 kHz.
    TCCR1A = 0;           // undo the configuration done by...
    TCCR1B = 0;           // ...the Arduino core library
    TCNT1  = 0;           // reset timer
    TCCR1A = _BV(COM1A1)  // non-inverted PWM on ch. A
           | _BV(COM1B1)  // same on ch; B
           | _BV(WGM11);  // mode 10: ph. correct PWM, TOP = ICR1
    TCCR1B = _BV(WGM13)   // ditto
           | _BV(CS10);   // prescaler = 1
    ICR1   = 320;         // TOP = 320

    // Set the PWM pins as output.
    pinMode( 9, OUTPUT);
    pinMode(10, OUTPUT);
    pinMode(11, OUTPUT);
    pinMode(12, OUTPUT);

    digitalWrite(12, HIGH);
    digitalWrite(pumpPin, HIGH);
}

// PWM output @ 25 kHz, only on pins 9 and 10.
// Output value should be between 0 and 320, inclusive.
void analogWrite25k(int pin, int value)
{
    switch (pin) {
        case 9:
            OCR1A = value;
            break;
        case 10:
            OCR1B = value;
            break;
        default:
            // no other pin will work
            break;
    }
}

void loop() {

  long unsigned int currentTime = millis();
  long unsigned int duration = currentTime - timeLastUpdated;
  if (pump) {
    if (duration > onTime) {
      pump = false;
      digitalWrite(pumpPin, LOW);
      timeLastUpdated = currentTime;
    }
  } else {
    if (duration > offTime) {
      pump = true;
      digitalWrite(pumpPin, HIGH);
      timeLastUpdated = currentTime;
    }
  }
  
    // read the analog in value:
    sensorValue = analogRead(analogInPin);

    // map it to the range of the analog out:
    outputValue = map(sensorValue, 0, 1023, 0, 320);

    analogWrite25k(9, outputValue);
}