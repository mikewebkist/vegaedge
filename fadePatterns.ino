void doFlashing(int flash_type) {
    switch(flash_type) {
        case 1: candle(); break;
        case 2: softNoise(); break;
        case 3: fireflies(); break;
        case 4: flickerSunrise(); break;
        case 5: goSolid(safetyBrightness); break;
        case 6: goSolid(fashionBrightness); break;
        case 7: mackeySpecial(); break;
        case 8: strobe(); break;
        case 9: binaryCount(); break;
        case 10: grayCount(); break;
        case 11: strobe(); break;
        default: state = 99; break;
    }
}

void goSolid(byte brightness) {
    for(int i=0; i<NUMLEDS; i++) {
        if(currentLEDvalue[i] < doGamma(brightness)) {
            currentLEDvalue[i] = fadeUp(currentLEDvalue[i], doGamma(brightness) & 0xff);
        } else if(currentLEDvalue[i] > doGamma(brightness)) {
            currentLEDvalue[i] = fadeDown(currentLEDvalue[i], doGamma(brightness) & 0xff);
        }
    }
    delay(3);
}

void chasingMode() {
    static int frameStep = 0;

    for(int i=0; i<NUMLEDS; i++) {
        currentLEDvalue[i] = doGamma(getChase(frameStep, i % 3));// * fashionBrightness / 255);
    }
    delay(3);
    frameStep = (frameStep + 1) % 256;  // reset! consider variable-length flash pattern, then 255 should be something else.
}

void mackeySpecial() {
    currentLEDvalue[1] = 0;
    static int fadeDir = 1;

    long time = (millis() - modeStartTime) % 1700;
    long fadeVal = 0;
    long flashVal = 0;

    // Fade in
    if (time < 750) { fadeVal = time * safetyBrightness / 1500; flashVal = 0; }
    // Fade out
    else if (time >= 750 && time < 1500)  { fadeVal = (750 - (time - 750)) * safetyBrightness / 1500; flashVal = 0; }
    // Turn on middle
    else if (time >= 1500 && time < 1600) { fadeVal = 0; flashVal = safetyBrightness; }
    // Turn off middle
    else if (time >= 1600 && time < 1700) { fadeVal = 0; flashVal = 0; }


    currentLEDvalue[0] = doGamma(fadeVal);
    currentLEDvalue[2] = doGamma(flashVal);
    currentLEDvalue[1] = doGamma(fadeVal);
}

void strobe() {
    /*
    20 annoying cyclist
    50 even more annoying
    100 emergency blinker
    */

    static int goNow;
    goNow = millis()/100;
    allLEDs(doGamma((goNow % 2) != 0 ? 255 : 0));
}

void noise() {
    currentLEDvalue[0] = doGamma(random(fashionBrightness));
    currentLEDvalue[1] = doGamma(random(fashionBrightness));
    currentLEDvalue[2] = doGamma(random(fashionBrightness));
}

void softNoise() {
    /*
    50 fire-like flicker
    10 spzzy fire
    100 still firelike
    200
    */
    //int counter = (millis()/200)%3;
    if ((millis() - modeStartTime) % 50 == 0) { // only change on the 500ms boundary
        currentLEDvalue[((millis() - modeStartTime) / 50) % NUMLEDS] = doGamma(random(fashionBrightness));
    }
}

void candle() {
    /*
    50 fire-like flicker
    10 spzzy fire
    100 still firelike
    200
    */
    uint32_t r = 128 - random(24);
    uint32_t g = 96 - random(8);
    uint32_t b = random(16);

    currentLEDvalue[(millis()/50) % NUMLEDS] = doGamma(r, g, b);
}

void fireflies() {
    static int flyTime = 10000;      // max time between flashes on an LED
    // static long nextFly[3] = { random(flyTime) + modeStartTime, random(flyTime) + modeStartTime, random(flyTime) + modeStartTime };
    static long nextFly[3] = { 0, 0, 0 };
    static long timeNow;
    static long fadeOn;

    timeNow = millis();

    if(timeNow > fadeOn) {
        if(anyLit()) {
            for(int i=0; i<NUMLEDS; i++) {
                if(currentLEDvalue[i] > 0) {
                    currentLEDvalue[i] = fadeDown(currentLEDvalue[i]);
                }
            }
            fadeOn = timeNow + 5; // fade by one every 5 millis.
        }
    }

    // flash the fly if its wait time has passed
    for (int x=0; x<NUMLEDS; x++){
        if (timeNow > nextFly[x]) {
            currentLEDvalue[x] = doGamma(fashionBrightness - random(fashionBrightness / 25), fashionBrightness, random(fashionBrightness / 10));
            nextFly[x] = timeNow + random(flyTime);
        }
        // else if ((timeNow - nextFly[x]) > flyTime) {    // eliminate weird persistence from previous iterations
        //     nextFly[x] = timeNow + random(flyTime);
        // }
    }
}

void flickerSunrise() {
    int counter = (millis()/20)%256;

    // fade
    currentLEDvalue[0] = doGamma(random(counter) >> 1);
    currentLEDvalue[1] = doGamma(random(counter));
    currentLEDvalue[2] = doGamma(random(counter) >> 1);
}

void binaryCount() {
    static int n = 0;
    const unsigned long nextIncrement = 250;
    static unsigned long nextTime = 0;
    unsigned long timeNow;

    timeNow = millis();
    if (timeNow > nextTime) {
        for(int i=0; i<NUMLEDS; i++) {
            currentLEDvalue[i] = doGamma(((n >> i) & 1) * fashionBrightness);
        }
        n = ++n % 8;
        nextTime = timeNow + nextIncrement;
    }
}

void grayCount() {
    // http://en.wikipedia.org/wiki/Gray_code
    static int n = 0;
    const unsigned long nextIncrement = 250;
    static unsigned long nextTime = 0;
    unsigned long timeNow;

    timeNow = millis();
    if (timeNow > nextTime) {
        int x = n - 1;
        x = x ^ (x >> 1);
        for(int i=0; i<NUMLEDS; i++) {
            currentLEDvalue[i] = doGamma(((x >> i) & 1) * fashionBrightness);
        }
        n++;
        nextTime = timeNow + nextIncrement;
    }
}

// void johnsonCounter() {
//     // http://en.wikipedia.org/wiki/Ring_counter#Four-bit_ring_counter_sequences
//     static byte n = 0;
//     const unsigned long nextIncrement = 100;
//     static unsigned long nextTime = 0;
//     unsigned long timeNow;
//
//     timeNow = millis();
//     if (timeNow > nextTime) {
//         // Take LSB, flip it, move it to MSB, shift byte right 1 bit.
//         n = (n & 1 ^ 1) << 2 | n >> 1;
//         currentLEDvalue[0] =  (n &  1)      * fashionBrightness;
//         currentLEDvalue[1] = ((n >> 1) & 1) * fashionBrightness;
//         currentLEDvalue[2] = ((n >> 2) & 1) * fashionBrightness;
//         nextTime = timeNow + nextIncrement;
//     }
// }
