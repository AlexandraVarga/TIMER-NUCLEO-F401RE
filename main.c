#include "tftlcdlib.h"
#include "stm32f4xx.h"

void SystemCoreClockSetHSI(void) {

  RCC->CR |= ((uint32_t)RCC_CR_HSION);                     // Enable HSI
  while ((RCC->CR & RCC_CR_HSIRDY) == 0);                  // Wait for HSI Ready

  RCC->CFGR = RCC_CFGR_SW_HSI;                             // HSI is system clock
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);  // Wait for HSI used as system clock

  FLASH->ACR  = FLASH_ACR_PRFTEN;                          // Enable Prefetch Buffer
  FLASH->ACR |= FLASH_ACR_ICEN;                            // Instruction cache enable
  FLASH->ACR |= FLASH_ACR_DCEN;                            // Data cache enable
  FLASH->ACR |= FLASH_ACR_LATENCY_5WS;                     // Flash 5 wait state

  RCC->CFGR |= RCC_CFGR_HPRE_DIV1;                         // HCLK = SYSCLK/2
  RCC->CFGR |= RCC_CFGR_PPRE1_DIV1;                        // APB1 = HCLK
  RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;                        // APB2 = HCLK

  RCC->CR &= ~RCC_CR_PLLON;                                // Disable PLL

  // PLL configuration:  VCO = HSI/M * N,  Sysclk = VCO/P
  RCC->PLLCFGR = ( 16ul                   |                // PLL_M =  16
                 (192ul <<  6)            |                // PLL_N = 192
                 (  1ul << 16)            |                // PLL_P =   8
                 (RCC_PLLCFGR_PLLSRC_HSI) |                // PLL_SRC = HSI
                 (  3ul << 24)             );              // PLL_Q =   8

  RCC->CR |= RCC_CR_PLLON;                                 // Enable PLL
  while((RCC->CR & RCC_CR_PLLRDY) == 0) __NOP();           // Wait till PLL is ready

  RCC->CFGR &= ~RCC_CFGR_SW;                               // Select PLL as system clock source
  RCC->CFGR |=  RCC_CFGR_SW_PLL;
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);  // Wait till PLL is system clock src
}

void TIM_Init(){

RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;	
		
	TIM2->PSC = 200; 
	TIM2->ARR = 3000;
	
	
	NVIC_EnableIRQ(TIM2_IRQn); 
	TIM2->DIER |= TIM_DIER_UIE;
	TIM2->CR1 |=TIM_CR1_CEN;
	TIM2->SR &= ~TIM_SR_UIF;
	
     
}
uint16_t g=0;
void TIM2_IRQHandler() {
		TIM2->SR &= ~TIM_SR_UIF;
		g++;
		
}


void LCD_Write_Number_dec(uint16_t x, uint16_t y, uint16_t value,uint16_t numberOfChars){

	uint16_t tmp;
	
	for(int i=0; i<numberOfChars;i++){
		
		tmp= value%10;
		value = value/10;
		LCD_WriteChar(x+18-i*6,y,tmp+48,YELLOW,BLACK);
	
	}
}

void LCD_Write_Number_hex(uint16_t x, uint16_t y, uint16_t value){
	uint32_t remainder;
	uint16_t tmp = value;
	uint16_t i=0;
	    while (tmp != 0)
			{
        remainder = tmp % 16;

        if (remainder < 10)
            LCD_WriteChar(x+18-i*6,y,48 + remainder,YELLOW,BLACK);
				else
            LCD_WriteChar(x+18-i*6,y,55 + remainder,YELLOW,BLACK);
				i++;
        tmp = tmp / 16;

    }

}

uint16_t len=0;

void LCD_Write_String(uint16_t x, uint16_t y, unsigned char *str){
	
	for(int i=0;i<len;i++)
	LCD_WriteChar(x+i*6,y,str[i],BLUE,BLACK);

}


int main(){
		
	SystemCoreClockSetHSI();
	SystemCoreClockUpdate();
	TIM_Init();
	
	GPIO_Init();
	ADC_Init();	
	
	LCD_Init(BLACK);
	
	unsigned char str1[] = {'D','i','s','p','l','a','y',' ','I','D',':'};
	len = sizeof(str1)/sizeof(*str1);
	LCD_Write_String(10,10,str1);
	
	unsigned char str2[] = {'p','r','e','s','s','u','r','e',':'};
	len = sizeof(str2)/sizeof(*str2);
	LCD_Write_String(10,290,str2);
	
	unsigned char str3[] = {'t','o','u','c','h',' ','X',':'};
	len = sizeof(str3)/sizeof(*str3);
	LCD_Write_String(10,300,str3);
		
	unsigned char str4[] = {'t','o','u','c','h',' ','Y',':'};
	len = sizeof(str4)/sizeof(*str4);
	LCD_Write_String(10,310,str4);

	uint16_t tmp = LCD_ReadID();
	LCD_Write_Number_hex(80,10,tmp);
	
	uint16_t clr = 0xF800;
	LCD_FilledCircle(120,160,100,TEAL);
	LCD_FilledCircle(120,160,90,BLACK);
	LCD_FilledCircle(120,160,2,BLUE);
	LCD_Circle(120,160,100,BLUE);
	LCD_Circle(120,160,90,BLUE);
	LCD_WriteChar(115,62,'1',clr,TEAL);
	LCD_WriteChar(121,62,'2',clr,TEAL);
	LCD_WriteChar(118,252,'6',clr,TEAL);
	LCD_WriteChar(23,157,'9',clr,TEAL);
	LCD_WriteChar(213,157,'3',clr,TEAL);

	int16_t angle=180;
	
	uint16_t x,y,z;
	uint16_t p=0;
	while(1){
		
		if(g>2){
			angle=(angle-6)%360;
			LCD_Line(120,160,87,angle+6,BLACK);
			LCD_Line(120,160,87,angle,RED);
			LCD_FilledCircle(120,160,2,BLUE);
			g=0;
		}
		
		LCD_GetPoint(&x,&y,&z);

		if(p>1000){
			
			LCD_Write_Number_dec(59,300,x,3);
			LCD_Write_Number_dec(59,310,y,3);
			LCD_Write_Number_dec(65,290,z,4);
					
			p=0;
		}
		for(int i =0;i<LCD_DELAY;i++){}
		p++;
	}
}
