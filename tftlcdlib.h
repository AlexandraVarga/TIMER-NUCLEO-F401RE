#ifndef __tftlcdlib_h
#define __tftlcdlib_h

#include "stm32f4xx.h"
#include <math.h>

// this library is intended for tft lcd displays 240x320 pixels 
// type st7781 or ili932x, tested only on ILI9325 with NUCLEO-F446RE

// for other types of displays with a PARALLEL interface it is necessary to change the function LCD_Init() 
// according to the app note for your display
// you may need to change the LCD_SetAddressWindow function, as well as 
// the shape drawing functions that use this function

// for the same display type but SPI version, you only need to change the LCD_WriteData8b(data) and ConfirmEntry() 
// functions and change the GPIO_MODER, GPIO_IDR, GPIO_ODR registry configuration according to your schema/pinout

///////////////////////////////////////////////////////////////
// 					DISPLAY CONNECTION (446RE - ILI9325)						 //
// __________________________________________________________//
//  ________GPIO_outputs_shared_with_display_(data)________	 //
// |__D7__|__D6__|__D5__|__D4__|__D3__|__D2__|__D1__|__D0__| //
// |__PA8_|_PB10_|__PB4_|__PB5_|__PB3_|_PA10_|__PC7_|__PA9_| //
//																													 //
// 						 ______GPIO_outputs_(control)_______					 //
// 						|__RD__|__WR__|__RS__|__CS__|__RST__|					 //
// 						|__PA0_|__PA1_|__PA4_|__PB0_|__PC1__|					 //
///////////////////////////////////////////////////////////////

// display dimensions //
#define LCD_WIDTH 240
#define LCD_HEIGHT 320

////////////////////// list of colors /////////////////////////
#define WHITE RGB(255,255,255)
#define SILVER RGB(192,192,192)
#define GRAY RGB(128,128,128)
#define BLACK RGB(0,0,0)
#define RED RGB(255,0,0)
#define MAROON RGB(128,0,0)
#define YELLOW RGB(255,255,0)
#define OLIVE RGB(128,128,0)
#define LIME RGB(0,255,0)
#define GREEN RGB(0,128,0)
#define AQUA RGB(0,255,255)
#define TEAL RGB(0,128,128)
#define BLUE RGB(0,0,255)
#define NAVY RGB(0,0,128)
#define FUCHSIA RGB(255,0,255)
#define PURPLE RGB(128,0,128)

//			change of values on pins for display control  			//

#define rst_reset() GPIOC->BSRR |= 1<<17
#define cs_active() GPIOB->BSRR |= 1<<16
#define rs_command() GPIOA->BSRR |= 1<<20
#define wr_active() GPIOA->BSRR |= 1<<17
#define rd_active() GPIOA->BSRR |= 1<<16

#define rst_active() GPIOC->BSRR |= 1<<1
#define cs_idle() GPIOB->BSRR |= 1
#define rs_data() GPIOA->BSRR |= 1<<4
#define wr_idle() GPIOA->BSRR |= 1<<1
#define rd_idle() GPIOA->BSRR |= 1

////////////////// values for ADC calibration  ///////////////

#define X_MIN 1300
#define X_MAX 2850
#define Y_MIN 800
#define Y_MAX 3100
#define ADC_MAX 4095
#define NUMBEROFSAMPLES 2 // number of ADC samples, two are averaged, for more than 3 samples the median is calculated 

/////////////////////////////////////////////////////////////

#define LCD_DELAY 0x7D3
#define PI 3.14159265

////////////////// font parameters  ///////////////////////

#define FONT_WIDTH 5
#define FONT_HEIGHT 7
#define TEXT_ROTATION 1 // 1 - when using the display upright, 0 - when using the display horizontally 
#define TEXT_BACKGROUND 1 // if you want to use a font without a background, change the value from 1 to 0 

//////////////////////////////////////////////////////////////
//										MUST BE ACTIVATED							  			//
//////////////////////////////////////////////////////////////

