const byte chasing[256][3] PROGMEM = {
    {168, 0, 0},  {168, 0, 0},  {168, 0, 0},  {168, 0, 0},   {168, 0, 0},   {168, 0, 0},   {168, 0, 0},   {168, 0, 0}, 
    {168, 0, 0},  {168, 0, 0},  {168, 0, 0},  {168, 0, 0},   {168, 0, 0},   {168, 0, 0},   {168, 0, 0},   {168, 0, 0}, 
    {168, 0, 0},  {166, 0, 0},  {163, 0, 0},  {161, 0, 0},   {158, 0, 0},   {156, 0, 0},   {154, 0, 0},   {151, 0, 0}, 
    {149, 0, 0},  {147, 0, 0},  {145, 0, 0},  {143, 0, 0},   {140, 0, 0},   {138, 0, 0},   {136, 0, 0},   {134, 0, 0}, 
    {132, 0, 0},  {130, 1, 0},  {128, 4, 0},  {126, 8, 0},   {124, 14, 0},  {121, 23, 0},  {119, 34, 0},  {117, 46, 0}, 
    {115, 61, 0}, {113, 78, 0}, {110, 97, 0}, {108, 118, 0}, {106, 140, 0}, {103, 163, 0}, {101, 186, 0}, {98, 209, 0}, 
    {95, 232, 0}, {92, 239, 0}, {89, 243, 0}, {86, 246, 0},  {82, 247, 0},  {79, 247, 0},  {76, 245, 0},  {72, 242, 0}, 
    {69, 238, 0}, {66, 233, 0}, {63, 227, 0}, {60, 221, 0},  {57, 214, 0},  {54, 206, 0},  {51, 199, 0},  {49, 191, 0}, 
    {46, 183, 0}, {44, 181, 0}, {42, 178, 0}, {41, 176, 0},  {39, 173, 0},  {37, 171, 0},  {36, 168, 0},  {34, 166, 0}, 
    {32, 163, 0}, {31, 161, 0}, {29, 158, 0}, {28, 156, 0},  {27, 153, 0},  {25, 150, 0},  {24, 148, 0},  {23, 145, 0}, 
    {22, 143, 0}, {20, 139, 0}, {19, 136, 0}, {17, 133, 0},  {16, 130, 0},  {15, 127, 0},  {14, 124, 0},  {12, 121, 0}, 
    {11, 118, 0}, {10, 115, 0}, {9, 111, 0},  {8, 108, 0},   {8, 105, 0},   {7, 102, 0},   {6, 99, 0},    {5, 96, 0}, 
    {5, 93, 0},   {4, 90, 1},   {3, 87, 3},   {3, 85, 8},    {2, 82, 15},   {2, 79, 24},   {2, 76, 35},   {1, 74, 49}, 
    {1, 71, 65},  {1, 69, 83},  {1, 66, 103}, {0, 64, 125},  {0, 61, 148},  {0, 59, 172},  {0, 57, 197},  {0, 55, 222}, 
    {0, 53, 247}, {0, 51, 253}, {0, 49, 255}, {0, 48, 255},  {0, 46, 255},  {0, 44, 255},  {0, 43, 255},  {0, 41, 251}, 
    {0, 40, 246}, {0, 39, 239}, {0, 37, 232}, {0, 36, 223},  {0, 35, 215},  {0, 33, 205},  {0, 32, 196},  {0, 31, 187}, 
    {0, 30, 177}, {0, 28, 172}, {0, 27, 167}, {0, 26, 162},  {0, 24, 157},  {0, 23, 152},  {0, 22, 147},  {0, 21, 142}, 
    {0, 20, 137}, {0, 19, 133}, {0, 18, 128}, {0, 17, 124},  {0, 16, 120},  {0, 15, 116},  {0, 14, 112},  {0, 13, 108}, 
    {0, 12, 105}, {0, 11, 102}, {0, 11, 99},  {0, 10, 97},   {0, 9, 94},    {0, 9, 92},    {0, 8, 90},    {0, 8, 88}, 
    {0, 7, 86},   {0, 7, 84},   {0, 6, 82},   {0, 6, 80},    {0, 5, 79},    {0, 5, 77},    {0, 4, 76},    {0, 4, 74}, 
    {0, 4, 73},   {0, 3, 71},   {0, 3, 70},   {0, 3, 69},    {0, 2, 67},    {0, 2, 66},    {0, 2, 65},    {0, 1, 64}, 
    {0, 1, 62},   {0, 1, 61},   {0, 1, 60},   {0, 1, 59},    {0, 1, 57},    {0, 1, 56},    {0, 0, 54},    {0, 0, 53}, 
    {0, 0, 51},   {0, 0, 49},   {0, 0, 47},   {0, 0, 44},    {0, 0, 42},    {0, 0, 40},    {0, 0, 38},    {0, 0, 36}, 
    {0, 0, 33},   {0, 0, 31},   {0, 0, 29},   {0, 0, 27},    {0, 0, 25},    {0, 0, 24},    {0, 0, 22},    {0, 0, 20}, 
    {0, 0, 18},   {0, 0, 17},   {0, 0, 16},   {0, 0, 15},    {0, 0, 13},    {0, 0, 12},    {0, 0, 11},    {0, 0, 10}, 
    {0, 0, 9},    {0, 0, 9},    {0, 0, 8},    {0, 0, 7},     {0, 0, 6},     {0, 0, 6},     {0, 0, 5},     {0, 0, 5},
    {0, 0, 4},    {0, 0, 4},    {0, 0, 3},    {0, 0, 3},     {0, 0, 2},     {0, 0, 2},     {0, 0, 2},     {0, 0, 2},
    {0, 0, 1},    {0, 0, 1},    {0, 0, 1},    {0, 0, 1},     {0, 0, 1},     {0, 0, 1},     {0, 0, 1},     {0, 0, 0},
    {0, 0, 0},    {0, 0, 0},    {0, 0, 0},    {0, 0, 0},     {0, 0, 0},     {0, 0, 0},     {0, 0, 0},     {0, 0, 0},
    {0, 0, 0},    {0, 0, 0},    {0, 0, 0},    {0, 0, 0},     {0, 0, 0},     {0, 0, 0},     {0, 0, 0},     {0, 0, 0},
    {0, 0, 0},    {0, 0, 0},    {0, 0, 0},    {0, 0, 0},     {0, 0, 0},     {0, 0, 0},     {0, 0, 0},     {0, 0, 0},
    {0, 0, 0},    {0, 0, 0},    {0, 0, 0},    {0, 0, 0},     {0, 0, 0},     {0, 0, 0},     {0, 0, 0},     {0, 0, 0}, 
};

