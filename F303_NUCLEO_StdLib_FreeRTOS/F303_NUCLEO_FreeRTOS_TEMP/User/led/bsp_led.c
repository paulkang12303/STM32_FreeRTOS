
#include "bsp_led.h"
  

void LED_GPIO_Config(void)
{		
	RCC_AHBPeriphClockCmd(LED1_GPIO_CLK, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Pin = LED1_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_1;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	
	GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStruct);
}
