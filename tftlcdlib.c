#include "tftlcdlib.h"

void GPIO_Init(void){
// sets the outputs for control and data transfer for the display 
	
	RCC_GPIO_CLK_enable();
	
	GPIOA->OSPEEDR |= 0xFFFFFFFF;
	GPIOB->OSPEEDR |= 0xFFFFFFFF;
	GPIOC->OSPEEDR |= 0xFFFFFFFF;

	GPIOA->MODER &= 0xFFC0FCF0;
	GPIOB->MODER &= 0xFFCFF03C;
	GPIOC->MODER &= 0xFFFF3FF3;
		
	GPIOA->MODER |= 0x00150105;
	GPIOB->MODER |= 0x00100541;
	GPIOC->MODER |= 0x00004004;
	
}

void LCD_GPIOSetIN(){
// sets the data interface pins to inputs for reading data from the display 
	
	GPIOA->MODER &= 0xFFC0FFFF;
	GPIOB->MODER &= 0xFFCFF03F;
	GPIOC->MODER &= 0xFFFF3FFF;

}

void LCD_GPIOSetOUT(){
// sets the data interface pins to the outputs for writing to the display  
	
	GPIOA->MODER &= 0xFFC0FFFF;
	GPIOB->MODER &= 0xFFCFF03F;
	GPIOC->MODER &= 0xFFFF3FFF;
		
	GPIOA->MODER |= 0x00150000;
	GPIOB->MODER |= 0x00100540;
	GPIOC->MODER |= 0x00004000;

}

void ADC_Init(void){
	//sets the parameters of the AD converter for touch reading 
	
	ADC1->CR1 &= ~(ADC_CR1_RES_1|ADC_CR1_RES_0); // 10-bit resolution
	ADC1->CR1 &= ~(ADC_CR1_DISCNUM_0|ADC_CR1_DISCNUM_1|ADC_CR1_DISCNUM_2);
	ADC1->CR1 |= ADC_CR1_SCAN | ADC_CR1_DISCEN; // scan mode and regular discontinuous mode
	ADC1->CR1 &= ~(ADC_CR1_JDISCEN);
	ADC1->CR2 &= ~(ADC_CR2_ALIGN);
	ADC1->SMPR2 &= ~(ADC_SMPR2_SMP0_Msk);
	ADC1->SMPR2 |= ADC_SMPR2_SMP0_1; // 28 cycles, sampling time slection
	ADC1->SQR1 &= ~ADC_SQR1_L;
	ADC->CCR &= ~(ADC_CCR_VBATE|ADC_CCR_TSVREFE);
	ADC->CCR &= ~(ADC_CCR_ADCPRE);
		
}


void DrawMode(){
	//GPIOs are configured to write commands and data to the display 
	
	ADC1->CR2 &= ~ADC_CR2_ADON; // Disable the ADC
	
	GPIOA->OSPEEDR = 0xFFFFFFFF;
	GPIOB->OSPEEDR = 0xFFFFFFFF;
	GPIOC->OSPEEDR = 0xFFFFFFFF;
	
	GPIOA->MODER &= 0xFFC0FCF0;
	GPIOB->MODER &= 0xFFCFF03C;
	GPIOC->MODER &= 0xFFFF3FFF;
		
	GPIOA->MODER |= 0x00150105;
	GPIOB->MODER |= 0x00100541;
	GPIOC->MODER |= 0x00004000;
	
	wr_idle();
	rs_data();
	rd_idle();
	
}

void TouchMode(){
	// GPIOs and other parameters are configured to read shared pins for touch sensing
	
	cs_idle();
	
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
	
	GPIOA->OSPEEDR &= 0xFFFCFCF3;
	GPIOB->OSPEEDR &= 0xFFCFFFFF;

	LCD_GPIOSetIN();
	
	ADC1->CR2 |= ADC_CR2_ADON;
	ADC1->CR2 |= ADC_CR2_ADON;
}

void GPIO_ADCx(){
	// GPIOs are configured to read X coordinate 
	
	GPIOA->MODER &= 0xFFC0FCF0;
	GPIOB->MODER &= 0xFFCFF03C;
	
	GPIOA->MODER |= 0x00010304; // PA1 - OUTPUT, PA4 - ANALOG, PA8 - OUTPUT
	GPIOB->MODER |= 0x00000000; // PB10 - INPUT
		
	GPIOA->BSRR |= 1<<8;
	GPIOA->BSRR |= 1<<17;
	
	ADC1->SQR3 &= ~ADC_SQR3_SQ1;
	ADC1->SQR3 |= ADC_SQR3_SQ1_0; // ADC_IN1
	
}

