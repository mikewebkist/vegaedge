
/* VEGA 4.0 code

   ATtiny85 @ 1 MHz (internal oscillator; BOD disabled)

notes:
- Works with the arduino-tiny firmware <https://code.google.com/p/arduino-tiny/> but _not_ with attiny firmware specified by the hi-lo tech group.
- The fade pattern is determined by a 256-long array of values for each of the LED brightnesses.
- Assumed a gamma correction curve for the LEDs of ^2. That shaping is not implemented in the arduino code, we've done it in the stage before the arduino code.
- Behaviour: double-click to turn on, single-click to cycle between solid, flashing, off (asleep).
- The chip never goes off, it goes asleep. We've done almost everything we can to assure that power consumption while asleep is as low as possible.
- for more information see http://www.vegalite.com/nerds .

TO DO
- More beautiful startup flashes.
- lower clock speed to 128 kHz ultimately to save even more power, tho makes chip unprogrammable
- low-battery detection
- easter egg!

- versioning querying. Either softserial out, or some kind of light pulsing on startup, or with a specific button push. Easter egg!
- software serial does not function at 1mHz. Even worth implementing? How else do we query version? Writing on board?


We'd love your help making the code better! Send comments, snippets, anything to hello@angellamackey.com .

http://www.vegalite.com/ , 2014
Angella Mackey, David NG McCallum, Johannes Omberg, and other smart people.
 */

// *** SLEEP CODE

#include <avr/sleep.h>
#include <avr/wdt.h>
#include <Adafruit_NeoPixel.h>

// Hardware parameters //

#define PIN 12
#define button A5
#define FET 1

unsigned long shutdownTimer;

const byte ledPin0 = 0;
const byte ledPin2 = 1;
const byte ledPin1 = 4;
const byte voltPin = 3;
const byte buttonPin = 2;


const int doubleClickThresh = 500;    // time between double-clicks, otherwise goes to sleep

// When all lights solid, actually dimmed to reduce strain on the battery.
//const byte solidBrightness = 32;       // no resistors
//const byte solidBrightness = 64;
//const byte solidBrightness = 128;     // now using gamma array. value of 128 corresponds to PWM of 64
const byte solidBrightness = 192;
//const byte solidBrightness = 255;

const byte fashionBrightness = 96;
const byte safetyBrightness = 192;

// Flashing timing
const byte framerate = 2;    // time between flashing frames
int frameStep = 0;          // frame counter for flashing modes
const int transitionRate = 3;    // fade time transitioning between modes

// Interface memorizing
boolean buttonState;             // the current reading from the input pin

// Things to remember
int state = -1;      // What state of the programme are we in?
int pressed = 0;
int firstPressedTime;    // how long ago was the button pressed?
byte currentLEDvalue[3] = { 0, 0, 0};

Adafruit_NeoPixel strip = Adafruit_NeoPixel(3, PIN, NEO_GRB + NEO_KHZ800);

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

void setup() {
  setup_watchdog(9);

  pinMode(FET,OUTPUT);
  digitalWrite(FET,HIGH); //setup FET

  pinMode(PIN,OUTPUT);
  digitalWrite(PIN,LOW); //setup LED signal bus

  shutdownTimer = millis() + (60000*90);  // shutdown timer;

  randomSeed(analogRead(A8)+analogRead(A7));

  PORTA = (1<<PA7); // turn on pull-up on button

  strip.begin();
  strip.show();

  startupFlash();    // flash to show that the programme's started

  goToSleep();    // sshhhh... there there...
}

