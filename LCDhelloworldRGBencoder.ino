/* in file LCDHelloworldRGBencoder
 *  
 * goal:
 * use encoder to set RGB value of lcd backlight
 * switch between red, green, blue, brightness when encoder button is pressed
 * SUCCESS!
 */

// include the library code:
#include <LiquidCrystal.h>
#include <Encoder.h>
#include <Adafruit_NeoPixel.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
// const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
//LiquidCrystal lcd(rs, en, d4, d5, d6, d7);



//todo: write defines for the LCD pins
//LCD constants
#define LCD_RED 3
#define LCD_GREEN 5
#define LCD_BLUE 6
#define LCD_ROWS 2
#define LCD_COLS 16
#define LCD_BACKLIGHT colors[0], colors[1], colors[2], colors[3]

//neopixel constants
#define NEOPIX_OUT A0
#define NEOPIX_PARAMS NEO_GRB + NEO_KHZ800

//encoder constants
#define KNOB_PRESS 4
#define KNOB_DATA_A 2
#define KNOB_DATA_B 13

Encoder knob(KNOB_DATA_A, KNOB_DATA_B);
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, NEOPIX_OUT, NEOPIX_PARAMS);

int mode = 0;

bool changed = false;
bool waiting = true;
bool pressed = false;

uint8_t knobPlace = 0;
uint8_t wait[] = {50, 50, 50, 50, 50};
uint8_t colors[] = {0,0,255, 255}; //Red Green Blue Brightness

uint16_t index_i = 0;
uint16_t index_j = 0;

void setup()
{
  // set up the LCD's number of columns and rows:
  lcd.begin(LCD_COLS, LCD_ROWS);
  pinMode(KNOB_PRESS, INPUT_PULLUP);
  pinMode(LCD_RED, OUTPUT);
  pinMode(LCD_GREEN, OUTPUT);
  pinMode(LCD_BLUE, OUTPUT);

  setBacklight(LCD_BACKLIGHT);
  
  strip.begin();
  strip.show();
}

void loop()
{

  //when using a uint8_t, each detent on my encoders moves the position by 4...so just divide by 4 to make it move by 1.
  uint8_t newKnob = knob.read()/4;

  if(newKnob != knobPlace)
  {
    knobPlace = newKnob;
    colors[mode%4] = knobPlace;
    setBacklight(LCD_BACKLIGHT);
  }

  if(digitalRead(KNOB_PRESS) == HIGH)
  {
    if(!pressed)
    {
      pressed = true;
      mode++;
      mode%=5;
      changed = true;
      index_i = 0;
      index_j = 0;
      //knob.write(colors[mode]); // this isn't working atm.

      writeColors(LCD_BACKLIGHT);
      
    }
  }
  else
  {
    pressed = false;
  }

  strip.setPixelColor(index_i, getNextColor());
  strip.show();
  if(waiting)
  {
    delay(wait[mode]);
  }

  index_i += 1;
  index_j += 1;
}

//figure out the next color and the next wait
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

    // this case should never happen
     default: mode += 1;
              mode %= 5;
              color = getNextColor();
  }

  return color;
}

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
  uint32_t color;
  if(WheelPos < 85)
  {
    color =  strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170)
  {
    WheelPos -= 85;
    color = strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  else
  {
    WheelPos -= 170;
    color = strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }

  return color;
}

void setBacklight(uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness)
{
  writeColors(red, green, blue, brightness);
  // normalize the red LED - its brighter than the rest!
  red = map(red, 0, 255, 0, 100);
  green = map(green, 0, 255, 0, 150);
 
  red = map(red, 0, 255, 0, brightness);
  green = map(green, 0, 255, 0, brightness);
  blue = map(blue, 0, 255, 0, brightness);
 
  // common anode so invert!
  red = map(red, 0, 255, 255, 0);
  green = map(green, 0, 255, 255, 0);
  blue = map(blue, 0, 255, 255, 0);
  analogWrite(LCD_RED, red);
  analogWrite(LCD_GREEN, green);
  analogWrite(LCD_BLUE, blue);
}

void writeColors(uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness)
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(red);
  lcd.setCursor(4,0);
  lcd.print(green);
  lcd.setCursor(0,1);
  lcd.print(blue);
  lcd.setCursor(4,1);
  lcd.print(brightness);

  lcd.setCursor(10,0);
  lcd.print(mode);

}