void GPIO_ADCy(){
	// GPIOs are configured to read Y coordinate
	
	GPIOA->MODER &= 0xFFC0FCF0;
	GPIOB->MODER &= 0xFFCFF03C;
	
	GPIOA->MODER |= 0x0000010C; // PA1 - ANALOG, PA4 - OUTPUT, PA8 - INPUT
	GPIOB->MODER |= 0x00100000; // PB10 - OUTPUT
	
	GPIOA->BSRR |= 1<<3;
	GPIOB->BSRR |= 1<<26;
	
	ADC1->SQR3 &= ~ADC_SQR3_SQ1;
	ADC1->SQR3 |= ADC_SQR3_SQ1_2; // ADC_IN4

}

void GPIO_ADCpress(){
	// GPIOs are configured to read pressure data
	
	GPIOA->MODER &= 0xFFC0FCF0;
	GPIOB->MODER &= 0xFFCFF03C;
	
	GPIOA->MODER |= 0x0001030C; // PA1 - ANALOG, PA4 - ANALOG, PA8 - OUTPUT
	GPIOB->MODER |= 0x00100000; // PB10 - OUTPUT
	
	GPIOA->BSRR |= 1<<24;
	GPIOB->BSRR |= 1<<10;

}

uint16_t TouchArea(int16_t x, uint16_t max){
	// checks whether the read value does not exceed the display border
	
	if(x<0){
		return 0;
	}
	if(x>max){
		return max;
	}
	
	return (uint16_t)x;

}

void LCD_GetPoint(uint16_t* x, uint16_t* y, uint16_t* z){
	// stores the read ADC values in the inserted addresses of the variables x, y, z 
	
	TouchMode();
	GPIO_ADCx();
	x[0] = TouchArea((int16_t)((1.0f-(ReadADC()-X_MIN)*adc_x)*LCD_WIDTH),LCD_WIDTH);
	GPIO_ADCy();
	y[0] = TouchArea((int16_t)((1.0f-(ReadADC()-Y_MIN)*adc_y)*LCD_HEIGHT),LCD_HEIGHT);
	GPIO_ADCpress();
	z[0]=ReadPressure();
	
	DrawMode();
}

uint16_t ReadPressure(){
	// returns higher values by touching the display 
	
	ADC1->SQR3 &= ~ADC_SQR3_SQ1;
	ADC1->SQR3 |= ADC_SQR3_SQ1_0; // ADC_IN1
	uint16_t x1 = ReadADC();
	
	ADC1->SQR3 &= ~ADC_SQR3_SQ1;
	ADC1->SQR3 |= ADC_SQR3_SQ1_2; // ADC_IN4
	uint16_t y1 = ReadADC();

	int16_t pressure = ADC_MAX+(x1-y1);
	if(pressure<0){
		return -pressure;
	}
	else{
		return pressure;
	}

}

uint16_t ReadADC(){
	// calculates the average of the read ADC values
	// for multiple samples it calculates the median 

	uint16_t avg_value;
	
	if(NUMBEROFSAMPLES<3){
		for(int i=0;i<NUMBEROFSAMPLES;i++){
		
			avg_value += ADC_Value();
		
		}
		
		return avg_value/NUMBEROFSAMPLES;
	}
	else{
		
		uint16_t adc_value[NUMBEROFSAMPLES];
		uint16_t tmp;
		for(int i=0;i<NUMBEROFSAMPLES;i++){
		
			adc_value[i] = ADC_Value();
		
		}
		
		for(int i = 0;i < NUMBEROFSAMPLES-1;i++) {
      for(int j = 0;j < NUMBEROFSAMPLES-i-1;j++) {
         if(adc_value[j] > adc_value[j+1]){
					 tmp = adc_value[j];
				   adc_value[j] = adc_value[j+1];
           adc_value[j+1] = tmp;
				}
			}
		}
		
		return adc_value[NUMBEROFSAMPLES/2];
		
	}	
}

