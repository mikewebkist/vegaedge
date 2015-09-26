#include "Qduino.h"
#include "Wire.h"
#include <Adafruit_NeoPixel.h>
// *** SLEEP CODE

#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#define BODS 7                   //BOD Sleep bit in MCUCR
#define BODSE 2                  //BOD Sleep enable bit in MCUCR
uint8_t mcucr1, mcucr2;

// Hardware parameters //

const byte ledPin0 = 10;
const byte ledPin2 = 11;
const byte ledPin1 = 13;
const byte buttonPin = 2;
const int numLeds = 8;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(numLeds, 6, NEO_GRB + NEO_KHZ800);
qduino q;
fuelGauge battery;

const int holdClickTime = 1000;    // time between double-clicks, otherwise goes to sleep

// When all lights solid, actually dimmed to reduce strain on the battery.
const byte solidBrightness = 192;
const byte fashionBrightness = 96;
const byte safetyBrightness = 192;

// Flashing timing
int frameStep = 0;          // frame counter for flashing modes
const int transitionRate = 3;    // fade time transitioning between modes

// Interface memorizing
boolean buttonState = LOW;

// Things to remember
int state = -1;      // What state of the programme are we in?
int pressed = 0;
long firstPressedTime;    // how long ago was the button pressed?
byte currentLEDvalue[numLeds] = { 0, 0, 0, 0, 0, 0, 0, 0 };
int sleepCycled=0;

void setup() {
    // disable ADC, cut power consumption
    ADCSRA = 0;
    strip.begin();
    q.setup();
    q.ledOff();
    battery.setup();

    //  Define pins
    pinMode(buttonPin, INPUT_PULLUP);
    startupFlash();    // flash to show that the programme's started
    goToSleep();    // sshhhh... there there...
}

long lastDebounceTime = 0;
boolean lastButtonState = HIGH;

void loop() {
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

    if(state == -1) {
	if (buttonState == LOW) {
	    if ((millis() - firstPressedTime) > holdClickTime) {
		state++;
	    }
	} else { // buttonState == HIGH: released too early
	    q.setRGB("yellow");
	    delay(10);
	    goToSleep();
	}
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
	    for(int i=0; i<numLeds; i++) {
		currentLEDvalue[i] = 0; // set current value to 0 so that we can fade up.
	    }
	}
    }

    // All of the states set the currentLEDvalue, here we set the LEDs from those values
    for(int i=0; i<numLeds; i++) {
	strip.setPixelColor(i,
		doGamma(currentLEDvalue[i]),
		doGamma(currentLEDvalue[i]),
		doGamma(currentLEDvalue[i]));
    }
    strip.show();

    // SAFETY SOLID
    if (state == 1) {
	for(int i=0; i<numLeds; i++) {
	    if (currentLEDvalue[i] < safetyBrightness) {
		currentLEDvalue[i]++;
	    } // fade in to solidBrightness value
	}
	delay(3);
    }

    // FASHION SOLID
    else if (state == 2) {
	for(int i=0; i<numLeds; i++) {
	    if (currentLEDvalue[i] > fashionBrightness) {
		currentLEDvalue[i]--;
	    } // fade in to solidBrightness value
	}
	delay(3);
    }
    else if (state > 2 && state < 99) {
	doFlashing(state);
    }

    // Waiting for button release to go to sleep
    else if (state == 99) {
	// linear fading
	for(int i=0; i<numLeds; i++) {
	    if (currentLEDvalue[i] > 0) {
		currentLEDvalue[i]--;
	    }
	}
	delay(transitionRate);

	int total = 0;
	for(int i=0; i<numLeds; i++) { total = total + currentLEDvalue[i]; }
	if (total == 0) {
	    goToSleep();
	}       // go to sleep when the button's been released and fading is done
    }


}

// Flash pattern when the Edge turns on
void startupFlash() {
    // v 3.2.2 flash pattern
    for(int j=0; j<2; j++) {
	for(int k = 255; k > 0; k--) {
	    for(int i=0; i<numLeds; i++) {
		if (i % 2) {
		    strip.setPixelColor(i, doGamma(k), doGamma(k), doGamma(k));
		} else {
		    strip.setPixelColor(i, doGamma(k >> 1), doGamma(k >> 1), doGamma(k >> 1));
		}
	    }
	    strip.show();
	    delay(1);
	}
    }
}

void goToSleep(void)
{
    state = -1;

    // Disable LEDs.
    for(int i=0; i<numLeds; i++) {
	strip.setPixelColor(i, 0, 0, 0);
    }
    strip.show();

    q.setRGB("red");
    delay(100);
    q.ledOff();

    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    noInterrupts();           // timed sequence follows
    // will be called when pin D2 goes low
    attachInterrupt(1, wake, FALLING);
    EIFR = bit(INTF1);  // clear flag for interrupt 0

    interrupts();             // guarantees next instruction executed
    sleep_cpu();

    // cancel sleep as a precaution
    detachInterrupt(1);
    sleep_disable();
    q.setRGB("green");
    delay(10);
    q.ledOff();
    firstPressedTime = millis();
    lastButtonState = LOW;
}

void wake() {
}  // end of wake
