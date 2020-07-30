#ifndef _BSP_LED_H
#define	_BSP_LED_H

#include "stm32f30x.h"

#define LED1_GPIO_PORT            GPIOA 
#define LED1_GPIO_PIN             GPIO_Pin_5                                      
#define LED1_GPIO_CLK             RCC_AHBPeriph_GPIOA

void LED_GPIO_Config(void);

#endif 