void doFlashing(int flash_type) {
    if (flash_type == 3) {      // Mackey special
	currentLEDvalue[2] = 0;
	currentLEDvalue[3] = 0;
	currentLEDvalue[4] = 0;
	currentLEDvalue[5] = 0;
	static int fadeDir = 1;

	if (currentLEDvalue[0] == 0) {
	    fadeDir = 1;
	    strip.setPixelColor(2, 255, 255, 255);
	    strip.setPixelColor(3, 255, 255, 255);
	    strip.setPixelColor(4, 255, 255, 255);
	    strip.setPixelColor(5, 255, 255, 255);
	    strip.show();
	    delay(100);
	    strip.setPixelColor(2, 0, 0, 0);
	    strip.setPixelColor(3, 0, 0, 0);
	    strip.setPixelColor(4, 0, 0, 0);
	    strip.setPixelColor(5, 0, 0, 0);
	    strip.show();
	    delay(100);
	}
	else if (currentLEDvalue[0] == 255) { fadeDir = -1; }     
	currentLEDvalue[0] += fadeDir;
	currentLEDvalue[1] = currentLEDvalue[0];
	currentLEDvalue[6] = currentLEDvalue[0];
	currentLEDvalue[7] = currentLEDvalue[0];
	delay(3);
    }

    else if (flash_type == 4) {      // chasing     
	for(int i=0; i<numLeds; i++) {
	    currentLEDvalue[i] = pgm_read_byte(&(chasing[frameStep][i % 3]));
	}
	delay(3);
	frameStep = (frameStep + 1) % 256;  // reset! consider variable-length flash pattern, then 255 should be something else.   
    }

    else if (flash_type == 5) { softNoise(); }
    else if (flash_type == 6) { fireflies(); }
    //else if (flash_type == 4) { gaussRise(); }
    else if (flash_type == 7) { binaryCount(); }
    else if (flash_type == 8) { grayCount(); }
    else if (flash_type == 9) { johnsonCounter(); }
    else if (flash_type == 10) { batteryLevel(); }
}

