/***************************************
 * �ļ���  ��main.c
 * ����    ������KEY1��KEY2���ֱ�תLED1�� LED2�����ò�ѯ��ʽ��         
 * ʵ��ƽ̨��MINI STM32������ ����STM32F103C8T6
 * ��汾  ��ST3.0.0
 
*********************************************************/ 

#include "stm32f10x.h"
#include "led.h"
#include "key.h"  
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
osThreadId task_button_id;                    /* assigned task id of task: t_uart_rx_id  */
void task_button(void const *argument)
{			
		/*����Ƿ��а������� */
		u16 button_dn_cnt, button_up_cnt;
		button_dn_cnt = 0;
		button_up_cnt = 0;
		for(;;) {
			if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) == KEY_ON ) 
			{	   
				/*��ʱ����*/
				osDelay(100);	
				while(1) {
					if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) == KEY_ON )  
					{	 
						/*�ȴ������ͷ� */
						osDelay(50);		
						button_dn_cnt++;
					}
					else {
						button_dn_cnt = 0;
					}
					if(button_dn_cnt > 100) {
						break;
					}
					
				}
				if(button_dn_cnt > 100) {
						GPIO_WriteBit(GPIOA, GPIO_Pin_1, (BitAction)(1));
						GPIO_WriteBit(GPIOA, GPIO_Pin_2, (BitAction)(1));
				}			
				while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) == KEY_ON);  
			
		}
		osDelay(50);	
	}
}
osThreadDef(task_button, osPriorityHigh, 1, 0);

int main(void)
{   
  SystemInit();	// ����ϵͳʱ��Ϊ72M 	
  LED_GPIO_Config(); //LED �˿ڳ�ʼ��   	
  Key_GPIO_Config();//�����˿ڳ�ʼ��

	task_button_id = osThreadCreate(osThread(task_button), NULL);  

  osDelay(osWaitForever);

}