uint16_t ADC_Value(){
	// it only returns the ADC value 
	
	ADC1->CR2 |= ADC_CR2_SWSTART; // SWSTART
	
	while(!(ADC1->SR&ADC_SR_EOC));
	uint16_t tmp = ADC1->DR;
	return tmp;
	
}

uint16_t LCD_ReadFrameMemory(uint16_t x, uint16_t y){
	// reads the color stored in the display memory for the specified pixel 
	
	LCD_WriteCommand8b(0x20);
	LCD_WriteData8b(x >> 8);
	LCD_WriteData8b(x);
	
	
	LCD_WriteCommand8b(0x21);
	
	LCD_WriteData8b(y >> 8);
	LCD_WriteData8b(y);
	
	LCD_WriteCommand8b(0x22);
	
	
	LCD_GPIOSetIN();
	cs_active();
	rs_data();
	rd_idle();
	wr_idle();
		
	Read_8b();
	Read_8b(); //dummy
	uint16_t datahigh = Read_8b()<<8;
	uint16_t datalow = Read_8b();
	
	uint16_t data = datahigh|datalow;
	LCD_GPIOSetOUT();
	return data;
}

uint8_t Read_8b(){
// reads 8 bits from the parallel display interface 
	
	rd_active();
	
	LCD_Wait(1);
	
	uint8_t data = ((GPIOA->IDR&0x0100)>>1)|((GPIOA->IDR&0x0400)>>8)|((GPIOA->IDR&0x0200)>>9)\
	| ((GPIOB->IDR&0x0400)>>4)|((GPIOB->IDR&0x0010)<<1)|((GPIOB->IDR&0x0020)>>1)|(GPIOB->IDR&0x0008)\
	| ((GPIOC->IDR&0x0080)>>6);

	LCD_Wait(1);
	
	rd_idle();

	return data;
}

void ReadContinuousData(uint16_t command, uint16_t *data, uint16_t numberOfBytes){
// reads continuous 8 bits data after writing the command
	LCD_WriteCommand8b(command>>8);
	LCD_WriteCommand8b(command);
	
	LCD_GPIOSetIN();
	cs_active();
	rs_data();
	rd_idle();
	wr_idle();

	for(int i=0;i<numberOfBytes;i++)
	data[i] = Read_8b();
	
	cs_idle();
	LCD_GPIOSetOUT();
	
}

uint16_t Read_Data16b(uint16_t command){
	// reads 16 bits of data for the inserted command
	LCD_WriteCommand8b(command>>8);
	LCD_WriteCommand8b(command);
	
	LCD_GPIOSetIN();
	
	cs_active();
	rs_data();
	rd_idle();
	wr_idle();
	
	uint16_t datahigh = Read_8b()<<8;
	uint16_t datalow = Read_8b();
	
	uint16_t data = datahigh|datalow;
	
	
	cs_idle();
	LCD_GPIOSetOUT();
	
	return data;
}

uint16_t LCD_ReadID(){
// returns the display ID for its identification
	
	uint16_t displayID = Read_Data16b(0x0000);
	
return displayID;
}

void LCD_DrawPixel(uint16_t x, uint16_t y, uint16_t color){
	// renders one pixel 
	
	cs_active();
	
	LCD_WriteCommand8b(0x20);
	LCD_WriteData8b(x >> 8);
	LCD_WriteData8b(x);
	
	LCD_WriteCommand8b(0x21);
	LCD_WriteData8b(y >> 8);
	LCD_WriteData8b(y);
	
	LCD_WriteCommand8b(0x22);
	LCD_WriteData8b(color >> 8);
	LCD_WriteData8b(color);
	
	cs_idle();
	
}

void LCD_SetAddressWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
	// selects an array in the display memory to be written to using LCD_WR 
  cs_active();
	
    uint16_t x, y;
    
      x  = x1;
      y  = y1;
      
		//address window
    LCD_WriteCommand8b(0x50); LCD_WriteData8b(x1 >> 8); LCD_WriteData8b(x1);
    LCD_WriteCommand8b(0x51); LCD_WriteData8b(x2 >> 8); LCD_WriteData8b(x2);
    LCD_WriteCommand8b(0x52); LCD_WriteData8b(y1 >> 8); LCD_WriteData8b(y1);
    LCD_WriteCommand8b(0x53); LCD_WriteData8b(y2 >> 8); LCD_WriteData8b(y2);
		
		//address counter
    LCD_WriteCommand8b(0x20); LCD_WriteData8b(x >> 8); LCD_WriteData8b(x);
    LCD_WriteCommand8b(0x21); LCD_WriteData8b(y >> 8); LCD_WriteData8b(y);

	cs_idle();

}

