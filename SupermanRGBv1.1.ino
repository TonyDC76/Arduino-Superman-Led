/*
Neopixel code for Arduino UNO R3
RGB Animations designed along a Superman Colour scheme, using mostly Red, Green, Blue whole tones and animated transitions between them
Written by Tony Cook using boiler plate code borrowed from the Adafruit Neopixel Examples.
*/

// Includes Go Here...

// Adafruit NeoPixel Library
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>  // Required for 16 MHz Adafruit Trinket
#endif

// Comment out if production code to save CPU cycles.
//#define __DevMode__

// Define LED Data Pin
#define LED_PIN 6

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 31

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

unsigned long pixelPrevious = 0;    // Previous Pixel Millis
unsigned long patternPrevious = 0;  // Previous Pattern Millis
int patternCurrent = 0;             // Current Pattern Number
int patternInterval = 5000;         // Pattern Interval (ms)
int pixelInterval = 50;             // Pixel Interval (ms)
int pixelQueue = 0;                 // Pattern Pixel Queue
int pixelCycle = 0;                 // Pattern Pixel Cycle
uint16_t pixelCurrent = 0;          // Pattern Current Pixel Number
uint16_t pixelNumber = LED_COUNT;   // Total Number of Pixels

// Define some colours here, RGB Format
const int SUPERMAN_RED[] = { 255, 0, 0 };
const int SUPERMAN_GREEN[] = { 0, 255, 0 };
const int SUPERMAN_BLUE[] = { 0, 0, 255 };
const int SUPERMAN_GOLD[] = { 255, 215, 0 };
const int SUPERMAN_WHITE[] = {255, 255, 255};

const int MAX_BRIGHTNESS = 128;
const int MAX_RANDOM = 20;

const long MIN_DELAY = 50000;
const long MAX_DELAY = 300000;

// Convert custom RGB values into packed 32 bit values, expected by the strip class
const uint32_t supermanColRed = strip.Color(SUPERMAN_RED[0], SUPERMAN_RED[1], SUPERMAN_RED[2]);
const uint32_t supermanColBlue = strip.Color(SUPERMAN_BLUE[0], SUPERMAN_BLUE[1], SUPERMAN_BLUE[2]);
const uint32_t supermanColGreen = strip.Color(SUPERMAN_GREEN[0], SUPERMAN_GREEN[1], SUPERMAN_GREEN[2]);
const uint32_t supermanColGold = strip.Color(SUPERMAN_GOLD[0], SUPERMAN_GOLD[1], SUPERMAN_GOLD[2]);
const uint32_t supermanColWhite = strip.Color(255, 255, 255);

//Define Effects for easy randomisation
enum rgbEffects {
    TheaterChase = 0,
    FadeUp = 1,
    FadeDown = 2,
    SpeedingBullet = 3,
    Breathe = 4,
    ColourWipe = 5,
    Rainbow = 6,
    TheaterChaseRainbow = 7,
    Cryptonite = 8
};

rgbEffects runningEffect = 7;

// For speedingBullet function
uint32_t previousPixelColour = 0;

// Random Number to match to the random number for cryptoniteTest
long numberToMatch = 0;

// Main loop colour selection for running effect
uint32_t runningColour;

class PixelData {
  public:
    uint32_t Colour;
    int Brightness;
    int step;
};

uint32_t selectColour(bool useAllColours = false);

void setup() {
  // put your setup code here, to run once:

  // Initialise the strip
  strip.begin();                        // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();                         // Turn OFF all pixels ASAP
  strip.setBrightness(MAX_BRIGHTNESS);  // Set BRIGHTNESS to about 1/5 (max = 255)

  // Setup Serial Interface for DeBugging
  Serial.begin(9600);
  Serial.println("Serial Debugger Now Available...");
  Serial.println();
  Serial.println("Superman themed LED Light effects v1.1");
  Serial.println("Designed and built by Tony Cook");
  Serial.println("for Randal Manawatu-Pearcy");
  Serial.println("Xmas 2023.");
  Serial.println();

  runningColour = supermanColRed;
}

