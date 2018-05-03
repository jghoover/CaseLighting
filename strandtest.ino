// from https://github.com/adafruit/Adafruit_NeoPixel/blob/master/examples/strandtest/strandtest.ino
// edited to write as object-oriented, using millis() instead of delay, etc.
// see https://learn.adafruit.com/multi-tasking-the-arduino-part-1/overview
// and https://learn.adafruit.com/multi-tasking-the-arduino-part-2/overview

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 6

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

// neopixel strand
class NP_Strand
{
  // attributes
  uint8_t wait;
  int mode;
  int nextPixel_i;
  int nextPixel_j;
  unsigned long previousMillis;
  Adafruit_NeoPixel strip;
  
  // todo: rewrite these in terms of nextColor()
  
  uint32_t nextColorWipeRed()
  {
    return nextColorWipe(strip.Color(255, 0, 0));
  }

  uint32_t nextColorWipeGreen()
  {
    return nextColorWipe(strip.Color(0, 255, 0));
  }

  uint32_t nextColorWipeBlue()
  {
    return nextColorWipe(strip.Color(0, 0, 255));
  }

  uint32_t nextColorWipe(uint32_t color)
  {
    waiting = true;
    index_i %= strip.numPixels();
    return color;
  }

  uint32_t nextRainbow()
  {
    index_i %= strip.numPixels();
    index_j %= 256;

    if(index_i == 0)
    {
      waiting = true;
    }
    else
    {
      waiting = false;
    }

    //uint32_t = color = Wheel((i+j) & 255);

    return Wheel((index_i + index_j) & 255);
  }

  // rewrite this so that it sets the wait based on every index
  uint32_t nextRainbowCycle()
  {
    index_i %= strip.numPixels();
    index_j %= 1280; //1280 = 256 * 5

    if(index_i == 0)
    {
      waiting = true;
    }
    else
    {
      waiting = false;
    }

    //uint32_t color = Wheel((i*265/pixels) + j) & 255);

    return Wheel(((index_i*265/strip.numPixels()) + index_j) & 255);
  }

  // Input a value 0 to 255 to get a color value.
  // The colours are a transition r - g - b - back to r.
  uint32_t Wheel(byte WheelPos)
  {
    WheelPos = 255 - WheelPos;
    if(WheelPos < 85)
    {
      return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
    }
    if(WheelPos < 170)
    {
      WheelPos -= 85;
      return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
    WheelPos -= 170;
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
  
  uint32_t getNextColor()
  {
    uint32_t color;

    switch(mode)
    {
      case 0: color = nextColorWipeRed();
              break;

      case 1: color = nextColorWipeGreen();
              break;

      case 2: color = nextColorWipeBlue();
              break;

      case 3: color = nextRainbow();
              break;

      case 4: color = nextRainbowCycle();
              break;

      // this case should never happen...but if something goes wrong, it could infinitely recurse.  rewrite.
       default: mode += 1;
                mode %= 5;
                color = getNextColor();
    }
    
    return color;
  }
    
  
  public:
  NP_Strand(numPixels, pin, flags)
  {
    mode = 0;
    wait = 60;
    previousMillis = 0;
    strip = Adafruit_NeoPixel(numPixels, pin, flags);
    nextColor = strip.color(255,0,0);
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
  }
  
  void update()
  {
    if(millis() - previousMillis > wait)
    {
      previousMillis = 0;
      strip.setPixelColor(index_i, getNextColor());
      strip.show();
    }
  }
  
  void setMode(int mode)
  {
    this->mode = mode;
    nextPixel_i = 0;
    nextPixel_j = 0;
  }
}

void setup()
{
  // strip.begin();
  // strip.show(); // Initialize all pixels to 'off'
}

void loop()
{
  
  // Some example procedures showing how to display to the pixels:
  colorWipe(strip.Color(255, 0, 0), 50); // Red
  colorWipe(strip.Color(0, 255, 0), 50); // Green
  colorWipe(strip.Color(0, 0, 255), 50); // Blue
//colorWipe(strip.Color(0, 0, 0, 255), 50); // White RGBW
  // Send a theater pixel chase in...
  theaterChase(strip.Color(127, 127, 127), 50); // White
  theaterChase(strip.Color(127, 0, 0), 50); // Red
  theaterChase(strip.Color(0, 0, 127), 50); // Blue

  rainbow(20);
  rainbowCycle(20);
  theaterChaseRainbow(50);
}
