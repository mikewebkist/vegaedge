/*

Based on code from the Vega Edge Kickstarter project, heavily modified.
- for more information see http://www.vegalite.com/nerds .

Thanks Angella Mackey, David NG McCallum, Johannes Omberg, and other smart people.

-- Michael Cramer / @mikewebkist / cramer@webkist.com

*/

#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <Adafruit_NeoPixel.h>

// Hardware parameters //

#define PIN 12
#define BUTTON 5
#define FET 1
#define NUMLEDS 3

unsigned long shutdownTimer;

// When all lights solid, actually dimmed to reduce strain on the battery.
const uint32_t solidBrightness = 192;
const uint32_t safetyBrightness = 255;
uint32_t fashionBrightness = solidBrightness;

// Flashing timing
int frameStep = 0;          // frame counter for flashing modes
long modeStartTime = millis();
const long sleepAfterSecs = 60 * 60 * 1; // 1 hour.

// Interface memorizing
boolean buttonState;             // the current reading from the input pin

// Things to remember
int state = 99; // What state of the program are we in?
int pressed = 0;
int firstPressedTime;    // how long ago was the button pressed?
uint32_t currentLEDvalue[NUMLEDS];
byte colorMask[3] = { 255, 255, 255 };

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMLEDS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
    // setup_watchdog(9);
    // ADCSRA &= ~_BV(ADEN); //disable ADC

    pinMode(FET,OUTPUT);
    digitalWrite(FET,HIGH); //setup FET

    pinMode(PIN,OUTPUT);
    digitalWrite(PIN,LOW); //setup LED signal bus

    pinMode(BUTTON,INPUT_PULLUP);

    randomSeed(analogRead(A8)+analogRead(A7));

    strip.begin();

    startupFlash(); // flash to show that the programme's started

    // state = 99; // Start asleep.
}

long lastDebounceTime = 0;
boolean lastButtonState = HIGH;
boolean fromSleep = false;

void loop() {
    // fashionBrightness = (analogRead(A9) >> 1) + (safetyBrightness >> 3);

    // Button debounce: still end up with buttonState having the
    // proper value, it just may take a few loop()s.
    boolean newButtonState = digitalRead(BUTTON);
    if (newButtonState != lastButtonState) {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > 50) { // debounce delay: 50ms
        buttonState = newButtonState;
    }

    lastButtonState = newButtonState;

    if (buttonState == LOW) {
        pressed = 1;
    } else if (pressed == 1 && buttonState == HIGH) {
        if(fromSleep) { // Don't change mode when waking.
            fromSleep=false;
        } else {
            state++;
        }
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

    // Go to sleep if running for more than N seconds.
    if((millis() - modeStartTime) > (sleepAfterSecs * 1000)) {
        goToSleep();
    }

    if (state > 0 && state < 99) {
        doFlashing(state);
    }

    // Last mode, go to sleep.
    else if (state == 99) {
        state = 1;
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

void goToSleep(void) {
    for(int i=0; i<NUMLEDS; i++) {
        strip.setPixelColor(i, 0);
    }
    strip.show();

    digitalWrite(FET, LOW); // turn off FET

    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    PCMSK0 |= _BV(PCINT7);  // button is connected to PCINT7
    PCIFR  |= _BV(PCIF0);   // clear any outstanding interrupts for PCINT[7:0]
    PCICR  |= _BV(PCIE0);   // enable pin change interrupts for PCINT[7:0]

    byte adcsra = ADCSRA;   // save ADCSRA
    ADCSRA &= ~_BV(ADEN);   // disable ADC
    cli();                  // stop interrupts to ensure the BOD timed sequence executes as required
    sleep_bod_disable();    // disable brown out detection
    sei();                  // ensure interrupts enabled so we can wake up again
    sleep_cpu();            // go to sleep
    sleep_disable();        // wake up here
    PCMSK0 &= ~_BV(PCINT7); // turn off interrupts for PCINT7
    fromSleep = true;
    digitalWrite(FET,HIGH); // turn FET back on
    ADCSRA = adcsra;        // restore ADCSRA
    modeStartTime = millis();
}

EMPTY_INTERRUPT(PCINT0_vect);
