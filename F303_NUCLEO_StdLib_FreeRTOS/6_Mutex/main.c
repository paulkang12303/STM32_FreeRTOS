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

static TaskHandle_t AppTaskCreate_Handle = NULL;/* ���������� */
static TaskHandle_t LowPriority_Task_Handle = NULL;/* LowPriority_Task������ */
static TaskHandle_t MidPriority_Task_Handle = NULL;/* MidPriority_Task������ */
static TaskHandle_t HighPriority_Task_Handle = NULL;/* HighPriority_Task������ */

SemaphoreHandle_t MuxSem_Handle =NULL;

static void AppTaskCreate(void);/* ���ڴ������� */
static void LowPriority_Task(void* pvParameters);/* LowPriority_Task����ʵ�� */
static void MidPriority_Task(void* pvParameters);/* MidPriority_Task����ʵ�� */
static void HighPriority_Task(void* pvParameters);/* MidPriority_Task����ʵ�� */

static void BSP_Init(void);

int main(void)
{
	BSP_Init();
	BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  
	/* ����AppTaskCreate���� */
	xReturn = xTaskCreate(	(TaskFunction_t )AppTaskCreate,  /* ������ں��� */
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
  
	/* ����MuxSem */
	MuxSem_Handle = xSemaphoreCreateMutex();	 
	if(NULL != MuxSem_Handle)
		printf("MuxSem_Handle�����������ɹ�!\r\n");

	xReturn = xSemaphoreGive( MuxSem_Handle );//����������

	/* ����LowPriority_Task���� */
	xReturn = xTaskCreate((TaskFunction_t )LowPriority_Task, /* ������ں��� */
							(const char*    )"LowPriority_Task",/* �������� */
							(uint16_t       )512,   /* ����ջ��С */
							(void*          )NULL,	/* ������ں������� */
							(UBaseType_t    )2,	    /* ��������ȼ� */
							(TaskHandle_t*  )&LowPriority_Task_Handle);/* ������ƿ�ָ�� */
	if(pdPASS == xReturn)
		printf("����LowPriority_Task����ɹ�!\r\n");
  
	/* ����MidPriority_Task���� */
	xReturn = xTaskCreate((TaskFunction_t )MidPriority_Task,  /* ������ں��� */
                        (const char*    )"MidPriority_Task",/* �������� */
                        (uint16_t       )512,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )3, /* ��������ȼ� */
                        (TaskHandle_t*  )&MidPriority_Task_Handle);/* ������ƿ�ָ�� */ 
	if(pdPASS == xReturn)
		printf("����MidPriority_Task����ɹ�!\n");
  
	/* ����HighPriority_Task���� */
	xReturn = xTaskCreate((TaskFunction_t )HighPriority_Task,  /* ������ں��� */
                        (const char*    )"HighPriority_Task",/* �������� */
                        (uint16_t       )512,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )4, /* ��������ȼ� */
                        (TaskHandle_t*  )&HighPriority_Task_Handle);/* ������ƿ�ָ�� */ 
	if(pdPASS == xReturn)
		printf("����HighPriority_Task����ɹ�!\n\n");
  
	vTaskDelete(AppTaskCreate_Handle); //ɾ��AppTaskCreate����
  
	taskEXIT_CRITICAL();            //�˳��ٽ���
}

static void LowPriority_Task(void* parameter)
{	
	static uint32_t i;
	BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
	while (1)
	{
		printf("LowPriority_Task ��ȡ������\r\n");
		//��ȡ������ MuxSem,û��ȡ����һֱ�ȴ�
		xReturn = xSemaphoreTake(MuxSem_Handle,/* ��������� */
                              portMAX_DELAY); /* �ȴ�ʱ�� */
		if(pdTRUE == xReturn)
		printf("LowPriority_Task Runing\r\n");
    
		for(i=0;i<4000000;i++)//ģ������ȼ�����ռ�û�����
		{
			taskYIELD();//�����������
		}
    
		printf("LowPriority_Task �ͷŻ�����!\r\n");
		xReturn = xSemaphoreGive( MuxSem_Handle );//����������
    
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
	BaseType_t xReturn = pdTRUE;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
	while (1)
	{
		printf("HighPriority_Task ��ȡ������\r\n");
		//��ȡ������ MuxSem,û��ȡ����һֱ�ȴ�
		xReturn = xSemaphoreTake(MuxSem_Handle,/* ��������� */
                              portMAX_DELAY); /* �ȴ�ʱ�� */
		if(pdTRUE == xReturn)
			printf("HighPriority_Task Runing\r\n");

    
		printf("HighPriority_Task �ͷŻ�����!\r\n");
		xReturn = xSemaphoreGive( MuxSem_Handle );//����������

  
		vTaskDelay(1000);
	}
}


