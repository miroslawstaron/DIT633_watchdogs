/*
 * Button-controlled Watchdog example
 * Watchdog is kicked ONLY when button is pressed.
 * If no press for 3 seconds -> system resets.
 */

#include <avr/wdt.h>
#include <avr/io.h>

const int pinLed = 12;
const int pinButton = 2;

void setup()
{
  Serial.begin(9600);
  delay(500);   // allow serial to initialize

  // Check if reset was caused by watchdog
  if (MCUSR & (1 << WDRF)) {
    Serial.println("Watchdog timeout - restarting system");
    MCUSR = 0;  // clear reset flags
  } else {
    Serial.println("Normal startup");
  }

  pinMode(pinLed, OUTPUT);
  pinMode(pinButton, INPUT);

  // Enable watchdog with 4 second timeout
  // (closest available value above 3 seconds)
  wdt_enable(WDTO_4S);
}

/*
 * Watchdog is kicked only when button is pressed.
 * If no press occurs, watchdog times out.
 */
void loop()
{
  int buttonState = digitalRead(pinButton);

  if (buttonState)
  {
    // Turn LED on
    digitalWrite(pinLed, HIGH);

    // Kick watchdog
    wdt_reset();
    Serial.println("Watchdog kicked");

    delay(200);   // small debounce delay
  }
  else
  {
    digitalWrite(pinLed, LOW);
    // No watchdog reset here!
  }
}
