/*********************************************************************
This is a port of the Adafruit Arduino library for Color SHARP Memory Displays
This library supports multi-color Sharp LCDs and is designed for the Arduino Due.
vcom-related stuff has been removed, I didn't understand what it was doing.

This code is not optimised at all! It's barely more than a proof of concept.

If you need something to run really fast:

 * SPI peripheral and DMA should be used
 * 32-bit accesses should be used where possible
 * Rotation should be done per object, not per pixel
 * Fast horizontal lines should be implemented

It can be made more robust too:

 * Optimise timing, remove hacky volatile delays that depend on clock speed


The original readme follows:


  Pick a SHARP Memory LCD up today in the adafruit shop!
  ------> http://www.adafruit.com/products/1393

These displays use SPI to communicate, 3 pins are required to  
interface

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada for Adafruit Industries.
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
 *********************************************************************/

#include "Adafruit_ColorSharpMem.h"

/**************************************************************************
    Sharp Memory Display Connector
    -----------------------------------------------------------------------
    Pin   Function        Notes
    ===   ==============  ===============================
      1   VIN             3.3-5.0V (into LDO supply)
      2   3V3             3.3V out
      3   GND
      4   SCLK            Serial Clock
      5   MOSI            Serial Data Input
      6   CS              Serial Chip Select
      9   EXTMODE         COM Inversion Select (Low = SW clock/serial)
      7   EXTCOMIN        External COM Inversion Signal
      8   DISP            Display On(High)/Off(Low)

 **************************************************************************/

#define SHARPMEM_BIT_WRITECMD   (0x80)
#define SHARPMEM_BIT_CLEAR      (0x20)


//Volatile dummy for small delays
volatile uint32_t d = 0;

//Pack data 2 pixels per byte - eg 0bX111X000 is a white followed by a black. (X is ignored)
uint8_t sharpmem_buffer[(SHARPMEM_LCDWIDTH * SHARPMEM_LCDHEIGHT) / 2];

/* ************* */
/* CONSTRUCTORS  */
/* ************* */
Adafruit_SharpMem::Adafruit_SharpMem(uint8_t clk, uint8_t mosi, uint8_t ss) :
										Adafruit_GFX(SHARPMEM_LCDWIDTH, SHARPMEM_LCDHEIGHT) {
	_clk = clk;
	_mosi = mosi;
	_ss = ss;

	//Set pin state before direction to make sure they start this way (no glitching)
	digitalWrite(_ss, HIGH);
	digitalWrite(_clk, LOW);
	digitalWrite(_mosi, HIGH);

	pinMode(_ss, OUTPUT);
	pinMode(_clk, OUTPUT);
	pinMode(_mosi, OUTPUT);

	clkport     = portOutputRegister(digitalPinToPort(_clk));
	clkpinmask  = digitalPinToBitMask(_clk);
	dataport    = portOutputRegister(digitalPinToPort(_mosi));
	datapinmask = digitalPinToBitMask(_mosi);

}

void Adafruit_SharpMem::begin() {
	setRotation(2);
}

/* *************** */
/* PRIVATE METHODS */
/* *************** */

/**************************************************************************/
/*!
    @brief  Sends a single byte in pseudo-SPI.
 */
/**************************************************************************/
void Adafruit_SharpMem::sendbyte(uint8_t data)
{
	uint8_t i = 0;

	//LCD expects LSB first
	for (i=0; i<8; i++)
	{

		d++;d++;d++;

		//Make sure clock starts low
		*clkport &= ~clkpinmask;
		if (data & 0x80)
			*dataport |=  datapinmask;
		else
			*dataport &= ~datapinmask;

		d++;d++;d++;

		//Clock is active high
		*clkport |=  clkpinmask;
		data <<= 1;
	}

	d++;d++;d++;

	//Make sure clock ends low
	*clkport &= ~clkpinmask;
}



void Adafruit_SharpMem::sendBit(uint8_t data, uint8_t mask) {

	//Make sure clock starts low
	*clkport &= ~clkpinmask;


	d++;d++;d++;

	if (data & mask)
		*dataport |=  datapinmask;
	else
		*dataport &= ~datapinmask;

	d++;d++;d++;

	//Clock is active high
	*clkport |=  clkpinmask;

}

void Adafruit_SharpMem::sendPixelPair(uint8_t data)
{

	sendBit(data, 0x01);
	sendBit(data, 0x02);
	sendBit(data, 0x04);

	sendBit(data, 0x10);
	sendBit(data, 0x20);
	sendBit(data, 0x40);

	//Make sure clock ends low
	*clkport &= ~clkpinmask;

}

void Adafruit_SharpMem::sendbyteLSB(uint8_t data)
{
	uint8_t i = 0;

	//LCD expects LSB first
	for (i=0; i<8; i++)
	{

		d++;d++;d++;

		//Make sure clock starts low
		*clkport &= ~clkpinmask;

		if (data & 0x01)
			*dataport |=  datapinmask;
		else
			*dataport &= ~datapinmask;

		d++;d++;d++;

		//Clock is active high
		*clkport |=  clkpinmask;
		data >>= 1;

	}

	d++;d++;d++;

	//Make sure clock ends low
	*clkport &= ~clkpinmask;

}


