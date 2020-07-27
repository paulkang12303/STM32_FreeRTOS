

/* Includes ------------------------------------------------------------------*/
#include "stm32f30x.h"

#include "FreeRTOS.h"
#include "task.h"

#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_key.h"

static void BSP_Init(void);

static TaskHandle_t AppTaskCreate_Handle = NULL;/* ���������� */
static TaskHandle_t LED_Task_Handle = NULL;/* LED������ */
static TaskHandle_t KEY_Task_Handle = NULL;/* KEY������ */

static void AppTaskCreate(void);/* ���ڴ������� */
static void LED_Task(void* pvParameters);/* LED_Task����ʵ�� */
static void KEY_Task(void* pvParameters);/* KEY_Task����ʵ�� */

int main(void)
{
  /* ������Ӳ����ʼ�� */
  BSP_Init();
	
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
   /* ����AppTaskCreate���� */
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
  
  /* ����LED_Task���� */
  xReturn = xTaskCreate((TaskFunction_t )LED_Task, /* ������ں��� */
                        (const char*    )"LED_Task",/* �������� */
                        (uint16_t       )512,   /* ����ջ��С */
                        (void*          )NULL,	/* ������ں������� */
                        (UBaseType_t    )2,	    /* ��������ȼ� */
                        (TaskHandle_t*  )&LED_Task_Handle);/* ������ƿ�ָ�� */
  if(pdPASS == xReturn)
    printf("����LED_Task����ɹ�!\r\n");
  /* ����KEY_Task���� */
  xReturn = xTaskCreate((TaskFunction_t )KEY_Task,  /* ������ں��� */
                        (const char*    )"KEY_Task",/* �������� */
                        (uint16_t       )512,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )3, /* ��������ȼ� */
                        (TaskHandle_t*  )&KEY_Task_Handle);/* ������ƿ�ָ�� */ 
  if(pdPASS == xReturn)
    printf("����KEY_Task����ɹ�!\r\n");
  
  vTaskDelete(AppTaskCreate_Handle); //ɾ��AppTaskCreate����
  
  taskEXIT_CRITICAL();            //�˳��ٽ���
}

static void LED_Task(void* parameter)
{	
  while (1)
  {
    GPIO_SetBits(GPIOA, GPIO_Pin_5);
    vTaskDelay(500);   /* ��ʱ500��tick */
    
    GPIO_ResetBits(GPIOA, GPIO_Pin_5);     
    vTaskDelay(500);   /* ��ʱ500��tick */
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
			printf("����LED����ɹ���\n");
			flag = 1;
		}
		else
		{
			vTaskResume(LED_Task_Handle);
			printf("�ָ�LED����ɹ���\n");
			flag = 0;
		}
    } 
    vTaskDelay(20);/* ��ʱ20��tick */
  }
}




