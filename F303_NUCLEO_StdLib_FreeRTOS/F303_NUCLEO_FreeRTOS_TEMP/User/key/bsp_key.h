#ifndef _BSP_KEY_H
#define	_BSP_KEY_H

#define 	KEY1_GPIO_PORT 		GPIOC
#define		KEY1_GPIO_PIN		GPIO_Pin_13

#include "stm32f30x.h"

void Key_GPIO_Config(void);
uint8_t Key_Scan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin);

#endif 
