void doFlashing(int flash_type) {
    switch(flash_type) {
        case 1: goSolid(safetyBrightness); break;
        case 2: goSolid(fashionBrightness); break;
        // case 3: mackeySpecial(); break;
        case 3: candle(); break;
        case 4: chasingMode(); break;
        case 5: softNoise(); break;
        case 6: fireflies(); break;
        case 7: binaryCount(); break;
        case 8: grayCount(); break;
        case 9: flickerSunrise(); break;
        case 10: strobe(); break;
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
    for(int i=0; i<NUMLEDS; i++) {
        currentLEDvalue[i] = doGamma(getChase(frameStep, i % 3));// * fashionBrightness / 255);
    }
    delay(3);
    frameStep = (frameStep + 1) % 256;  // reset! consider variable-length flash pattern, then 255 should be something else.
}

// void mackeySpecial() {
//     currentLEDvalue[1] = 0;
//     static int fadeDir = 1;
//
//     if (currentLEDvalue[0] == 0) {
//         fadeDir = 1;
//         currentLEDvalue[1] = doGamma(fashionBrightness);
//         for(int i=0; i<NUMLEDS; i++) {
//             strip.setPixelColor(i, doGamma(currentLEDvalue[i]));
//         }
//         strip.show();
//         delay(100);
//         currentLEDvalue[1] = 0;
//         for(int i=0; i<NUMLEDS; i++) {
//             strip.setPixelColor(i, doGamma(currentLEDvalue[i]));
//         }
//         strip.show();
//         delay(100);
//     }
//     else if (currentLEDvalue[0] == doGamma(fashionBrightness)) {
//         fadeDir = -1;
//     }
//
//     currentLEDvalue[0] += fadeDir;
//     currentLEDvalue[2] = currentLEDvalue[0];
//     delay(3);
// }

void strobe() {
    /*
    20 annoying cyclist
    50 even more annoying
    100 emergency blinker
    */

    static int goNow;
    goNow = (millis()/20)%2;
    currentLEDvalue[0] = doGamma(goNow * 255 >> 1);
    currentLEDvalue[1] = doGamma(goNow * 255);
    currentLEDvalue[2] = doGamma(goNow * 255 >> 1);

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
    currentLEDvalue[(millis()/50) % NUMLEDS] = doGamma(random(fashionBrightness),0,0);
}
void candle() {
    /*
    50 fire-like flicker
    10 spzzy fire
    100 still firelike
    200
    */
    currentLEDvalue[(millis()/30) % NUMLEDS] = doGamma(255 - random(64), 110, random(16));
}

void fireflies() {
    static long nextFly[3] = {0, 0, 0};

    static uint32_t fireflyFade = 0x010100;
    static int flyTime = 10000;      // max time between flashes on an LED
    static long timeNow;

    timeNow = millis();

    // flash the fly if its wait time has passed
    for (int x = 0; x < NUMLEDS; x++){
        if (timeNow > nextFly[x]) {
            uint32_t flyGlow = fashionBrightness;
            currentLEDvalue[x] = doGamma(flyGlow, flyGlow, 0);
            nextFly[x] = timeNow + random(flyTime);
        }
        else if ((timeNow - nextFly[x]) > flyTime) {    // eliminate weird persistence from previous iterations
            nextFly[x] = timeNow + random(flyTime);
        }
    }

    // fade
    for(int i=0; i<NUMLEDS; i++) {
        if(currentLEDvalue[i] > 0) {    
            currentLEDvalue[i] = currentLEDvalue[i] - fireflyFade;
        }
    }
}

void flickerSunrise() {
    int counter = (millis()/20)%256;

    // fade
    currentLEDvalue[0] = doGamma(random(counter) >> 1);
    currentLEDvalue[1] = doGamma(random(counter) >> 1);
    currentLEDvalue[2] = doGamma(random(counter), random(counter), 0);
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