void loop() {
  // put your main code here, to run repeatedly:

  long randomDelay = 0;

  #ifdef __DevMode__
    debugMessage("Running", "Main Code Loop");
  #endif
  
  randomSeed(calculateRandomSeed());
  numberToMatch = random(1, MAX_RANDOM);

  // Generate a new random effect to come up next and make sure it is not the same as the running one
  int newEffect = random(1,8);
    while (newEffect == runningEffect) {
    newEffect = random(1,8);
  }

  #ifdef __DevMode__
    debugMessage(String(newEffect), "New Effect");
  #endif

  rgbEffects effectToRun = newEffect;
  runningEffect = effectToRun;

  // Set the colour for the next effect run
  if (!(effectToRun == 2)) {
    runningColour = selectColour(false);
  };

  #ifdef __DevMode__
    debugMessage(String(runningColour),"Running Colour");
  #endif

  // Run the selected Effect
  switch (effectToRun) {

    case 0:
    // TheaterChase effect
    #ifdef __DevMode__
      debugMessage("Running", "TheaterChase");
    #endif
    theaterChase(runningColour,50);
    break;

    case 1:
    // FadeUp effect
    // Fade up the the running colour
    #ifdef __DevMode__
      debugMessage("Running", "FadeUp");
    #endif
    fadeUp(runningColour, MAX_BRIGHTNESS, 45);
    // Pause for a random delay on the solid colour
    delay(random(MIN_DELAY, MAX_DELAY));
    break;

    case 2:
    // Fade Down
    #ifdef __DevMode__
      debugMessage("Running", "FadeDown");
    #endif
    fadeDown(runningColour, MAX_BRIGHTNESS, 45);
    break;

    case 3:
    // SpeedingBullet
    #ifdef __DevMode__
      debugMessage("Running", "SpeedingBullet");
    #endif
    speedingBullet();
    break;

    case 4:
    // breathe for a random number of times
    #ifdef __DevMode__
      debugMessage("Running", "Breathe");
    #endif
    breathe(runningColour,random(1, 50), MAX_BRIGHTNESS);
    break;

    case 5:
    // ColourWipe
    #ifdef __DevMode__
      debugMessage("Running", "ColourWipe");
    #endif
    colorWipe(runningColour, 40);
    break;

    case 6:
    // Rainbow, keep this one pretty random, don't run all the time
    #ifdef __DevMode__
      debugMessage("Checking", "Rainbow");
    #endif
    numberToMatch = random(1, MAX_RANDOM * 5);
    if (numberToMatch == random(1, MAX_RANDOM * 5)) {
      #ifdef __DevMode__
        debugMessage("Running", "Rainbow");
      #endif
      rainbow(30);
    }
    break;

    case 7:
    // Theater Rainbow, keep this one pretty random, don't run all the time
    numberToMatch = random(1, MAX_RANDOM * 5);
    #ifdef __DevMode__
      debugMessage("Checking", "TheaterChaseRainbow");
    #endif
    if (numberToMatch == random(1, MAX_RANDOM * 5)) {
      #ifdef __DevMode__
        debugMessage("Running", "TheaterChaseRainbow");
      #endif
      theaterChaseRainbow(40);
    }
    break;

    case 8:
    // Cryptonite
    #ifdef __DevMode__
      debugMessage("Checking", "CryptoniteTest");
    #endif
    cryptoniteTest();

  };

}

rgbEffects getNextEffect() {

  #ifdef __DevMode__
      debugMessage("Running", "getNextEffect()");
    #endif

  static rgbEffects effect;
  int randomNumber = random(1,8);

  return rgbEffects(randomNumber);


}

void cryptoniteTest() {

  #ifdef __DevMode__
    debugMessage("Running", "cryptoniteTest()");
  #endif
  long randomNumber = 0;

  randomSeed(calculateRandomSeed());
  randomNumber = random(1, MAX_RANDOM);

  if (numberToMatch == randomNumber) {
    cryptonite();
  }
}

