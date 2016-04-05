
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
#include <avr/interrupt.h>
#include <Adafruit_NeoPixel.h>

// Hardware parameters //

#define PIN 12
#define button A5
#define FET 1
#define NUMLEDS 3

unsigned long shutdownTimer;

const byte ledPin0 = 0;
const byte ledPin2 = 1;
const byte ledPin1 = 4;
const byte voltPin = 3;
const byte buttonPin = 5;


const int doubleClickThresh = 500;    // time between double-clicks, otherwise goes to sleep

// When all lights solid, actually dimmed to reduce strain on the battery.
//const byte solidBrightness = 32;       // no resistors
//const byte solidBrightness = 64;
//const byte solidBrightness = 128;     // now using gamma array. value of 128 corresponds to PWM of 64
const byte solidBrightness = 192;
//const byte solidBrightness = 255;

const byte safetyBrightness = 255;
byte fashionBrightness = safetyBrightness >> 2;

// Flashing timing
const byte framerate = 2;    // time between flashing frames
int frameStep = 0;          // frame counter for flashing modes
const int transitionRate = 3;    // fade time transitioning between modes
long modeStartTime = 0;

// Interface memorizing
boolean buttonState;             // the current reading from the input pin

// Things to remember
int state = 1;      // What state of the programme are we in?
int pressed = 0;
int firstPressedTime;    // how long ago was the button pressed?
uint32_t currentLEDvalue[NUMLEDS];
byte colorMask[3] = { 255, 255, 255 };

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMLEDS, PIN, NEO_GRB + NEO_KHZ800);

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

void setup() {
    // setup_watchdog(9);

    pinMode(FET,OUTPUT);
    digitalWrite(FET,HIGH); //setup FET

    pinMode(PIN,OUTPUT);
    digitalWrite(PIN,LOW); //setup LED signal bus

    pinMode(buttonPin, INPUT_PULLUP);

    randomSeed(analogRead(A8)+analogRead(A7));

    // PORTA = (1<<PA7); // turn on pull-up on button

    strip.begin();

    startupFlash();    // flash to show that the programme's started
    state = 1;
    // state = 99; // Start asleep.
}

long lastDebounceTime = 0;
boolean lastButtonState = HIGH;

void loop() {
    fashionBrightness = (analogRead(A9) >> 1) + (safetyBrightness >> 3);

    // Button debounce: still end up with buttonState having the
    // proper value, it just may take a few loop()s.
    boolean newButtonState = digitalRead(buttonPin);
    if (newButtonState != lastButtonState) {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > 50) { // debounce delay: 50ms
        buttonState = newButtonState;
    }

    lastButtonState = newButtonState;

    if (state > -1 && buttonState == LOW) {
        pressed = 1;
    } else if (pressed == 1 && buttonState == HIGH) {
        state++;
        pressed = 0;
        // The number of modes
        if (state > 2) { // Turn everthing off when switching to a blinking mode.
            modeStartTime = millis();
            for(int i=0; i<NUMLEDS; i++) {
                currentLEDvalue[i] = 0; // set current value to 0 so that we can fade up.
            }
        }
    }

    // All of the states set the currentLEDvalue, here we set the LEDs from those values
    for(int i=0; i<NUMLEDS; i++) {
        strip.setPixelColor(i, currentLEDvalue[i]);
    }
    strip.show();

    if (state > 0 && state < 99) {
        doFlashing(state);
    }

    // Waiting for button release to go to sleep
    else if (state == 99) {
        // linear fading
        for(int i=0; i<NUMLEDS; i++) {
            if (currentLEDvalue[i] > 0) {
                currentLEDvalue[i] = fadeDown(currentLEDvalue[i]);
            }
        }
        goToSleep();
    }
}

uint32_t fadeDown(uint32_t val) { return fadeDown(val, 0); }
uint32_t fadeDown(uint32_t val, uint32_t lowVal) {
    uint32_t r = (val >> 16) & 0xff;
    uint32_t g = (val >> 8) & 0xff;
    uint32_t b = val & 0xff;
    if(r > lowVal) { r--; }
    if(g > lowVal) { g--; }
    if(b > lowVal) { b--; }
    return r << 16 | g << 8 | b;
}

uint32_t fadeUp(uint32_t val) { return fadeUp(val, 255); }
uint32_t fadeUp(uint32_t val, uint32_t highVal) {
    uint32_t r = (val >> 16) & 0xff;
    uint32_t g = (val >> 8) & 0xff;
    uint32_t b = val & 0xff;
    if(r < highVal) { r++; }
    if(g < highVal) { g++; }
    if(b < highVal) { b++; }
    return r << 16 | g << 8 | b;
}

void startupFlash() {
    // v 3.2.2 flash pattern
    for(int j=0; j<2; j++) {
        for(int k = 255; k > 0; k--) {
            for(int i=0; i<NUMLEDS; i++) {
                strip.setPixelColor(i, doGamma(k));
            }
            strip.show();
            delay(1);
        }
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
    // GIMSK |= _BV(INT0);                       //enable INT0
    byte adcsra = ADCSRA;                     //save ADCSRA
    ADCSRA &= ~_BV(ADEN);                     //disable ADC
    cli();                                    //stop interrupts to ensure the BOD timed sequence executes as required
    byte mcucr1 = MCUCR | _BV(BODS) | _BV(BODSE);  //turn off the brown-out detector
    byte mcucr2 = mcucr1 & ~_BV(BODSE);
    MCUCR = mcucr1;
    MCUCR = mcucr2;
    sei();                                    //ensure interrupts enabled so we can wake up again
    sleep_cpu();                              //go to sleep
    // GIMSK = 0x00;                  //disable INT0
    sleep_disable();                          //wake up here
    ADCSRA = adcsra;                          //restore ADCSRA
}

// Watchdog Interrupt Service / is executed when watchdog timed out
ISR(INT0_vect) {
    //f_wdt=1;  // set global flag
}
