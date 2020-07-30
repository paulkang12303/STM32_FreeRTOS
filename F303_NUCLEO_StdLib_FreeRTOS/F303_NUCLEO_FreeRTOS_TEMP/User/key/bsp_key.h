#ifndef _BSP_KEY_H
#define	_BSP_KEY_H

#include "stm32f30x.h"

#define 	KEY1_GPIO_PORT 		GPIOA
#define		KEY1_GPIO_PIN		GPIO_Pin_0
#define 	KEY1_GPIO_CLK		RCC_AHBPeriph_GPIOA

#define		KEY2_GPIO_PORT 		GPIOA
#define		KEY2_GPIO_PIN		GPIO_Pin_1
#define 	KEY2_GPIO_CLK		RCC_AHBPeriph_GPIOA

#define		KEY_DOWN		0
#define		KEY_UP			1

void Key_GPIO_Config(void);
uint8_t Key_Scan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin);

#endif 