void speedingBullet() {

  #ifdef __DevMode__
    debugMessage("Running", "speedingBullet()");
  #endif
  
  setStripColour(runningColour, MAX_BRIGHTNESS);

  for (int ctr = 1; ctr <= strip.numPixels(); ctr++) {
    strip.setPixelColor(ctr - 1, runningColour);
    strip.setPixelColor(ctr, supermanColWhite);
    strip.show();
    delay(8);
  }

  delay(2000);

  for (int ctr = strip.numPixels(); ctr >= 1; ctr--) {

    strip.setPixelColor(ctr + 1, runningColour);
    strip.setPixelColor(ctr, supermanColWhite);
    strip.show();
    delay(8);
  }

  setStripColour(runningColour, MAX_BRIGHTNESS);

}

void cryptonite() {

  #ifdef __DevMode__
    debugMessage("Running", "cryptonite()");
  #endif

  // Pulse green
  int numPulses = 5;

  colorWipe(supermanColGreen, 10);
  breathe(supermanColGreen, numPulses, MAX_BRIGHTNESS);
  colorWipe(supermanColGreen, 10);
}


// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color, int wait) {
  for (int i = 0; i < strip.numPixels(); i++) {  // For each pixel in strip...
    strip.setPixelColor(i, color);               //  Set pixel's color (in RAM)
    strip.show();                                //  Update strip to match
    delay(wait);                                 //  Pause for a moment
  }
}

void debugMessage(String value, String name) {

  Serial.println(String(millis()) + " - " + name + ": " + value);
}

void breathe(uint32_t colour, int breaths, int maxBrightness) {

  #ifdef __DevMode__
    debugMessage("Running " + String(breaths), "breathe()");
  #endif

  //Clear the strip
  //strip.setBrightness(1);
  //strip.show();

  // Set Strip Colour
  for (int x = 0; x <= strip.numPixels(); x++) {
    strip.setPixelColor(x, colour);
  }

  // Breath in and out, one breath cycle
  
  for (int breath = 1; breath <= breaths; breath++) {

    /*
    Breathe in
    for (int x = 1; x <= maxBrightness; x++) {

      strip.setBrightness(x);
      strip.show();
      delay(60);
    }
    
    

    Breathe out

    for (int x = maxBrightness; x >= 1; x--) {

      strip.setBrightness(x);
      strip.show();
      delay(25);
    }
    */
    fadeUp(runningColour,maxBrightness,35);
    fadeDown(runningColour,maxBrightness,25);

  }
  
  
  
  strip.setBrightness(1);
  strip.show();
  strip.setBrightness(maxBrightness);
}

void setStripColour(uint32_t colour, int brightness) {

  #ifdef __DevMode__
    debugMessage("Running", "setStripColour()");
  #endif
  strip.setBrightness(brightness);
  strip.fill(colour, 0, LED_COUNT);
  strip.show();
}

void fadeUp(uint32_t colour, int brightness, int iDelay) {
  #ifdef __DevMode__
    debugMessage("Running", "fadeUp()");
  #endif
  setStripColour(colour, 1);
  strip.show();

  for (int ctr = 1; ctr <= brightness; ctr++) {
    strip.setBrightness(ctr);
    strip.show();
    delay(iDelay);
  }
}

void fadeDown(uint32_t colour, int brightness, int iDelay) {

  #ifdef __DevMode__
    debugMessage("Running", "fadeDown()");
  #endif
  setStripColour(colour, brightness);
  strip.show();

  for (int ctr = brightness; ctr >= 1; ctr--) {
    strip.setBrightness(ctr);
    strip.show();
    delay(iDelay);
  }

  strip.setBrightness(0);
  strip.show();
  strip.setBrightness(MAX_BRIGHTNESS);
  strip.show();
}