void loop() {
  buttonState = digitalRead(buttonPin);

  // All of the states set the currentLEDvalue, here we set the LEDs from those values
  analogWrite(ledPin0, doGamma(currentLEDvalue[0]));
  analogWrite(ledPin1, doGamma(currentLEDvalue[1]));
  analogWrite(ledPin2, doGamma(currentLEDvalue[2]));

  // ASLEEP, woken up
  if (state == -1) {
    if (buttonState == HIGH) {     // button released, wait for second button click
      firstPressedTime = millis();
      state++;
    }
  }

  // Double-click
  else if (state == 0) { // waiting for another click
    int timeSinceFirstPress = millis() - firstPressedTime;
    if (timeSinceFirstPress > doubleClickThresh) {
      goToSleep();
    }    // double click didn't happen in time, go back to sleep
  }

  if (state > -1 && buttonState == LOW) {
    pressed = 1;
  } else if (pressed == 1 && buttonState == HIGH) {
    state++;
    pressed = 0;
    // The number of modes
    if (state > 10) {
      state = 99;
    } else if (state > 2) { // Turn everthing off when switching to a blinking mode.
      currentLEDvalue[0] = 0; // set current value to 0 so that we can fade up.
      currentLEDvalue[1] = 0;
      currentLEDvalue[2] = 0;
    }
  }

  // SAFETY SOLID
  if (state == 1) {
    if (currentLEDvalue[0] < safetyBrightness) {
      currentLEDvalue[0]++;
    }     // fade in to solidBrightness value
    if (currentLEDvalue[1] < safetyBrightness) {
      currentLEDvalue[1]++;
    }
    if (currentLEDvalue[2] < safetyBrightness) {
      currentLEDvalue[2]++;
    }
    delay(3);
  }

  // FASHION SOLID
  else if (state == 2) {
    if (currentLEDvalue[0] > fashionBrightness) {
      currentLEDvalue[0]--;
    }     // fade down to solidBrightness value
    if (currentLEDvalue[1] > fashionBrightness) {
      currentLEDvalue[1]--;
    }
    if (currentLEDvalue[2] > fashionBrightness) {
      currentLEDvalue[2]--;
    }
    delay(3);
  }
  else if (state > 2 && state < 99) {
    doFlashing(state);
  }

  // Waiting for button release to go to sleep
  else if (state == 99) {
    // linear fading
    if (currentLEDvalue[0] > 0) {
      currentLEDvalue[0]--;
    }
    if (currentLEDvalue[1] > 0) {
      currentLEDvalue[1]--;
    }
    if (currentLEDvalue[2] > 0) {
      currentLEDvalue[2]--;
    }
    delay(transitionRate);

    if ((currentLEDvalue[0] + currentLEDvalue[1] + currentLEDvalue[2]) == 0) {
      goToSleep();
    }       // go to sleep when the button's been released and fading is done
  }


}

// Flash pattern when the Edge turns on
void startupFlash() {
  // v 3.2.2 flash pattern
  for (int i = 255; i > 0; i--) {
    strip.setPixelColor(0, doGamma(i), doGamma(i), doGamma(i));
    strip.setPixelColor(1, doGamma(i), doGamma(i), doGamma(i));
    strip.setPixelColor(2, doGamma(i), doGamma(i), doGamma(i));
    strip.show();
    delay(1);
  }
  for (int i = 255; i > 0; i--) {
    strip.setPixelColor(0, doGamma(i), doGamma(i), doGamma(i));
    strip.setPixelColor(1, doGamma(i >> 1), doGamma(i >> 1), doGamma(i >> 1));
    strip.setPixelColor(2, doGamma(i), doGamma(i), doGamma(i));
    strip.show();
    delay(1);
  }
}

void system_sleep() {
  //f_wdt=0;                             // reset flag
  cbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter OFF

  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  sleep_enable();

  sleep_mode();                        // System sleeps here

  sleep_disable();                     // System continues execution here when watchdog timed out
  sbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter ON
}

void goToSleep(void)
{
    state = -1;

    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    MCUCR &= ~(_BV(ISC01) | _BV(ISC00));      //INT0 on low level
//    GIMSK |= _BV(INT0);                       //enable INT0
    byte adcsra = ADCSRA;                     //save ADCSRA
    ADCSRA &= ~_BV(ADEN);                     //disable ADC
    cli();                                    //stop interrupts to ensure the BOD timed sequence executes as required
    byte mcucr1 = MCUCR | _BV(BODS) | _BV(BODSE);  //turn off the brown-out detector
    byte mcucr2 = mcucr1 & ~_BV(BODSE);
    MCUCR = mcucr1;
    MCUCR = mcucr2;
    sei();                                    //ensure interrupts enabled so we can wake up again
    sleep_cpu();                              //go to sleep
    sleep_disable();                          //wake up here
    ADCSRA = adcsra;                          //restore ADCSRA
}

// 0=16ms, 1=32ms,2=64ms,3=128ms,4=250ms,5=500ms
// 6=1 sec,7=2 sec, 8=4 sec, 9=8sec
void setup_watchdog(int ii) {

  byte bb;
  int ww;
  if (ii > 9 ) ii=9;
  bb=ii & 7;
  if (ii > 7) bb|= (1<<5);
  bb|= (1<<WDCE);
  ww=bb;

  MCUSR &= ~(1<<WDRF);
  // start timed sequence
  WDTCR |= (1<<WDCE) | (1<<WDE);
  // set new watchdog timeout value
  WDTCR = bb;
  WDTCR |= _BV(WDIE);
}

// Watchdog Interrupt Service / is executed when watchdog timed out
ISR(WDT_vect) {
  //f_wdt=1;  // set global flag
}