void FastWrite(uint8_t colorhigh, uint8_t colorlow, uint32_t numberOfEntries){
// If the lower part of the byte is the same as the upper one, writing is 
// simplified by using this function 
	
	LCD_WriteCommand8b(0x22);
		
	if(colorhigh==colorlow){
		
		LCD_WriteData8b(colorhigh);
		LCD_WriteData8b(colorlow);
		for(int i = 0;i<numberOfEntries-1;i++){	
		
			ConfirmEntry();
			ConfirmEntry();
			
		}
	}
	else
	{
		for(int i = 0;i<numberOfEntries;i++){	
		
			LCD_WriteData8b(colorhigh);
			LCD_WriteData8b(colorlow);
		}
	}

}


void LCD_DisplayFill(uint16_t color){
	// fills the display with the selected color 
	
	cs_active();

	uint8_t col_l = color;
	uint8_t col_h = color >> 8;
	  
	LCD_SetAddressWindow(0,0,LCD_WIDTH-1,LCD_HEIGHT-1);
		
	FastWrite(col_h,col_l,LCD_WIDTH*LCD_HEIGHT);
		
	cs_idle();
	
}

void LCD_FilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color){
	// fills the rectangular area with the selected color 
	
	cs_active();
	
	uint8_t col_l = color;
	uint8_t col_h = color >> 8;
	
	LCD_SetAddressWindow(x,y,x+w-1,y+h-1);
	
	FastWrite(col_h,col_l,w*h);
	
	LCD_SetAddressWindow(0,0,LCD_WIDTH-1,LCD_HEIGHT-1);
	
	cs_idle();
	
}

void LCD_Rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color){
// draws a rectangle border 
	
	cs_active();
	
	uint8_t col_l = color;
	uint8_t col_h = color >> 8;
	
	LCD_SetAddressWindow(x,y,x+w-1,y);
	
	FastWrite(col_h,col_l,w);
	
	LCD_SetAddressWindow(x,y,x,y+h-1);
	
	FastWrite(col_h,col_l,h);
	
	LCD_SetAddressWindow(x,y+h-1,x+w-1,y+h-1);
	
	FastWrite(col_h,col_l,w);
	
	LCD_SetAddressWindow(x+w-1,y,x+w-1,y+h-1);
	
	FastWrite(col_h,col_l,h);
		
	LCD_SetAddressWindow(0,0,LCD_WIDTH-1,LCD_HEIGHT-1);

	cs_idle();
	
}

void LCD_FilledCircle(uint16_t x, uint16_t y, uint16_t r, uint16_t color){
// draws a fill circle 
	
	cs_active();
	
	if (r<1) return;
	LCD_HorizontalLine(x, y - r, 2*r+1, color);
	
	int16_t d2x = 1;
  int16_t d2y = - 2*r;
  int16_t xp = 0;
	
	int16_t f = 1 - r;


  while (xp < r) {
    if (f >= 0) {
      r--;
      d2y += 2;
      f += d2y;
    }
    xp++;
    d2x += 2;
    f += d2x;
		LCD_HorizontalLine(x + xp, y - r, 2*r + 1, color);
    LCD_HorizontalLine(x + r, y - xp, 2*xp + 1, color);
		LCD_HorizontalLine(x - xp, y - r, 2*r + 1, color);
    LCD_HorizontalLine(x - r, y - xp, 2*xp + 1, color);
    
  }
	
	cs_idle();
}
	
	
void LCD_Circle(uint16_t x, uint16_t y, uint16_t r, uint16_t color){
// draws a circle
	cs_active();
	
  if (r<1) return;
  int16_t d2x = 1;
  int16_t d2y = - 2*r;
  int16_t xp = 0;
	
	int16_t f = 1 - r;

  LCD_DrawPixel(x + r, y, color);
  LCD_DrawPixel(x - r, y, color);
  LCD_DrawPixel(x, y - r, color);
  LCD_DrawPixel(x, y + r, color);

  while (xp < r) {
    if (f >= 0) {
      r--;
      d2y += 2;
      f += d2y;
    }
    xp++;
    d2x += 2;
    f += d2x;

    LCD_DrawPixel(x + xp, y + r, color);
    LCD_DrawPixel(x - xp, y + r, color);
    LCD_DrawPixel(x - xp, y - r, color);
    LCD_DrawPixel(x + xp, y - r, color);

    LCD_DrawPixel(x + r, y + xp, color);
    LCD_DrawPixel(x - r, y + xp, color);
    LCD_DrawPixel(x - r, y - xp, color);
    LCD_DrawPixel(x + r, y - xp, color);
  }

	cs_idle();

}