long calculateRandomSeed() {

  // Grab the Analog value from the pins A0-A3, which will be floating as there is no pull down/up resistor on them, and using
  // these values and some fairly BS Maths, generate a pseudo randomSeed value for the Pseudo random number generator, to get an even more Pseudo Random number.
  // No genius to this algorythm!

  #ifdef __DevMode__
    debugMessage("Running", "calculateRandomSeed()");
  #endif

  long valA0 = analogRead(A0);
  long valA1 = analogRead(A1);
  long valA2 = analogRead(A2);
  long valA3 = analogRead(A3);

  // Read a second time as the values will fluctuate
  valA0 = analogRead(A0);
  valA1 = analogRead(A1);
  valA2 = analogRead(A2);
  valA3 = analogRead(A3);

  long newSeed1 = ((valA0 % valA1) + (valA2 % valA3) + valA0 + valA1 + valA2 + valA3) / 4;
  //debugMessage(String(newSeed1),"newSeed1");

  valA0 = analogRead(A0);
  valA1 = analogRead(A1);
  valA2 = analogRead(A2);
  valA3 = analogRead(A3);

  long newSeed2 = ((valA0 % valA1) + (valA2 % valA3) + valA0 + valA1 + valA2 + valA3) / 4;
  //debugMessage(String(newSeed2),"newSeed2");

  valA3 = analogRead(A3);
  long newSeed = (newSeed1 * newSeed2) * valA3;
  #ifdef __DevMode__
    debugMessage(String(newSeed), "New Seed");
  #endif

  return newSeed;
}

// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
void theaterChase(uint32_t color, int wait) {
  for (int a = 0; a < 30; a++) {   // Repeat 30 times...
    for (int b = 0; b < 3; b++) {  //  'b' counts from 0 to 2...
      strip.clear();               //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for (int c = b; c < strip.numPixels(); c += 3) {
        strip.setPixelColor(c, color);  // Set pixel 'c' to value 'color'
      }
      strip.show();  // Update strip with new contents
      delay(wait);   // Pause for a moment
    }
  }
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this loop:
  for (long firstPixelHue = 0; firstPixelHue < 5 * 65536; firstPixelHue += 256) {
    // strip.rainbow() can take a single argument (first pixel hue) or
    // optionally a few extras: number of rainbow repetitions (default 1),
    // saturation and value (brightness) (both 0-255, similar to the
    // ColorHSV() function, default 255), and a true/false flag for whether
    // to apply gamma correction to provide 'truer' colors (default true).
    strip.rainbow(firstPixelHue);
    // Above line is equivalent to:
    // strip.rainbow(firstPixelHue, 1, 255, 255, true);
    strip.show();  // Update strip with new contents
    delay(wait);   // Pause for a moment
  }
}

// Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
void theaterChaseRainbow(int wait) {
  int firstPixelHue = 0;           // First pixel starts at red (hue 0)
  for (int a = 0; a < 30; a++) {   // Repeat 30 times...
    for (int b = 0; b < 3; b++) {  //  'b' counts from 0 to 2...
      strip.clear();               //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for (int c = b; c < strip.numPixels(); c += 3) {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip.numPixels() steps):
        int hue = firstPixelHue + c * 65536L / strip.numPixels();
        uint32_t color = strip.gamma32(strip.ColorHSV(hue));  // hue -> RGB
        strip.setPixelColor(c, color);                        // Set pixel 'c' to value 'color'
      }
      strip.show();                 // Update strip with new contents
      delay(wait);                  // Pause for a moment
      firstPixelHue += 65536 / 90;  // One cycle of color wheel over 90 frames
    }
  }
}

uint32_t selectColour(bool useAllColours) {

  uint32_t newColour;
  int iRandom;
  #ifdef __DevMode__
    debugMessage("Running", "selectColour()");
  #endif

  iRandom = random(1,2000);
  if (!useAllColours){
    if (iRandom >= 1000){
      newColour = supermanColRed;
    } else {
      newColour = supermanColBlue;
    }
  } else {

    if (iRandom <= 400){ 
      newColour = supermanColGold;
    } else if (iRandom > 400 && iRandom <= 800) {
      newColour = supermanColBlue;
    } else if (iRandom > 800 && iRandom <= 1200) {
      newColour = supermanColRed;
    } else if (iRandom > 1200 && iRandom <= 1600) {
      newColour = supermanColWhite;
    } else if (iRandom > 1600) {
      newColour = supermanColGreen;
    }

  }

  return newColour;
}