void LCD_Init(uint16_t color);
void GPIO_Init(void);

// only when using touch //

void ADC_Init(void); // use PA1 and PA4 as analog, PA8 and PB10 as inputs or outputs

//////////////////////////////////////////////////////////////
//							FUNCTIONS FOR USING THE DISPLAY 						//
//////////////////////////////////////////////////////////////

// reads the display ID with the command 0x00 //

uint16_t LCD_ReadID(void);

// reads a 16-bit color stored at a specified point in memory  //

uint16_t LCD_ReadFrameMemory(uint16_t x, uint16_t y);

// uses the ADC (ADC_Init() is necessary) to read the x and y coordinates and touch z  //

void LCD_GetPoint(uint16_t* x, uint16_t* y, uint16_t* z);

//	a list of functions for drawing basic shapes on the screen //

void LCD_FilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void LCD_Rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void LCD_FilledCircle(uint16_t x, uint16_t y, uint16_t r, uint16_t color);
void LCD_Circle(uint16_t x, uint16_t y, uint16_t r, uint16_t color);
void LCD_HorizontalLine(uint16_t x, uint16_t y, uint16_t len, uint16_t color);
void LCD_VerticalLine(uint16_t x, uint16_t y, uint16_t len, uint16_t color);
void LCD_Line(uint16_t x, uint16_t y, uint16_t len, int16_t rotation, uint16_t color);
void LCD_Ellipse(uint16_t x, uint16_t y, uint16_t rx, uint16_t ry, uint16_t color);
void LCD_FilledEllipse(uint16_t x, uint16_t y, uint16_t rx, uint16_t ry, uint16_t color);
void LCD_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void LCD_WriteChar(uint16_t x, uint16_t y, uint8_t ch, uint16_t color, uint16_t background_color);
void LCD_DisplayFill(uint16_t color);

// converts 8-bit red, blue, and green levels to 565 RGB (16-bit for display) //

uint16_t RGB(uint8_t red, uint8_t green, uint8_t blue);

// for writing custom commands and parameters to the display //

void LCD_WriteData8b(uint8_t data);
void LCD_WriteCommand8b(uint8_t data);
void LCD_SetAddressWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

// sets the parallel interface to a high impedance state //

void LCD_GPIOSetIN(void); 

// sets the interface parallel as output  //

void LCD_GPIOSetOUT(void);

//
//
//
//
//
//
//
//
//
//

//////////////////////////////////////////////////////////////
//								PRIVATE DISPLAY FUNCTIONS		      				//
//////////////////////////////////////////////////////////////

static void DrawMode(void);
static void LCD_Wait(uint16_t delay);
static void ConfirmEntry(void);
static void DisplayReset(void);
static void FastWrite(uint8_t colorhigh, uint8_t colorlow, uint32_t numberOfEntries);
static void ClearChar(uint16_t x, uint16_t y, uint16_t background_color);

//	functions for reading sequences of data of different lengths from the display 		//

static uint8_t Read_8b(void);
static void ReadContinuousData(uint16_t command, uint16_t *data, uint16_t numberOfBytes);
static uint16_t Read_Data16b(uint16_t command);

//	ADC functions for reading the coordinates of contact with the display 	//	

static uint16_t ADC_Value(void);
static void GPIO_ADCx(void);
static void GPIO_ADCy(void);
static void GPIO_ADCpress(void);
static uint16_t TouchArea(int16_t x, uint16_t max);
static uint16_t ReadPressure(void);
static const float adc_x = 1.0/(X_MAX - X_MIN);
static const float adc_y = 1.0/(Y_MAX - Y_MIN);
static void TouchMode(void);
static uint16_t ReadADC(void);

// 			enable clock for GPIO 			//

static void RCC_GPIO_CLK_enable(void) {RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN;}