void LCD_Ellipse(uint16_t x, uint16_t y, uint16_t rx, uint16_t ry, uint16_t color){
// draws an ellipse
	
	cs_active();
	
	if (rx<2) return;
  if (ry<2) return;
  int16_t xp, yp;
  int32_t rx2 = rx * rx;
  int32_t ry2 = ry * ry;
  int32_t fx2 = 4 * rx2;
  int32_t fy2 = 4 * ry2;
  int32_t s;

  for (xp = 0, yp = ry, s = 2*ry2+rx2*(1-2*ry); ry2*xp <= rx2*yp; xp++)
  {
    LCD_DrawPixel(x + xp, y + yp, color);
    LCD_DrawPixel(x - xp, y + yp, color);
    LCD_DrawPixel(x - xp, y - yp, color);
    LCD_DrawPixel(x + xp, y - yp, color);
    if (s >= 0)
    {
      s += fx2 * (1 - yp);
      yp--;
    }
    s += ry2 * ((4 * xp) + 6);
  }

  for (xp = rx, yp = 0, s = 2*rx2+ry2*(1-2*rx); rx2*yp <= ry2*xp; yp++)
  {
    LCD_DrawPixel(x + xp, y + yp, color);
    LCD_DrawPixel(x - xp, y + yp, color);
    LCD_DrawPixel(x - xp, y - yp, color);
    LCD_DrawPixel(x + xp, y - yp, color);
    if (s >= 0)
    {
      s += fy2 * (1 - xp);
      xp--;
    }
    s += rx2 * ((4 * yp) + 6);
  }

	cs_idle();
	
}
	

void LCD_FilledEllipse(uint16_t x, uint16_t y, uint16_t rx, uint16_t ry, uint16_t color){
// draws a filled ellipse 
	
	cs_active();
	
	if (rx<2) return;
  if (ry<2) return;
  int16_t xp, yp;
  int32_t rx2 = rx * rx;
  int32_t ry2 = ry * ry;
  int32_t fx2 = 4 * rx2;
  int32_t fy2 = 4 * ry2;
  int32_t s;

  for (xp = 0, yp = ry, s = 2*ry2+rx2*(1-2*ry); ry2*xp <= rx2*yp; xp++)
  {
    LCD_VerticalLine(x - xp, y - yp, 2*xp + 1, color);
    LCD_VerticalLine(x - xp, y + yp, 2*xp + 1, color);
    if (s >= 0)
    {
      s += fx2 * (1 - yp);
      yp--;
    }
    s += ry2 * ((4 * xp) + 6);
  }

  for (xp = rx, yp = 0, s = 2*rx2+ry2*(1-2*rx); rx2*yp <= ry2*xp; yp++)
  {
    
		LCD_VerticalLine(x - xp, y - yp, 2*xp + 1, color);
    LCD_VerticalLine(x - xp, y + yp, 2*xp + 1, color);
    if (s >= 0)
    {
      s += fy2 * (1 - xp);
      xp--;
    }
    s += rx2 * ((4 * yp) + 6);
  }

	cs_idle();

}
	
void LCD_HorizontalLine(uint16_t x, uint16_t y, uint16_t len, uint16_t color){
// draws a horizontal line 
	
	cs_active();
	
	uint8_t col_l = color;
	uint8_t col_h = color >> 8;
	LCD_SetAddressWindow(x,y,x,y+len-1);
		
	FastWrite(col_h,col_l,len);
	
	LCD_SetAddressWindow(0,0,LCD_WIDTH-1,LCD_HEIGHT-1);
	
	cs_idle();
	
}


