//in FreeRTOSConfig #define configUSE_MUTEXES		  1 

/* Includes ------------------------------------------------------------------*/
#include "stm32f30x.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_key.h"

static TaskHandle_t AppTaskCreate_Handle = NULL;/* 创建任务句柄 */
static TaskHandle_t LowPriority_Task_Handle = NULL;/* LowPriority_Task任务句柄 */
static TaskHandle_t MidPriority_Task_Handle = NULL;/* MidPriority_Task任务句柄 */
static TaskHandle_t HighPriority_Task_Handle = NULL;/* HighPriority_Task任务句柄 */

SemaphoreHandle_t MuxSem_Handle =NULL;

static void AppTaskCreate(void);/* 用于创建任务 */
static void LowPriority_Task(void* pvParameters);/* LowPriority_Task任务实现 */
static void MidPriority_Task(void* pvParameters);/* MidPriority_Task任务实现 */
static void HighPriority_Task(void* pvParameters);/* MidPriority_Task任务实现 */

static void BSP_Init(void);

int main(void)
{
	BSP_Init();
	BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
  
	/* 创建AppTaskCreate任务 */
	xReturn = xTaskCreate(	(TaskFunction_t )AppTaskCreate,  /* 任务入口函数 */
							(const char*    )"AppTaskCreate",/* 任务名字 */
							(uint16_t       )512,  /* 任务栈大小 */
							(void*          )NULL,/* 任务入口函数参数 */
							(UBaseType_t    )1, /* 任务的优先级 */
							(TaskHandle_t*  )&AppTaskCreate_Handle);/* 任务控制块指针 */ 
	/* 启动任务调度 */           
	if(pdPASS == xReturn)
		vTaskStartScheduler();   /* 启动任务，开启调度 */
	else
		return -1;  

}


static void BSP_Init(void)
{
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	
	LED_GPIO_Config();
	Key_GPIO_Config();
	DEBUG_USART_Config();
}

static void AppTaskCreate(void)
{
	BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
  
	taskENTER_CRITICAL();           //进入临界区
  
	/* 创建MuxSem */
	MuxSem_Handle = xSemaphoreCreateMutex();	 
	if(NULL != MuxSem_Handle)
		printf("MuxSem_Handle互斥量创建成功!\r\n");

	xReturn = xSemaphoreGive( MuxSem_Handle );//给出互斥量

	/* 创建LowPriority_Task任务 */
	xReturn = xTaskCreate((TaskFunction_t )LowPriority_Task, /* 任务入口函数 */
							(const char*    )"LowPriority_Task",/* 任务名字 */
							(uint16_t       )512,   /* 任务栈大小 */
							(void*          )NULL,	/* 任务入口函数参数 */
							(UBaseType_t    )2,	    /* 任务的优先级 */
							(TaskHandle_t*  )&LowPriority_Task_Handle);/* 任务控制块指针 */
	if(pdPASS == xReturn)
		printf("创建LowPriority_Task任务成功!\r\n");
  
	/* 创建MidPriority_Task任务 */
	xReturn = xTaskCreate((TaskFunction_t )MidPriority_Task,  /* 任务入口函数 */
                        (const char*    )"MidPriority_Task",/* 任务名字 */
                        (uint16_t       )512,  /* 任务栈大小 */
                        (void*          )NULL,/* 任务入口函数参数 */
                        (UBaseType_t    )3, /* 任务的优先级 */
                        (TaskHandle_t*  )&MidPriority_Task_Handle);/* 任务控制块指针 */ 
	if(pdPASS == xReturn)
		printf("创建MidPriority_Task任务成功!\n");
  
	/* 创建HighPriority_Task任务 */
	xReturn = xTaskCreate((TaskFunction_t )HighPriority_Task,  /* 任务入口函数 */
                        (const char*    )"HighPriority_Task",/* 任务名字 */
                        (uint16_t       )512,  /* 任务栈大小 */
                        (void*          )NULL,/* 任务入口函数参数 */
                        (UBaseType_t    )4, /* 任务的优先级 */
                        (TaskHandle_t*  )&HighPriority_Task_Handle);/* 任务控制块指针 */ 
	if(pdPASS == xReturn)
		printf("创建HighPriority_Task任务成功!\n\n");
  
	vTaskDelete(AppTaskCreate_Handle); //删除AppTaskCreate任务
  
	taskEXIT_CRITICAL();            //退出临界区
}

static void LowPriority_Task(void* parameter)
{	
	static uint32_t i;
	BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
	while (1)
	{
		printf("LowPriority_Task 获取互斥量\r\n");
		//获取互斥量 MuxSem,没获取到则一直等待
		xReturn = xSemaphoreTake(MuxSem_Handle,/* 互斥量句柄 */
                              portMAX_DELAY); /* 等待时间 */
		if(pdTRUE == xReturn)
		printf("LowPriority_Task Runing\r\n");
    
		for(i=0;i<4000000;i++)//模拟低优先级任务占用互斥量
		{
			taskYIELD();//发起任务调度
		}
    
		printf("LowPriority_Task 释放互斥量!\r\n");
		xReturn = xSemaphoreGive( MuxSem_Handle );//给出互斥量
    
		vTaskDelay(1000);
	}
}

static void MidPriority_Task(void* parameter)
{	 
	while (1)
	{
		printf("MidPriority_Task Runing\r\n");
		vTaskDelay(1000);
	}
}

static void HighPriority_Task(void* parameter)
{	
	BaseType_t xReturn = pdTRUE;/* 定义一个创建信息返回值，默认为pdPASS */
	while (1)
	{
		printf("HighPriority_Task 获取互斥量\r\n");
		//获取互斥量 MuxSem,没获取到则一直等待
		xReturn = xSemaphoreTake(MuxSem_Handle,/* 互斥量句柄 */
                              portMAX_DELAY); /* 等待时间 */
		if(pdTRUE == xReturn)
			printf("HighPriority_Task Runing\r\n");

    
		printf("HighPriority_Task 释放互斥量!\r\n");
		xReturn = xSemaphoreGive( MuxSem_Handle );//给出互斥量

  
		vTaskDelay(1000);
	}
}