//////////////////////////////////////////////////////////////
//								font size 5x7 pixels 						          //
//////////////////////////////////////////////////////////////
static const uint8_t font5x7[] ={	//  char		 // index (ascii)
	0x00, 0x00, 0x00, 0x00, 0x00,// (space)// 32
	0x00, 0x00, 0x5F, 0x00, 0x00,// !			 // 33
	0x00, 0x07, 0x00, 0x07, 0x00,// "      // 34
	0x14, 0x7F, 0x14, 0x7F, 0x14,// #      // 35
	0x24, 0x2A, 0x7F, 0x2A, 0x12,// $      // 36
	0x23, 0x13, 0x08, 0x64, 0x62,// %      // 37
	0x36, 0x49, 0x55, 0x22, 0x50,// &      // 38
	0x00, 0x05, 0x03, 0x00, 0x00,// '      // 39
	0x00, 0x1C, 0x22, 0x41, 0x00,// (      // 40
	0x00, 0x41, 0x22, 0x1C, 0x00,// )      // 41
	0x08, 0x2A, 0x1C, 0x2A, 0x08,// *      // 42
	0x08, 0x08, 0x3E, 0x08, 0x08,// +      // 43
	0x00, 0x50, 0x30, 0x00, 0x00,// ,      // 44
	0x08, 0x08, 0x08, 0x08, 0x08,// -      // 45
	0x00, 0x60, 0x60, 0x00, 0x00,// .      // 46
	0x20, 0x10, 0x08, 0x04, 0x02,// /      // 47
	0x3E, 0x51, 0x49, 0x45, 0x3E,// 0      // 48
	0x00, 0x42, 0x7F, 0x40, 0x00,// 1      // 49
	0x42, 0x61, 0x51, 0x49, 0x46,// 2      // 50
	0x21, 0x41, 0x45, 0x4B, 0x31,// 3      // 51
	0x18, 0x14, 0x12, 0x7F, 0x10,// 4      // 52
	0x27, 0x45, 0x45, 0x45, 0x39,// 5      // 53
	0x3C, 0x4A, 0x49, 0x49, 0x30,// 6      // 54
	0x01, 0x71, 0x09, 0x05, 0x03,// 7      // 55
	0x36, 0x49, 0x49, 0x49, 0x36,// 8      // 56
	0x06, 0x49, 0x49, 0x29, 0x1E,// 9      // 57
	0x00, 0x36, 0x36, 0x00, 0x00,// :      // 58
	0x00, 0x56, 0x36, 0x00, 0x00,// ;      // 59
	0x00, 0x08, 0x14, 0x22, 0x41,// <      // 60
	0x14, 0x14, 0x14, 0x14, 0x14,// =      // 61
	0x41, 0x22, 0x14, 0x08, 0x00,// >      // 62
	0x02, 0x01, 0x51, 0x09, 0x06,// ?      // 63
	0x32, 0x49, 0x79, 0x41, 0x3E,// @      // 64
	0x7E, 0x11, 0x11, 0x11, 0x7E,// A      // 65
	0x7F, 0x49, 0x49, 0x49, 0x36,// B      // 66
	0x3E, 0x41, 0x41, 0x41, 0x22,// C      // 67
	0x7F, 0x41, 0x41, 0x22, 0x1C,// D      // 68
	0x7F, 0x49, 0x49, 0x49, 0x41,// E      // 69
	0x7F, 0x09, 0x09, 0x01, 0x01,// F      // 70
	0x3E, 0x41, 0x41, 0x51, 0x32,// G      // 71
	0x7F, 0x08, 0x08, 0x08, 0x7F,// H      // 72
	0x00, 0x41, 0x7F, 0x41, 0x00,// I      // 73
	0x20, 0x40, 0x41, 0x3F, 0x01,// J      // 74
	0x7F, 0x08, 0x14, 0x22, 0x41,// K      // 75
	0x7F, 0x40, 0x40, 0x40, 0x40,// L      // 76
	0x7F, 0x02, 0x04, 0x02, 0x7F,// M      // 77
	0x7F, 0x04, 0x08, 0x10, 0x7F,// N      // 78
	0x3E, 0x41, 0x41, 0x41, 0x3E,// O      // 79
	0x7F, 0x09, 0x09, 0x09, 0x06,// P      // 80
	0x3E, 0x41, 0x51, 0x21, 0x5E,// Q      // 81
	0x7F, 0x09, 0x19, 0x29, 0x46,// R      // 82
	0x46, 0x49, 0x49, 0x49, 0x31,// S      // 83
	0x01, 0x01, 0x7F, 0x01, 0x01,// T      // 84
	0x3F, 0x40, 0x40, 0x40, 0x3F,// U      // 85
	0x1F, 0x20, 0x40, 0x20, 0x1F,// V      // 86
	0x7F, 0x20, 0x18, 0x20, 0x7F,// W      // 87
	0x63, 0x14, 0x08, 0x14, 0x63,// X      // 88
	0x03, 0x04, 0x78, 0x04, 0x03,// Y      // 89
	0x61, 0x51, 0x49, 0x45, 0x43,// Z      // 90
	0x00, 0x00, 0x7F, 0x41, 0x41,// [      // 91
	0x02, 0x04, 0x08, 0x10, 0x20,// "\"    // 92
	0x41, 0x41, 0x7F, 0x00, 0x00,// ]      // 93
	0x04, 0x02, 0x01, 0x02, 0x04,// ^      // 94
	0x40, 0x40, 0x40, 0x40, 0x40,// _      // 95
	0x00, 0x01, 0x02, 0x04, 0x00,// `      // 96
	0x20, 0x54, 0x54, 0x54, 0x78,// a      // 97
	0x7F, 0x48, 0x44, 0x44, 0x38,// b      // 98
	0x38, 0x44, 0x44, 0x44, 0x20,// c      // 99
	0x38, 0x44, 0x44, 0x48, 0x7F,// d      // 100
	0x38, 0x54, 0x54, 0x54, 0x18,// e      // 101
	0x08, 0x7E, 0x09, 0x01, 0x02,// f      // 102
	0x08, 0x14, 0x54, 0x54, 0x3C,// g      // 103
	0x7F, 0x08, 0x04, 0x04, 0x78,// h      // 104
	0x00, 0x44, 0x7D, 0x40, 0x00,// i      // 105
	0x20, 0x40, 0x44, 0x3D, 0x00,// j      // 106
	0x00, 0x7F, 0x10, 0x28, 0x44,// k      // 107
	0x00, 0x41, 0x7F, 0x40, 0x00,// l      // 108
	0x7C, 0x04, 0x18, 0x04, 0x78,// m      // 109
	0x7C, 0x08, 0x04, 0x04, 0x78,// n      // 110
	0x38, 0x44, 0x44, 0x44, 0x38,// o      // 111
	0x7C, 0x14, 0x14, 0x14, 0x08,// p      // 112
	0x08, 0x14, 0x14, 0x18, 0x7C,// q      // 113
	0x7C, 0x08, 0x04, 0x04, 0x08,// r      // 114
	0x48, 0x54, 0x54, 0x54, 0x20,// s      // 115
	0x04, 0x3F, 0x44, 0x40, 0x20,// t      // 116
	0x3C, 0x40, 0x40, 0x20, 0x7C,// u      // 117
	0x1C, 0x20, 0x40, 0x20, 0x1C,// v      // 118
	0x3C, 0x40, 0x30, 0x40, 0x3C,// w      // 119
	0x44, 0x28, 0x10, 0x28, 0x44,// x      // 120
	0x0C, 0x50, 0x50, 0x50, 0x3C,// y      // 121
	0x44, 0x64, 0x54, 0x4C, 0x44,// z      // 122
	0x00, 0x08, 0x36, 0x41, 0x00,// {      // 123
	0x00, 0x00, 0x7F, 0x00, 0x00,// |      // 124
	0x00, 0x41, 0x36, 0x08, 0x00,// }      // 125
	0x08, 0x08, 0x2A, 0x1C, 0x08,// ->     // 126
	0x08, 0x1C, 0x2A, 0x08, 0x08 // <-     // 127
	
};


#endif
