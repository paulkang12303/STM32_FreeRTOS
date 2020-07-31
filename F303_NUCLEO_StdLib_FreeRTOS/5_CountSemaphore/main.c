//FreeRTOSConfig.h set #define configUSE_COUNTING_SEMAPHORES		1

/* Includes ------------------------------------------------------------------*/
#include "stm32f30x.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_key.h"

static TaskHandle_t AppTaskCreate_Handle = NULL;/* ���������� */
static TaskHandle_t Take_Task_Handle = NULL;/* Take_Task������ */
static TaskHandle_t Give_Task_Handle = NULL;/* Give_Task������ */

SemaphoreHandle_t CountSem_Handle =NULL;

static void AppTaskCreate(void);/* ���ڴ������� */
static void Take_Task(void* pvParameters);/* Take_Task����ʵ�� */
static void Give_Task(void* pvParameters);/* Give_Task����ʵ�� */
static void BSP_Init(void);

int main(void)
{
	BSP_Init();
	
	BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
	xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,  /* ������ں��� */
						 (const char*    )"AppTaskCreate",/* �������� */
                         (uint16_t       )512,  /* ����ջ��С */
                         (void*          )NULL,/* ������ں������� */
                         (UBaseType_t    )1, /* ��������ȼ� */
                         (TaskHandle_t*  )&AppTaskCreate_Handle);/* ������ƿ�ָ�� */ 
	/* ����������� */           
	if(pdPASS == xReturn)
		vTaskStartScheduler();   /* �������񣬿������� */
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
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  
  taskENTER_CRITICAL();           //�����ٽ���
  
  /* ����CountSemaphore */
  CountSem_Handle = xSemaphoreCreateCounting(5,5);	 
  if(NULL != CountSem_Handle)
    printf("CountSem_Handle�����ź��������ɹ�!\r\n");

  /* ����Take_Task���� */
  xReturn = xTaskCreate((TaskFunction_t )Take_Task, /* ������ں��� */
                        (const char*    )"Take_Task",/* �������� */
                        (uint16_t       )512,   /* ����ջ��С */
                        (void*          )NULL,	/* ������ں������� */
                        (UBaseType_t    )2,	    /* ��������ȼ� */
                        (TaskHandle_t*  )&Take_Task_Handle);/* ������ƿ�ָ�� */
  if(pdPASS == xReturn)
    printf("����Take_Task����ɹ�!\r\n");
  
  /* ����Give_Task���� */
  xReturn = xTaskCreate((TaskFunction_t )Give_Task,  /* ������ں��� */
                        (const char*    )"Give_Task",/* �������� */
                        (uint16_t       )512,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )3, /* ��������ȼ� */
                        (TaskHandle_t*  )&Give_Task_Handle);/* ������ƿ�ָ�� */ 
  if(pdPASS == xReturn)
    printf("����Give_Task����ɹ�!\n\n");
  
  vTaskDelete(AppTaskCreate_Handle); //ɾ��AppTaskCreate����
  
  taskEXIT_CRITICAL();            //�˳��ٽ���
}

static void Take_Task(void* parameter)
{	
  BaseType_t xReturn = pdTRUE;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  /* ������һ������ѭ�������ܷ��� */
  while (1)
  {
    //���KEY2������
		if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_DOWN )       
		{
			/* ��ȡһ�������ź��� */
			xReturn = xSemaphoreTake(CountSem_Handle,	/* �����ź������ */
												0); 	/* �ȴ�ʱ�䣺0 */
			if ( pdTRUE == xReturn ) 
				printf( "KEY2�����£��ɹ����뵽ͣ��λ��\n" );
			else
				printf( "KEY2�����£�������˼������ͣ����������\n" );							
		}
		vTaskDelay(20);     //ÿ20msɨ��һ��		
  }
}


static void Give_Task(void* parameter)
{	 
  BaseType_t xReturn = pdTRUE;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  /* ������һ������ѭ�������ܷ��� */
  while (1)
  {
    //���KEY1������
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_DOWN )       
		{
			/* ��ȡһ�������ź��� */
			xReturn = xSemaphoreGive(CountSem_Handle);//���������ź���                  
			if ( pdTRUE == xReturn ) 
				printf( "KEY1�����£��ͷ�1��ͣ��λ��\n" );
			else
				printf( "KEY1�����£������޳�λ�����ͷţ�\n" );							
		}
		vTaskDelay(20);     //ÿ20msɨ��һ��	
  }
}
