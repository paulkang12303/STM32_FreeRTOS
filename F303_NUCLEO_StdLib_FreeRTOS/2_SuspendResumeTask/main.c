

/* Includes ------------------------------------------------------------------*/
#include "stm32f30x.h"

#include "FreeRTOS.h"
#include "task.h"

#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_key.h"

static void BSP_Init(void);

static TaskHandle_t AppTaskCreate_Handle = NULL;/* 创建任务句柄 */
static TaskHandle_t LED_Task_Handle = NULL;/* LED任务句柄 */
static TaskHandle_t KEY_Task_Handle = NULL;/* KEY任务句柄 */

static void AppTaskCreate(void);/* 用于创建任务 */
static void LED_Task(void* pvParameters);/* LED_Task任务实现 */
static void KEY_Task(void* pvParameters);/* KEY_Task任务实现 */

int main(void)
{
  /* 开发板硬件初始化 */
  BSP_Init();
	
  BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
   /* 创建AppTaskCreate任务 */
  xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,  /* 任务入口函数 */
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
    printf("创建KEY_Task任务成功!\r\n");
  
  vTaskDelete(AppTaskCreate_Handle); //删除AppTaskCreate任务
  
  taskEXIT_CRITICAL();            //退出临界区
}

static void LED_Task(void* parameter)
{	
  while (1)
  {
    GPIO_SetBits(GPIOA, GPIO_Pin_5);
    vTaskDelay(500);   /* 延时500个tick */
    
    GPIO_ResetBits(GPIOA, GPIO_Pin_5);     
    vTaskDelay(500);   /* 延时500个tick */
  }
}

static void KEY_Task(void* parameter)
{
  static uint8_t flag = 0 ;
  while (1)
  {
    if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == 0 )
    {
		if (flag == 0)
		{
			vTaskSuspend(LED_Task_Handle);
			printf("挂起LED任务成功！\n");
			flag = 1;
		}
		else
		{
			vTaskResume(LED_Task_Handle);
			printf("恢复LED任务成功！\n");
			flag = 0;
		}
    } 
    vTaskDelay(20);/* 延时20个tick */
  }
}