/* ************** */
/* PUBLIC METHODS */
/* ************** */


/**************************************************************************/
/*! 
    @brief Write a single pixel to the image buffer

    @param[in]  x
                The x position (0 based)
    @param[in]  y
                The y position (0 based)
	@param[in]	color
				The pixel color to use
 */
/**************************************************************************/
void Adafruit_SharpMem::drawPixel(int16_t x, int16_t y, uint16_t color) 
{

	//Is the point out of range?
	if((x < 0) || (x >= _width) || (y < 0) || (y >= _height)) return;

	switch(rotation) {
	case 1:
		swap(x, y);
		x = WIDTH  - 1 - x;
		break;
	case 2:
		x = WIDTH  - 1 - x;
		y = HEIGHT - 1 - y;
		break;
	case 3:
		swap(x, y);
		y = HEIGHT - 1 - y;
		break;
	}

	//Mask off any additional color bits. No blending at the moment.
	color = color & 0b00000111;

	//Pack pixels in pairs
	if((x&1)) {
		//Wipe relevant bits
		sharpmem_buffer[(y*SHARPMEM_LCDWIDTH + x) / 2] &= ~(0b01110000);
		//Set relevant bits
		sharpmem_buffer[(y*SHARPMEM_LCDWIDTH + x) / 2] |= (uint8_t) (color << 4);
	} else {
		//Wipe relevant bits
		sharpmem_buffer[(y*SHARPMEM_LCDWIDTH + x) / 2] &= ~(0b00000111);
		//Set relevant bits
		sharpmem_buffer[(y*SHARPMEM_LCDWIDTH + x) / 2] |= (uint8_t) color;
	}

}

/**************************************************************************/
/*! 
    @brief Gets the value of the specified pixel from the buffer

    @param[in]  x
                The x position (0 based)
    @param[in]  y
                The y position (0 based)

    @return     The pixel's color value, as defined in SHARPMEM_COLORS.
 */
/**************************************************************************/
uint8_t Adafruit_SharpMem::getPixel(uint16_t x, uint16_t y)
{

	if(x&1)
		return (sharpmem_buffer[(y*SHARPMEM_LCDWIDTH + x) / 2] & 0b01110000) >> 4;
	else
		return (sharpmem_buffer[(y*SHARPMEM_LCDWIDTH + x) / 2] & 0b00000111);

}

/**************************************************************************/
/*! 
    @brief Clears the screen
 */
/**************************************************************************/
void Adafruit_SharpMem::clearDisplay() 
{

	//Clear framebuffer
	memset(sharpmem_buffer, (White | (White << 4)), (SHARPMEM_LCDWIDTH * SHARPMEM_LCDHEIGHT) / 2);

	//Activate the device
	digitalWrite(_ss, HIGH);

	//Delay tsSCS
	delayMicroseconds(3);

	//Send the clear screen command rather than doing a HW refresh (quicker)
	sendbyte(SHARPMEM_BIT_CLEAR);

	//Send 16 dummy bits (data sheet stipulates at least 13)
	sendbyte(0x00);
	sendbyte(0x00);

	//Delay thSCS
	delayMicroseconds(1);

	//deactivate the device
	digitalWrite(_ss, LOW);

	//Delay twSCSL
	delayMicroseconds(1);

}

/**************************************************************************/
/*! 
    @brief Renders the contents of the pixel buffer on the LCD
 */
/**************************************************************************/
void Adafruit_SharpMem::refresh(void) 
{
	uint16_t y, xpair, currentLine, oldLine;

	//Activate the device
	digitalWrite(_ss, HIGH);

	//Delay tsSCS
	delayMicroseconds(3);

	//Send the write command
	sendbyte(SHARPMEM_BIT_WRITECMD);

	//Send the address for line 1
	currentLine = 1;
	sendbyteLSB(currentLine);

	//For each horizontal line of the image
	for (y=0; y < SHARPMEM_LCDHEIGHT; y++)
	{

		//Send each pixel pair
		for(xpair=0; xpair < SHARPMEM_LCDWIDTH/2; xpair++)
		{
			sendPixelPair(sharpmem_buffer[((y*SHARPMEM_LCDWIDTH)/2) + xpair]);
		}

		currentLine++;

		//Dummy byte to end a line
		sendbyteLSB(0x00);

		//If there are more lines to follow, send the next line number.
		if (currentLine <= SHARPMEM_LCDHEIGHT)
		{
			sendbyteLSB(currentLine);
		}

	}

	//Send another trailing 8 dummy bits for the final line, making a total of 16 dummy bits at end of frame
	sendbyte(0x00);

	//Delay thSCS
	delayMicroseconds(1);

	//Deactivate the device
	digitalWrite(_ss, LOW);

	//Assume nothing will happen to the screen for another microsecond (twSCSL)
}
