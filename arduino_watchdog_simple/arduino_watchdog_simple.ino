/*
* Demonstration of the use of a button
* This version uses polling
* Added: simple hardware watchdog
*/

#include <avr/wdt.h>   // library for watchdog
#include <avr/io.h>

const int pinLed = 12;    // digital pin for LED
const int pinButton = 2;  // digital pin for button

void setup()
{
  // Optional: Check if last reset was caused by watchdog
  if (MCUSR & (1 << WDRF)) {
    MCUSR = 0;  // clear reset flags
    
    // Blink LED quickly to indicate watchdog reset
    pinMode(pinLed, OUTPUT);
    for (int i = 0; i < 3; i++) {
      digitalWrite(pinLed, HIGH);
      delay(200);
      digitalWrite(pinLed, LOW);
      delay(200);
    }
  }

  // Enable watchdog with 2 second timeout
  wdt_enable(WDTO_2S);

  // LED pin is output
  pinMode(pinLed, OUTPUT);
  
  // Button pin is input
  pinMode(pinButton, INPUT);
}

/*
* Main loop
* If the program hangs and does not reset the watchdog
* within 2 seconds, the Arduino will automatically reboot.
*/
void loop()
{
  // Reset ("kick") the watchdog
  wdt_reset();

  // Read button state
  int iButtonState = digitalRead(pinButton);
  
  // Control LED
  if (iButtonState)
      digitalWrite(pinLed, HIGH);
  else
      digitalWrite(pinLed, LOW);

  // Small delay for stability
  delay(10);

  // -----
  // If you uncomment the next line,
  // the watchdog will reset the board:
  // while(1);   // simulate software crash
}
