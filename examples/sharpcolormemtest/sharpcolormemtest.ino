/*********************************************************************
This is an example sketch for the Color SHARP Memory Display.
It is based on Adafruit's library for the monochrome displays, which
can be found at https://github.com/adafruit/Adafruit_SHARP_Memory_Display



The original comment follows:

This is an example sketch for our Monochrome SHARP Memory Displays

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/1393

These displays use SPI to communicate, 3 pins are required to  
interface

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.  
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/

#include <Adafruit_GFX.h>
#include <Adafruit_ColorSharpMem.h>

// any pins can be used
#define SCK 10
#define MOSI 11
#define SS 13

Adafruit_SharpMem display(SCK, MOSI, SS);

void setup(void) 
{
  Serial.begin(9600);
  Serial.println("Hello!");

  // start & clear the display
  display.begin();
  display.clearDisplay();

  // draw a single pixel
  display.drawPixel(10, 10, Black);
  display.refresh();
  delay(500);
  display.clearDisplay();

  // draw many lines
  testdrawline();
  delay(500);
  display.clearDisplay();

  // draw rectangles
  testdrawrect();
  delay(500);
  display.clearDisplay();

  // draw multiple rectangles
  testfillrect();
  display.refresh();
  delay(500);
  display.clearDisplay();

  // draw a circle, 10 pixel radius
  display.fillCircle(display.width()/2, display.height()/2, 10, Black);
  display.refresh();
  delay(500);
  display.clearDisplay();

  testdrawroundrect();
  display.refresh();  
  delay(500);
  display.clearDisplay();

  testfillroundrect();
  display.refresh();
  delay(500);
  display.clearDisplay();

  testdrawtriangle();
  display.refresh();
  delay(500);
  display.clearDisplay();
   
  testfilltriangle();
  display.refresh();
  delay(500);
  display.clearDisplay();

  // draw the first ~12 characters in the font
  testdrawchar();
  display.refresh();
  delay(2000);
  display.clearDisplay();

  // text display tests
  display.setTextSize(1);
  display.setTextColor(Blue);
  display.setCursor(0,0);
  display.println("Hello, world!");
  display.setTextColor(Red, Green);
  display.println(3.141592);
  display.setTextSize(2);
  display.setTextColor(Yellow);
  display.print("0x"); display.println(0xDEADBEEF, HEX);
  display.refresh();
  delay(2000);
}

void loop(void) 
{
  // Screen must be refreshed at least once per second
  display.refresh();
  delay(500);
}

///

void testdrawchar(void) {
  display.setTextSize(1);
  display.setTextColor(Black);
  display.setCursor(0,0);

  for (uint8_t i=0; i < 168; i++) {
    if (i == '\n') continue;
    display.write(i);
    //if ((i > 0) && (i % 14 == 0))
      //display.println();
  }    
  display.refresh();
}

void testdrawcircle(void) {
  for (uint8_t i=0; i<display.height(); i+=2) {
    display.drawCircle(display.width()/2-5, display.height()/2-5, i, Black);
    display.refresh();
  }
}

void testfillrect(void) {
  uint8_t color = 1;
  for (uint8_t i=0; i<display.height()/2; i+=3) {
    // alternate colors
    display.fillRect(i, i, display.width()-i*2, display.height()-i*2, color&7);
    display.refresh();
    color++;
  }
}

void testdrawtriangle(void) {
  for (uint16_t i=0; i<min(display.width(),display.height())/2; i+=5) {
    display.drawTriangle(display.width()/2, display.height()/2-i,
                     display.width()/2-i, display.height()/2+i,
                     display.width()/2+i, display.height()/2+i, Black);
    display.refresh();
  }
}

void testfilltriangle(void) {
  uint8_t color = Black;
  for (int16_t i=min(display.width(),display.height())/2; i>0; i-=5) {
    display.fillTriangle(display.width()/2, display.height()/2-i,
                     display.width()/2-i, display.height()/2+i,
                     display.width()/2+i, display.height()/2+i, color);
    if (color == White) color = Black;
    else color = White;
    display.refresh();
  }
}

void testdrawroundrect(void) {
  for (uint8_t i=0; i<display.height()/4; i+=2) {
    display.drawRoundRect(i, i, display.width()-2*i, display.height()-2*i, display.height()/4, Black);
    display.refresh();
  }
}

void testfillroundrect(void) {
  uint8_t color = Black;
  for (uint8_t i=0; i<display.height()/4; i+=2) {
    display.fillRoundRect(i, i, display.width()-2*i, display.height()-2*i, display.height()/4, color);
    if (color == White) color = Black;
    else color = White;
    display.refresh();
  }
}
   
void testdrawrect(void) {
  for (uint8_t i=0; i<display.height()/2; i+=2) {
    display.drawRect(i, i, display.width()-2*i, display.height()-2*i, Black);
    display.refresh();
  }
}

void testdrawline() {  
  for (uint8_t i=0; i<display.width(); i+=4) {
    display.drawLine(0, 0, i, display.height()-1, Red);
    display.refresh();
  }
  for (uint8_t i=0; i<display.height(); i+=4) {
    display.drawLine(0, 0, display.width()-1, i, Green);
    display.refresh();
  }
  delay(250);
  
  display.clearDisplay();
  for (uint8_t i=0; i<display.width(); i+=4) {
    display.drawLine(0, display.height()-1, i, 0, Blue);
    display.refresh();
  }
  for (int8_t i=display.height()-1; i>=0; i-=4) {
    display.drawLine(0, display.height()-1, display.width()-1, i, Yellow);
    display.refresh();
  }
  delay(250);
  
  display.clearDisplay();
  for (int8_t i=display.width()-1; i>=0; i-=4) {
    display.drawLine(display.width()-1, display.height()-1, i, 0, Cyan);
    display.refresh();
  }
  for (int8_t i=display.height()-1; i>=0; i-=4) {
    display.drawLine(display.width()-1, display.height()-1, 0, i, Magenta);
    display.refresh();
  }
  delay(250);

  display.clearDisplay();
  for (uint8_t i=0; i<display.height(); i+=4) {
    display.drawLine(display.width()-1, 0, 0, i, Black);
    display.refresh();
  }
  for (uint8_t i=0; i<display.width(); i+=4) {
    display.drawLine(display.width()-1, 0, i, display.height()-1, Black); 
    display.refresh();
  }
  delay(250);
}
