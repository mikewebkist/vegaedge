#include "Qduino.h"
#include "Wire.h"
#include <Adafruit_NeoPixel.h>
// *** SLEEP CODE

#include <avr/sleep.h>
#include <avr/interrupt.h>
#define BODS 7                   //BOD Sleep bit in MCUCR
#define BODSE 2                  //BOD Sleep enable bit in MCUCR
uint8_t mcucr1, mcucr2;

// Hardware parameters //

const byte ledPin0 = 10;
const byte ledPin2 = 11;
const byte ledPin1 = 13;
const byte voltPin = 3;
const byte buttonPin = 8;
const byte wakePin = 2;
const int numLeds = 8;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(numLeds, 6, NEO_GRB + NEO_KHZ800);
qduino q;
fuelGauge battery;

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
    pinMode(voltPin, INPUT);
    pinMode(wakePin, INPUT);

    startupFlash();    // flash to show that the programme's started

    // goToSleep();    // sshhhh... there there...
}

void loop() {
    buttonState = digitalRead(buttonPin);

    // All of the states set the currentLEDvalue, here we set the LEDs from those values
    for(int i=0; i<numLeds; i++) {
	strip.setPixelColor(i,
		doGamma(currentLEDvalue[i]),
		doGamma(currentLEDvalue[i]),
		doGamma(currentLEDvalue[i]));
    }
    strip.show();

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
	    for(int i=0; i<numLeds; i++) {
		currentLEDvalue[i] = 0; // set current value to 0 so that we can fade up.
	    }
	}
    }

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
	for (int k = 255; k > 0; k--) {
	    for(int i=0; i<numLeds; i++) {
		if(i % 2) {
		    strip.setPixelColor(i, doGamma(k), doGamma(k), doGamma(k));
		} else {
		    strip.setPixelColor(i, doGamma(int(float(k) * .66)), doGamma(int(float(k) * .66)), doGamma(int(float(k) * .66)));
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
    /*
    attachInterrupt(0, wakeUp, LOW);
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF); 
    detachInterrupt(0); 

    attachInterrupt(0, wakeUp, LOW);
    // attachInterrupt(0, wakeUpNow, LOW);
    set_sleep_mode(SLEEP_MODE_PWR_SAVE);
    cli();
    sleep_enable();
    //turn off the brown-out detector.
    //must have an ATtiny45 or ATtiny85 rev C or later for software to be able to disable the BOD.
    //current while sleeping will be <0.5uA if BOD is disabled, <25uA if not.
    sei();                         //ensure interrupts enabled so we can wake up again
    sleep_cpu();                   //go to sleep
    sleep_disable();
    sei();                         //enable interrupts again (but INT0 is disabled from above)
    */
}

/*
ISR(INT0_vect) {
    sleepCycled=1;
}
*/

void wakeUp() {
}                  //nothing to actually do here, the interrupt just wakes us up!
