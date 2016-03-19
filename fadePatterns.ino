void doFlashing(int flash_type) {
    switch(flash_type) {
        case 1: goSolid(safetyBrightness); break;
        case 2: goSolid(fashionBrightness); break;
        case 3: mackeySpecial(); break;
        case 4: chasingMode(); break;
        case 5: softNoise(); break;
        case 6: fireflies(); break;
        case 7: binaryCount(); break;
        case 8: grayCount(); break;
        case 9: johnsonCounter(); break;
        case 10: strobe(); break;
        default: state = 99; break;
    }
}

void goSolid(byte brightness) {
    for(int led=0; led<3; led++) {
        if(currentLEDvalue[led] < brightness) {
            currentLEDvalue[led]++;
        } else if(currentLEDvalue[led] > brightness) {
            currentLEDvalue[led]--;
        }
    }
    delay(3);
}

void chasingMode() {
    currentLEDvalue[0] = getChase(frameStep, 0);
    currentLEDvalue[1] = getChase(frameStep, 1);
    currentLEDvalue[2] = getChase(frameStep, 2);
    delay(3);
    frameStep = (frameStep + 1) % 256;  // reset! consider variable-length flash pattern, then 255 should be something else.
}

void mackeySpecial() {
    currentLEDvalue[1] = 0;
    static int fadeDir = 1;

    if (currentLEDvalue[0] == 0) {
        fadeDir = 1;
        currentLEDvalue[1] = fashionBrightness;
        setLEDs();
        delay(100);
        currentLEDvalue[1] = 0;
        setLEDs();
        delay(100);
    }
    else if (currentLEDvalue[0] == fashionBrightness) {
        fadeDir = -1;
    }

    currentLEDvalue[0] += fadeDir;
    currentLEDvalue[2] = currentLEDvalue[0];
    delay(3);
}

void strobe() {
    /*
    20 annoying cyclist
    50 even more annoying
    100 emergency blinker
    */

    static int goNow;
    goNow = (millis()/20)%2;
    currentLEDvalue[0] = goNow * 255 * .66;
    currentLEDvalue[1] = goNow * 255;
    currentLEDvalue[2] = goNow * 255 * .66;

}

void noise() {
    currentLEDvalue[0] = random(fashionBrightness);
    currentLEDvalue[1] = random(fashionBrightness);
    currentLEDvalue[2] = random(fashionBrightness);
}

void softNoise() {
    /*
    50 fire-like flicker
    10 spzzy fire
    100 still firelike
    200
    */
    colorMask[0] = 255; colorMask[1] = 127; colorMask[2] = 15;
    //int counter = (millis()/200)%3;
    currentLEDvalue[(millis()/100)%3] = random(fashionBrightness);
}

void softNoiseProper() {
    /*
    50 twinkling atmosphere
    30 faster twinkling
    10 epilepsy

    */

    static int counter;
    static int lastCounter = -1;
    counter = (millis()/50)%3;
    if (counter!= lastCounter) {
        currentLEDvalue[counter] = random(fashionBrightness);
        lastCounter = counter;
    }
}

void fireflies() {
    static int nextFly[3] = {0, 0, 0};

    static int fireflyFade = 1;
    static int flyTime = 3000;      // max time between flashes on an LED
    static int timeNow;

    colorMask[0] = 255; colorMask[1] = 255; colorMask[2] = 0;

    timeNow = millis();

    // flash the fly if its wait time has passed
    for (int x = 0; x < 3; x++){
        if (timeNow > nextFly[x]) {
            currentLEDvalue[x] = random(96, fashionBrightness);
            nextFly[x] = timeNow + random(flyTime);
        }
        else if ((timeNow - nextFly[x]) > flyTime) {    // eliminate weird persistence from previous iterations
            nextFly[x] = timeNow + random(flyTime);
        }
    }

    // fade
    currentLEDvalue[0] = max(currentLEDvalue[0] - fireflyFade, 0);
    currentLEDvalue[1] = max(currentLEDvalue[1] - fireflyFade, 0);
    currentLEDvalue[2] = max(currentLEDvalue[2] - fireflyFade, 0);
}

void flickerSunrise() {
    int counter = (millis()/20)%256;

    // fade
    currentLEDvalue[0] = random(int(float(counter) * .66));
    currentLEDvalue[1] = random(counter);
    currentLEDvalue[2] = random(int(float(counter) * .66));
}

void gaussRise() {
    static int counter;
    counter = (millis()/15)%256;
    static int offset = 5;

    // fade
    currentLEDvalue[0] = constrain(int(float(counter) * .33) + random(offset) - offset/2, 12 , 255);
    currentLEDvalue[1] = constrain( counter + random(offset) - offset/2, 12 , 255);
    currentLEDvalue[2] =  constrain(int(float(counter) * .33) + random(offset) - offset/2, 12 , 255);
}

void binaryCount() {
    static int n = 0;
    const unsigned long nextIncrement = 250;
    static unsigned long nextTime = 0;
    unsigned long timeNow;

    timeNow = millis();
    if (timeNow > nextTime) {
        currentLEDvalue[0] =  (n &  1)      * fashionBrightness;
        currentLEDvalue[1] = ((n >> 1) & 1) * fashionBrightness;
        currentLEDvalue[2] = ((n >> 2) & 1) * fashionBrightness;
        n = ++n % 8;
        nextTime = timeNow + nextIncrement;
    }
}

void grayCount() {
    // http://en.wikipedia.org/wiki/Gray_code
    static byte n = 0;
    static byte grayBits[] = { 0, 1, 0, 2 };
    const unsigned long nextIncrement = 250;
    static unsigned long nextTime = 0;
    unsigned long timeNow;

    timeNow = millis();
    if (timeNow > nextTime) {
        currentLEDvalue[grayBits[n]] = currentLEDvalue[grayBits[n]]  == 0 ? fashionBrightness : 0;
        n = ++n % 4;
        nextTime = timeNow + nextIncrement;
    }
}

void johnsonCounter() {
    // http://en.wikipedia.org/wiki/Ring_counter#Four-bit_ring_counter_sequences
    static byte n = 0;
    const unsigned long nextIncrement = 100;
    static unsigned long nextTime = 0;
    unsigned long timeNow;

    timeNow = millis();
    if (timeNow > nextTime) {
        // Take LSB, flip it, move it to MSB, shift byte right 1 bit.
        n = (n & 1 ^ 1) << 2 | n >> 1;
        currentLEDvalue[0] =  (n &  1)      * fashionBrightness;
        currentLEDvalue[1] = ((n >> 1) & 1) * fashionBrightness;
        currentLEDvalue[2] = ((n >> 2) & 1) * fashionBrightness;
        nextTime = timeNow + nextIncrement;
    }
}