void LCD_VerticalLine(uint16_t x, uint16_t y, uint16_t len, uint16_t color){
// draws a vertical line 
	
	cs_active();
	
	uint8_t col_l = color;
	uint8_t col_h = color >> 8;
	LCD_SetAddressWindow(x,y,x+len-1,y);
		
	FastWrite(col_h,col_l,len);
	
	LCD_SetAddressWindow(0,0,LCD_WIDTH-1,LCD_HEIGHT-1);

	cs_idle();
	
}

void LCD_Line(uint16_t x, uint16_t y, uint16_t len, int16_t rotation, uint16_t color){
// draws a line rotated by a specified angle 
	if(len<1) return;
	
	cs_active();
	
	LCD_DrawPixel(x,y,color);

	double x_tmp = x;
	double y_tmp = y;
	
	for(int i=0;i<len;i++){
		
	x_tmp = x_tmp + sin(rotation*PI/180.0);
	y_tmp = y_tmp + cos(rotation*PI/180.0);

	LCD_DrawPixel(round(x_tmp),round(y_tmp),color);
	}

	cs_idle();
	
}

void ClearChar(uint16_t x, uint16_t y, uint16_t background_color){
	// fills the background area behind a typed character 
	
	if(TEXT_ROTATION){
		LCD_FilledRectangle(x, y, FONT_WIDTH, FONT_HEIGHT, background_color);
	}
	else{
		LCD_FilledRectangle(x, y, FONT_HEIGHT, FONT_WIDTH, background_color);	
	}

}


void LCD_WriteChar(uint16_t x, uint16_t y, uint8_t ch, uint16_t color, uint16_t background_color){
// prints a character from the available font to the display 
	
	ClearChar(x,y,background_color);
	uint8_t tmp;
	
	for(int col=0;col<FONT_WIDTH;col++){
		
		uint8_t mask = 0x01;
		
		for(int row=0;row<FONT_HEIGHT;row++){
			
			tmp = font5x7[FONT_WIDTH*(ch-32)+col];
			
			if(tmp&(mask<<row)) {
				
				if(TEXT_ROTATION){
					LCD_DrawPixel(x+col,y+row,color);
				}
				else{
					LCD_DrawPixel(x+FONT_HEIGHT-row,y+col,color);
				}
				
			}
		}
	}
}


uint16_t RGB(uint8_t red, uint8_t green, uint8_t blue){
	// converts 565 RGB color to RGB(0-255,0-255,0-255) 
	
	uint16_t color = (2048*((uint16_t)round(31.0*(red/255.0)))) | (32*((uint16_t)round(63.0*(green/255.0)))) | ((uint16_t)round(31.0*(blue/255.0)));

	return color;
}


void ConfirmEntry(void){
	// sends data on the parallel 8-bit interface to the display 
	
	wr_active();
	wr_idle();
	
}

void DisplayReset(void){
 // resets the display to initialize 
	
	
	cs_idle(); 
	rs_data();
	wr_idle();
	rd_idle();
	rst_reset();
	
	LCD_Wait(20);
	
	rst_active();
	
	LCD_Wait(150);
	
	cs_active();
	
	LCD_WriteCommand8b(0x00);
	
	for(int i=0;i<3;i++) ConfirmEntry();
	
}


void LCD_WriteData8b(uint8_t data){
	// sets the GPIO pins and sends them to the display 
	cs_active();
	
	GPIOA->ODR = (GPIOA->ODR & 0b1111100011111111)\
	| ((data & 1) << 9) | ((data & 0x04) << 8) | ((data & 0x80) << 1);
	
	GPIOB->ODR = (GPIOB->ODR & 0b1111101111000111)\
	| (data & 0x08) | ((data & 0x10) << 1) | ((data & 0x20) >> 1) | ((data & 0x40) << 4);
	
	GPIOC->ODR = (GPIOC->ODR & 0b1111111101111111)\
	| ((data & 0x02) << 6);
		
	ConfirmEntry();

}


void LCD_WriteCommand8b(uint8_t data){
// sends a command over the parallel interface 
	
	rs_command();
	LCD_WriteData8b(data);
	rs_data();

}

void LCD_Wait(uint16_t delay){
// waits several processor cycles 
	
	for(int i=0; i<= delay; i++){
		for(int j=0; j <= LCD_DELAY; j++){}
	}
}