void noise() {
    for(int i=0; i<8; i++) {
	currentLEDvalue[i] = random(255);
    }
}

void softNoise() {
    /*
       50 fire-like flicker
       10 spzzy fire
       100 still firelike
       200

     */

    //int counter = (millis()/200)%3;
    currentLEDvalue[(millis()/10)%8] = random(fashionBrightness);
}

void fireflies() {
    static long nextFly[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    static int fireflyFade = 1;
    static int flyTime = 10000;      // max time between flashes on an LED
    static long timeNow;

    timeNow = millis();

    // flash the fly if its wait time has passed
    for (int x = 0; x < 8; x++){ 
	if (timeNow > nextFly[x]) {
	    currentLEDvalue[x] = random(fashionBrightness, safetyBrightness);
	    nextFly[x] = timeNow + random(flyTime);
	}
	else if ((timeNow - nextFly[x]) > flyTime) {    // eliminate weird persistence from previous iterations
	    nextFly[x] = timeNow + random(flyTime);
	}
    }

    // fade
    for(int i=0; i<8; i++) {
	currentLEDvalue[i] = max(currentLEDvalue[i] - fireflyFade, 0);
    }
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
    static byte n = 0;
    const unsigned long nextIncrement = 250;
    static unsigned long nextTime = 0;
    unsigned long timeNow;
    
    timeNow = millis();
    if (timeNow > nextTime) {
	for(int i=0; i<8; i++) {
	    currentLEDvalue[i] =  ((n >> i) &  1) * fashionBrightness;
	}
	n = ++n % 256;
	nextTime = timeNow + nextIncrement;
    }
}

void grayCount() {
    // http://en.wikipedia.org/wiki/Gray_code
    static byte n = 0;
    const unsigned long nextIncrement = 250;
    static unsigned long nextTime = 0;
    unsigned long timeNow;

    timeNow = millis();
    if (timeNow > nextTime) {
	int x = n - 1;
	x = x ^ (x >> 1);
	for(int i=0; i<8; i++) {
	    currentLEDvalue[i] =  ((x >> i) & 1) * fashionBrightness;
	}
	n = ++n % 256;
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
	n = ((n >> 7) ^ 1) | (n << 1);
	for(int i=0; i<8; i++) {
	    currentLEDvalue[i] =  ((n >> i) & 1) * fashionBrightness;
	}
	nextTime = timeNow + nextIncrement;  
    }
}

void batteryLevel() {
    static long nextIncrement = 10000;
    static long nextTime = 0;
    static int brightness = fashionBrightness;
    long timeNow;
    static int charge;
    int i;
    static int cycles = 3;
    
    timeNow = millis();
    if (timeNow > nextTime) {
	battery.wakeUp();
	charge = battery.chargePercentage();
	battery.reset();
	nextTime = timeNow + nextIncrement;
	battery.goToSleep();
    }

    if(brightness == 32) {
	if(cycles--) {
	    brightness = fashionBrightness;
	} else {
	    cycles = 3; // reset counter so next time through we blink
	    state++;    // move on to the next mode (which will for now is sleep)
	}
    } else {
	brightness--;
    }
    
    for(i=0; i<8; i++) {
	if(i <= (8 * charge / 100)) {
	    currentLEDvalue[i] = brightness;
	} else {
	    currentLEDvalue[i] = 0;
	}
    }
    delay(3);
}
