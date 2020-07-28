

/* Includes ------------------------------------------------------------------*/
#include "stm32f30x.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_key.h"

static TaskHandle_t AppTaskCreate_Handle = NULL;/* ���������� */
static TaskHandle_t Receive_Task_Handle = NULL;/* ���������� */
static TaskHandle_t Send_Task_Handle = NULL;/* ���������� */

QueueHandle_t Test_Queue =NULL;

#define  QUEUE_LEN    4   /* ���еĳ��ȣ����ɰ������ٸ���Ϣ */
#define  QUEUE_SIZE   4   /* ������ÿ����Ϣ��С���ֽڣ� */

static void AppTaskCreate(void);/* ���ڴ������� */
static void Receive_Task(void* pvParameters);/* Receive_Task����ʵ�� */
static void Send_Task(void* pvParameters);/* Send_Task����ʵ�� */
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
  
  /* ����Test_Queue */
  Test_Queue = xQueueCreate((UBaseType_t ) QUEUE_LEN,/* ��Ϣ���еĳ��� */
                            (UBaseType_t ) QUEUE_SIZE);/* ��Ϣ�Ĵ�С */
  if(NULL != Test_Queue)
    printf("����Test_Queue��Ϣ���гɹ�!\r\n");
  
  /* ����Receive_Task���� */
  xReturn = xTaskCreate((TaskFunction_t )Receive_Task, /* ������ں��� */
                        (const char*    )"Receive_Task",/* �������� */
                        (uint16_t       )512,   /* ����ջ��С */
                        (void*          )NULL,	/* ������ں������� */
                        (UBaseType_t    )2,	    /* ��������ȼ� */
                        (TaskHandle_t*  )&Receive_Task_Handle);/* ������ƿ�ָ�� */
  if(pdPASS == xReturn)
    printf("����Receive_Task����ɹ�!\r\n");
  
  /* ����Send_Task���� */
  xReturn = xTaskCreate((TaskFunction_t )Send_Task,  /* ������ں��� */
                        (const char*    )"Send_Task",/* �������� */
                        (uint16_t       )512,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )3, /* ��������ȼ� */
                        (TaskHandle_t*  )&Send_Task_Handle);/* ������ƿ�ָ�� */ 
  if(pdPASS == xReturn)
    printf("����Send_Task����ɹ�!\n\n");
  
  vTaskDelete(AppTaskCreate_Handle); //ɾ��AppTaskCreate����
  
  taskEXIT_CRITICAL();            //�˳��ٽ���
}

static void Receive_Task(void* parameter)
{	
  BaseType_t xReturn = pdTRUE;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdTRUE */
  uint32_t r_queue;	/* ����һ��������Ϣ�ı��� */
  while (1)
  {
    xReturn = xQueueReceive( Test_Queue,    /* ��Ϣ���еľ�� */
                             &r_queue,      /* ���͵���Ϣ���� */
                             portMAX_DELAY); /* �ȴ�ʱ�� һֱ�� */
    if(pdTRUE == xReturn)
      printf("���ν��յ���������%d\n\n",r_queue);
    else
      printf("���ݽ��ճ���,�������0x%lx\n",xReturn);
  }
}

static void Send_Task(void* parameter)
{	 
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  uint32_t send_data1 = 1;
  while (1)
  {
    if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == 0 )
    {
      printf("������Ϣsend_data1��\n");
      xReturn = xQueueSend( Test_Queue, /* ��Ϣ���еľ�� */
                            &send_data1,/* ���͵���Ϣ���� */
                            0 );        /* �ȴ�ʱ�� 0 */
      if(pdPASS == xReturn)
        printf("��Ϣsend_data1���ͳɹ�!\n\n");
    } 
    vTaskDelay(20);/* ��ʱ20��tick */
  }
}