void LCD_Init(uint16_t color){
	// launches the display 
	
	DisplayReset();
	LCD_Wait(150);

	cs_active();
	LCD_Wait(150);
    
		LCD_WriteCommand8b(0x01);LCD_WriteData8b(0x01);LCD_WriteData8b(0x00);
		LCD_WriteCommand8b(0x02);LCD_WriteData8b(0x07);LCD_WriteData8b(0x00);
		LCD_WriteCommand8b(0x03);LCD_WriteData8b(0x10);LCD_WriteData8b(0x30);
		LCD_WriteCommand8b(0x08);LCD_WriteData8b(0x03);LCD_WriteData8b(0x02);
		LCD_WriteCommand8b(0x09);LCD_WriteData8b(0x00);LCD_WriteData8b(0x00);
		LCD_WriteCommand8b(0x0A);LCD_WriteData8b(0x00);LCD_WriteData8b(0x08);
		
		//power control settings
		LCD_WriteCommand8b(0x10);LCD_WriteData8b(0x07);LCD_WriteData8b(0x90);
		LCD_WriteCommand8b(0x11);LCD_WriteData8b(0x00);LCD_WriteData8b(0x05);
		LCD_WriteCommand8b(0x12);LCD_WriteData8b(0x00);LCD_WriteData8b(0x00);
		LCD_WriteCommand8b(0x13);LCD_WriteData8b(0x00);LCD_WriteData8b(0x00);

	LCD_Wait(150);
		// power supply settings
		LCD_WriteCommand8b(0x10);LCD_WriteData8b(0x12);LCD_WriteData8b(0xB0);

		LCD_WriteCommand8b(0x11);LCD_WriteData8b(0x00);LCD_WriteData8b(0x07);

 LCD_Wait(150);

		LCD_WriteCommand8b(0x12);LCD_WriteData8b(0x00);LCD_WriteData8b(0x8C);
		LCD_WriteCommand8b(0x13);LCD_WriteData8b(0x17);LCD_WriteData8b(0x00);
		LCD_WriteCommand8b(0x29);LCD_WriteData8b(0x00);LCD_WriteData8b(0x22);

	LCD_Wait(150);

		//gamma parameters
		LCD_WriteCommand8b(0x30);LCD_WriteData8b(0x00);LCD_WriteData8b(0x00);
		LCD_WriteCommand8b(0x31);LCD_WriteData8b(0x05);LCD_WriteData8b(0x05);
		LCD_WriteCommand8b(0x32);LCD_WriteData8b(0x02);LCD_WriteData8b(0x05);
		LCD_WriteCommand8b(0x35);LCD_WriteData8b(0x02);LCD_WriteData8b(0x06);
		LCD_WriteCommand8b(0x36);LCD_WriteData8b(0x04);LCD_WriteData8b(0x08);
		LCD_WriteCommand8b(0x37);LCD_WriteData8b(0x00);LCD_WriteData8b(0x00);
		LCD_WriteCommand8b(0x38);LCD_WriteData8b(0x05);LCD_WriteData8b(0x04);
		LCD_WriteCommand8b(0x39);LCD_WriteData8b(0x02);LCD_WriteData8b(0x06);
		LCD_WriteCommand8b(0x3C);LCD_WriteData8b(0x02);LCD_WriteData8b(0x06);
		LCD_WriteCommand8b(0x3D);LCD_WriteData8b(0x04);LCD_WriteData8b(0x08);

		LCD_WriteCommand8b(0x50);LCD_WriteData8b(0x00);LCD_WriteData8b(0x00);
		LCD_WriteCommand8b(0x51);LCD_WriteData8b(0x00);LCD_WriteData8b(0xEF);
		LCD_WriteCommand8b(0x52);LCD_WriteData8b(0x00);LCD_WriteData8b(0x00);
		LCD_WriteCommand8b(0x53);LCD_WriteData8b(0x01);LCD_WriteData8b(0x3F);

		LCD_WriteCommand8b(0x60);LCD_WriteData8b(0xA7);LCD_WriteData8b(0x00);
		LCD_WriteCommand8b(0x61);LCD_WriteData8b(0x00);LCD_WriteData8b(0x01);
		LCD_WriteCommand8b(0x90);LCD_WriteData8b(0x00);LCD_WriteData8b(0x33); 
		LCD_Wait(150);
		LCD_WriteCommand8b(0x07); LCD_WriteData8b(0x01);LCD_WriteData8b(0x33); //Display ON
		LCD_Wait(150);
   
	LCD_DisplayFill(color);
	
}


															