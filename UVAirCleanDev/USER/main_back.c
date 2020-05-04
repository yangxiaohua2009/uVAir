/***************************************
 * 文件名  ：main.c
 * 描述    ：按下KEY1、KEY2，分别翻转LED1、 LED2，采用查询方式。         
 * 实验平台：MINI STM32开发板 基于STM32F103C8T6
 * 库版本  ：ST3.0.0
 
*********************************************************/ 

#include "stm32f10x.h"
#include "led.h"
#include "key.h"  
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
osThreadId task_button_id;                    /* assigned task id of task: t_uart_rx_id  */
void task_button(void const *argument)
{			
		/*检测是否有按键按下 */
		u16 button_dn_cnt, button_up_cnt;
		button_dn_cnt = 0;
		button_up_cnt = 0;
		for(;;) {
			if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) == KEY_ON ) 
			{	   
				/*延时消抖*/
				osDelay(100);	
				while(1) {
					if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) == KEY_ON )  
					{	 
						/*等待按键释放 */
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
  SystemInit();	// 配置系统时钟为72M 	
  LED_GPIO_Config(); //LED 端口初始化   	
  Key_GPIO_Config();//按键端口初始化

	task_button_id = osThreadCreate(osThread(task_button), NULL);  

  osDelay(osWaitForever);

}


