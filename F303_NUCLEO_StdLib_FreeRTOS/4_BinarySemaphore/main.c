

/* Includes ------------------------------------------------------------------*/
#include "stm32f30x.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_key.h"

static TaskHandle_t AppTaskCreate_Handle = NULL;/* ���������� */
static TaskHandle_t Receive_Task_Handle = NULL;/* LED������ */
static TaskHandle_t Send_Task_Handle = NULL;/* KEY������ */

SemaphoreHandle_t BinarySem_Handle =NULL;

static void AppTaskCreate(void);/* ���ڴ������� */
static void Receive_Task(void* pvParameters);/* Receive_Task����ʵ�� */
static void Send_Task(void* pvParameters);/* Send_Task����ʵ�� */

static void BSP_Init(void);

int main(void)
{
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

static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  
  taskENTER_CRITICAL();           //�����ٽ���
  
  /* ���� BinarySem */
  BinarySem_Handle = xSemaphoreCreateBinary();	 
  if(NULL != BinarySem_Handle)
    printf("BinarySem_Handle��ֵ�ź��������ɹ�!\r\n");

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
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  while (1)
  {
		LED1_GPIO_PORT->ODR ^=LED1_GPIO_PIN;
		//��ȡ��ֵ�ź��� xSemaphore,û��ȡ����һֱ�ȴ�
		xReturn = xSemaphoreTake(BinarySem_Handle,/* ��ֵ�ź������ */
									portMAX_DELAY); /* �ȴ�ʱ�� */
		if(pdTRUE == xReturn)
			printf("BinarySem_Handle��ֵ�ź�����ȡ�ɹ�!\n\n");
  }
}

static void Send_Task(void* parameter)
{	 
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  while (1)
  {
    /* K1 ������ */
    if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_DOWN )
    {
      xReturn = xSemaphoreGive( BinarySem_Handle );//������ֵ�ź���
		if( xReturn == pdTRUE )
			printf("BinarySem_Handle��ֵ�ź����ͷųɹ�!\r\n");
		else
			printf("BinarySem_Handle��ֵ�ź����ͷ�ʧ��!\r\n");
    } 
	/* K2 ������ */
    if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_DOWN )
    {
      xReturn = xSemaphoreGive( BinarySem_Handle );//������ֵ�ź���
		if( xReturn == pdTRUE )
			printf("BinarySem_Handle��ֵ�ź����ͷųɹ�!\r\n");
		else
			printf("BinarySem_Handle��ֵ�ź����ͷ�ʧ��!\r\n");
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


