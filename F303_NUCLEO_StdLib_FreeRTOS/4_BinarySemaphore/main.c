

/* Includes ------------------------------------------------------------------*/
#include "stm32f30x.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_key.h"

static TaskHandle_t AppTaskCreate_Handle = NULL;/* 创建任务句柄 */
static TaskHandle_t Receive_Task_Handle = NULL;/* LED任务句柄 */
static TaskHandle_t Send_Task_Handle = NULL;/* KEY任务句柄 */

SemaphoreHandle_t BinarySem_Handle =NULL;

static void AppTaskCreate(void);/* 用于创建任务 */
static void Receive_Task(void* pvParameters);/* Receive_Task任务实现 */
static void Send_Task(void* pvParameters);/* Send_Task任务实现 */

static void BSP_Init(void);

int main(void)
{
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

static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
  
  taskENTER_CRITICAL();           //进入临界区
  
  /* 创建 BinarySem */
  BinarySem_Handle = xSemaphoreCreateBinary();	 
  if(NULL != BinarySem_Handle)
    printf("BinarySem_Handle二值信号量创建成功!\r\n");

  /* 创建Receive_Task任务 */
  xReturn = xTaskCreate((TaskFunction_t )Receive_Task, /* 任务入口函数 */
                        (const char*    )"Receive_Task",/* 任务名字 */
                        (uint16_t       )512,   /* 任务栈大小 */
                        (void*          )NULL,	/* 任务入口函数参数 */
                        (UBaseType_t    )2,	    /* 任务的优先级 */
                        (TaskHandle_t*  )&Receive_Task_Handle);/* 任务控制块指针 */
  if(pdPASS == xReturn)
    printf("创建Receive_Task任务成功!\r\n");
  
  /* 创建Send_Task任务 */
  xReturn = xTaskCreate((TaskFunction_t )Send_Task,  /* 任务入口函数 */
                        (const char*    )"Send_Task",/* 任务名字 */
                        (uint16_t       )512,  /* 任务栈大小 */
                        (void*          )NULL,/* 任务入口函数参数 */
                        (UBaseType_t    )3, /* 任务的优先级 */
                        (TaskHandle_t*  )&Send_Task_Handle);/* 任务控制块指针 */ 
  if(pdPASS == xReturn)
    printf("创建Send_Task任务成功!\n\n");
  
  vTaskDelete(AppTaskCreate_Handle); //删除AppTaskCreate任务
  
  taskEXIT_CRITICAL();            //退出临界区
}

static void Receive_Task(void* parameter)
{	
  BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
  while (1)
  {
		LED1_GPIO_PORT->ODR ^=LED1_GPIO_PIN;
		//获取二值信号量 xSemaphore,没获取到则一直等待
		xReturn = xSemaphoreTake(BinarySem_Handle,/* 二值信号量句柄 */
									portMAX_DELAY); /* 等待时间 */
		if(pdTRUE == xReturn)
			printf("BinarySem_Handle二值信号量获取成功!\n\n");
  }
}

static void Send_Task(void* parameter)
{	 
  BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
  while (1)
  {
    /* K1 被按下 */
    if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_DOWN )
    {
      xReturn = xSemaphoreGive( BinarySem_Handle );//给出二值信号量
		if( xReturn == pdTRUE )
			printf("BinarySem_Handle二值信号量释放成功!\r\n");
		else
			printf("BinarySem_Handle二值信号量释放失败!\r\n");
    } 
	/* K2 被按下 */
    if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_DOWN )
    {
      xReturn = xSemaphoreGive( BinarySem_Handle );//给出二值信号量
		if( xReturn == pdTRUE )
			printf("BinarySem_Handle二值信号量释放成功!\r\n");
		else
			printf("BinarySem_Handle二值信号量释放失败!\r\n");
    }
    vTaskDelay(20);
  }
}

static void BSP_Init(void)
{
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	
	LED_GPIO_Config();
	Key_GPIO_Config();
	DEBUG_USART_Config();
}


