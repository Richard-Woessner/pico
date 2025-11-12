#include <Arduino.h>

#define PIN_LED   16
#define BTN_PIN   15

constexpr int RELAY_PIN = 17; // GP17

int ledState = LOW;             // the current state of the LED

void setup()
{
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BTN_PIN, INPUT_PULLUP);
}

void loop()
{
  // Read button state (LOW when pressed due to INPUT_PULLUP)
    // if (digitalRead(BTN_PIN) == LOW) {
    //   digitalWrite(PIN_LED, LOW);   // Turn LED OFF
    //   digitalWrite(RELAY_PIN, HIGH); // Turn Relay ON
    //   Serial.println("Button pressed - LED OFF and RELAY ON");
    // } else {
    //   digitalWrite(PIN_LED, HIGH);    // Turn LED ON
    //   digitalWrite(RELAY_PIN, LOW);  // Turn Relay OFF
    //   Serial.println("Button released - LED ON and RELAY OFF");
    // }
   
    // alternate led
    ledState = (ledState == LOW) ? HIGH : LOW;
    digitalWrite(PIN_LED, ledState);

  delay(1000); // Small delay for button debouncing
}