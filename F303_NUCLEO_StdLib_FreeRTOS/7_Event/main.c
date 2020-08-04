
//using STM32F303 NUCLEO Board

/* Includes ------------------------------------------------------------------*/
#include "stm32f30x.h"

#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_key.h"

static TaskHandle_t AppTaskCreate_Handle = NULL;/* 创建任务句柄 */
static TaskHandle_t LED_Task_Handle = NULL;/* LED_Task任务句柄 */
static TaskHandle_t KEY_Task_Handle = NULL;/* KEY_Task任务句柄 */

static EventGroupHandle_t Event_Handle =NULL;

#define KEY1_EVENT  (0x01 << 0)//设置事件掩码的位0
#define KEY2_EVENT  (0x01 << 1)//设置事件掩码的位1

static void AppTaskCreate(void);/* 用于创建任务 */
static void LED_Task(void* pvParameters);/* LED_Task 任务实现 */
static void KEY_Task(void* pvParameters);/* KEY_Task 任务实现 */
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
  
  /* 创建 Event_Handle */
  Event_Handle = xEventGroupCreate();	 
  if(NULL != Event_Handle)
    printf("Event_Handle 事件创建成功!\r\n");
    
  /* 创建LED_Task任务 */
  xReturn = xTaskCreate((TaskFunction_t )LED_Task, /* 任务入口函数 */
                        (const char*    )"LED_Task",/* 任务名字 */
                        (uint16_t       )512,   /* 任务栈大小 */
                        (void*          )NULL,	/* 任务入口函数参数 */
                        (UBaseType_t    )2,	    /* 任务的优先级 */
                        (TaskHandle_t*  )&LED_Task_Handle);/* 任务控制块指针 */
  if(pdPASS == xReturn)
    printf("创建LED_Task任务成功!\r\n");
  
  /* 创建KEY_Task任务 */
  xReturn = xTaskCreate((TaskFunction_t )KEY_Task,  /* 任务入口函数 */
                        (const char*    )"KEY_Task",/* 任务名字 */
                        (uint16_t       )512,  /* 任务栈大小 */
                        (void*          )NULL,/* 任务入口函数参数 */
                        (UBaseType_t    )3, /* 任务的优先级 */
                        (TaskHandle_t*  )&KEY_Task_Handle);/* 任务控制块指针 */ 
  if(pdPASS == xReturn)
    printf("创建KEY_Task任务成功!\n");
  
  vTaskDelete(AppTaskCreate_Handle); //删除AppTaskCreate任务
  
  taskEXIT_CRITICAL();            //退出临界区
}

static void LED_Task(void* parameter)
{	
	EventBits_t r_event;  /* 定义一个事件接收变量 */
	/* 任务都是一个无限循环，不能返回 */
	while (1)
	{
		r_event = xEventGroupWaitBits(	Event_Handle,  /* 事件对象句柄 */
										KEY1_EVENT|KEY2_EVENT,/* 接收线程感兴趣的事件 */
										pdTRUE,   /* 退出时清除事件位 */
										pdTRUE,   /* 满足感兴趣的所有事件 */
										portMAX_DELAY);/* 指定超时事件,一直等 */
                        
		if((r_event & (KEY1_EVENT|KEY2_EVENT)) == (KEY1_EVENT|KEY2_EVENT)) 
		{
			/* 如果接收完成并且正确 */
			printf ( "KEY1与KEY2都按下\n");		
		}
		else
			printf ( "事件错误！\n");	
  }
}

static void KEY_Task(void* parameter)
{	 
    /* 任务都是一个无限循环，不能返回 */
	while (1)
	{
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_DOWN )       //如果KEY2被单击
		{
			printf ( "KEY1被按下\n" );
			/* 触发一个事件1 */
			xEventGroupSetBits(Event_Handle,KEY1_EVENT);  					
		}
    
		if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_DOWN )       //如果KEY2被单击
		{
			printf ( "KEY2被按下\n" );	
			/* 触发一个事件2 */
			xEventGroupSetBits(Event_Handle,KEY2_EVENT); 				
		}
		vTaskDelay(20);     //每20ms扫描一次		
	}
}
