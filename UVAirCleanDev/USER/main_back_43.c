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
#include "uvdev.h"

struct myprivate uv_private;

typedef struct {
	char canData[10];
} message_t;

osThreadId task_button_id;                    /* assigned task id of task: t_uart_rx_id  */
osThreadId task_led_id; 
osThreadId task_buzzer_id; 

osPoolDef(task_led_pool, 3, message_t); 
osPoolId task_led_pool;
osMessageQDef(task_led_quenue, 3, message_t); 
osMessageQId task_led_quenue;
osEvent task_led_evt;

osPoolDef(task_buzzer_pool, 3, message_t); 
osPoolId task_buzzer_pool;
osMessageQDef(task_buzzer_quenue, 3, message_t); 
osMessageQId task_buzzer_quenue;
osEvent task_buzzer_evt;

/*
*	开关定时或亮度指示灯
*/
void led_time_or_lightness_onoff(unsigned char onoff)
{
	switch(uv_private.work_mode & 0x0f)
	{
		case 0:	//开机巡检模式
			work_status_flash(0, LED_OFF);
			work_status_flash(1, LED_OFF);
			work_status_flash(2, LED_OFF);	
			break;
		case 1: //紫外消杀
		case 2: //通风净化
		case 3: //紫外直照
			switch(dev_priv.work_time) 
			{
				case 0:   //开机巡检状态
					work_status_flash(LED_TIME_15, LED_OFF);
					work_status_flash(LED_TIME_30, LED_OFF);
					work_status_flash(LED_TIME_FOREVER, LED_OFF);
					break;
				case 1:   //工作15min
					work_status_flash(LED_TIME_15, onoff);
					work_status_flash(LED_TIME_30, LED_OFF);
					work_status_flash(LED_TIME_FOREVER, LED_OFF);
					break;
				case 2:   //工作30min
					work_status_flash(LED_TIME_15, LED_OFF);
					work_status_flash(LED_TIME_30, onoff);
					work_status_flash(LED_TIME_FOREVER, LED_OFF);
					break;
				case 3:		//一直工作
					work_status_flash(LED_TIME_15, LED_OFF);
					work_status_flash(LED_TIME_30, LED_OFF);
					work_status_flash(LED_TIME_FOREVER, onoff);
					break;
				default:
					break;
			}
			break;
		case 4: //台灯
			switch(uv_private.lightness) 
			{
				case 0:   //开机巡检状态, 照明关闭
					work_status_flash(LED_TIME_15, LED_OFF);
					work_status_flash(LED_TIME_30, LED_OFF);
					work_status_flash(LED_TIME_FOREVER, LED_OFF);
					break;
				case 1:   //照明开启，亮度30%
					work_status_flash(LED_TIME_15, onoff);
					work_status_flash(LED_TIME_30, LED_OFF);
					work_status_flash(LED_TIME_FOREVER, LED_OFF);
				break;
				case 2:   //照明开启，亮度50%
					work_status_flash(LED_TIME_15, LED_OFF);
					work_status_flash(LED_TIME_30, onoff);
					work_status_flash(LED_TIME_FOREVER, LED_OFF);
					break;
				case 3:		////照明开启，亮度100%
					work_status_flash(LED_TIME_15, LED_OFF);
					work_status_flash(LED_TIME_30, LED_OFF);
					work_status_flash(LED_TIME_FOREVER, onoff);;
					break;
				default:
					work_status_flash(LED_TIME_15, LED_OFF);
					work_status_flash(LED_TIME_30, LED_OFF);
					work_status_flash(LED_TIME_FOREVER, LED_OFF);;
					break;
			}
			break;
		default:
			break;
	}	
}

/*
*	开关模式指示灯
*/
void led_mode_onoff(unsigned char onoff)
{
	switch(uv_private.work_mode & 0x0f)
	{
		case 0:   //开机巡检模式
			work_status_flash(LED_MODE_UVAIR, LED_OFF);
			work_status_flash(LED_MODE_AIR, LED_OFF);
			work_status_flash(LED_MODE_UV, LED_OFF);
			break;
		case 1:		//紫外消杀
			work_status_flash(LED_MODE_UVAIR, onoff);
			work_status_flash(LED_MODE_AIR, LED_OFF);
			work_status_flash(LED_MODE_UV, LED_OFF);	
			break;
		case 2:		//通风净化
			work_status_flash(LED_MODE_UVAIR, LED_OFF);
			work_status_flash(LED_MODE_AIR, onoff);
			work_status_flash(LED_MODE_UV, LED_OFF);
			break;
		case 3:		//紫外直照
			work_status_flash(LED_MODE_UVAIR  , LED_OFF);
			work_status_flash(LED_MODE_AIR, LED_OFF);
			work_status_flash(LED_MODE_UV, onoff);
			break;
		case 4:		//台灯
			work_status_flash(LED_MODE_UVAIR, onoff);
			work_status_flash(LED_MODE_AIR, onoff);
			work_status_flash(LED_MODE_UV, onoff);
			break;
		default:
			break;
	}
}
/*
*	切换工作时间或照明亮度
*/
void switch_wtime_or_lightness(void)
{
	switch(uv_private.work_mode & 0x0f)
	{
		case 0:	//开机巡检模式
			break;
		case 1: //紫外消杀
		case 2: //通风净化
		case 3: //紫外直照
			switch(uv_private.work_time) 
			{
				case 0:   //开机巡检状态
					uv_private.work_time = 1;
					break;
				case 1:   //工作15min
					uv_private.work_time = 2;
					break;
				case 2:   //工作30min
					uv_private.work_time = 3;
					break;
				case 3:		//一直工作
					uv_private.work_time = 1;
					break;
				default:
					break;
			}
			break;
		case 4: //台灯
			switch(uv_private.lightness) 
			{
				case 0:   //开机巡检状态, 照明关闭
					uv_private.lightness = 1;
					uv_private.led_light_mode = 0;
					break;
				case 1:   //照明开启，亮度30%
					uv_private.lightness = 2;
					uv_private.led_light_mode = 0x10;
				break;
				case 2:   //照明开启，亮度50%
					uv_private.lightness = 3;
					uv_private.led_light_mode = 0x10;
					break;
				case 3:		////照明开启，亮度100%
					uv_private.lightness = 0;
					uv_private.led_light_mode = 0x00;
					break;
				default:
					uv_private.lightness = 0;
					uv_private.led_light_mode = 0x00;
					break;
			}
			break;
		default:
			break;
	}
}
/*
*	切换工作模式
*/
void switch_workmode(void)
{
	switch(uv_private.work_mode & 0x0f)
	{
		case 0:   //开机巡检模式
			uv_private.work_mode = (uv_private.led_light_mode |  1); //跳转到紫外通风消杀模式
			break;
		case 1:   //紫外消杀
			uv_private.work_mode =  (uv_private.led_light_mode | 2); //跳转到通风净化模式
			break;
		case 2:   //通风净化
			uv_private.work_mode = 	 (uv_private.led_light_mode | 3); //跳转到紫外直照
			break;
		case 3:   //紫外直照
			uv_private.work_mode =  (uv_private.led_light_mode | 4); //跳转到台灯
			break;		
		case 0x4:   //台灯
			uv_private.work_mode =  (uv_private.led_light_mode | 1); //跳转到紫外通风消杀模式
			break;	
		default:
			uv_private.work_mode = 0; //跳转到开机巡检
			break;	
	}
}
/*
* 功能执行
*/
void perform(void)
{
		switch(uv_private.work_mode)
		{
			case 0:
				osSignalSet(t_uv_air_id, 1);
				osSignalSet(t_ledlamp_id, 1);
				break;
			case 1:
			case 2:
			case 3:
				osSignalSet(t_uv_air_id, 1);
				break;
			case 4:
				osSignalSet(t_ledlamp_id, 1);
				break;
			default:
				break;
		}
}


/*
*	Buzzer Task
*/
void task_buzzer(void const *argument)
{
	unsigned char cnt;
	message_t *message_buzzer;
	message_t *message;
	message_buzzer = (message_t*)osPoolAlloc(task_buzzer_pool);	
	for(;;)
	{
		task_buzzer_evt = osMessageGet(task_buzzer_quenue, osWaitForever);
		if(task_buzzer_evt.status == osEventMessage) {
			message = (message_t*)task_buzzer_evt.value.p;
			switch((uint32_t)message->canData[0])
			{
				case 0: //idle mode
					for(cnt = 0; cnt < 40; cnt++) {
						GPIO_WriteBit(BUZZER_BANK, BUZZER, (BitAction)(BUZZER_ON));
						osDelay(20);
						GPIO_WriteBit(BUZZER_BANK, BUZZER, (BitAction)(BUZZER_OFF));
						osDelay(20);;
					}
					break;
				case 1: //idle mode
					for(cnt = 0; cnt < 40; cnt++) {
						GPIO_WriteBit(BUZZER_BANK, BUZZER, (BitAction)(BUZZER_ON));
						osDelay(40);
						GPIO_WriteBit(BUZZER_BANK, BUZZER, (BitAction)(BUZZER_OFF));
						osDelay(40);
					}
					break;
				case 2: //idle mode
					for(cnt = 0; cnt < 40; cnt++) {
						GPIO_WriteBit(BUZZER_BANK, BUZZER, (BitAction)(BUZZER_ON));
						osDelay(10);
						GPIO_WriteBit(BUZZER_BANK, BUZZER, (BitAction)(BUZZER_OFF));
						osDelay(10);
					}
					break;
				default:
					break;
			}	
			osPoolFree(task_buzzer_pool, message_buzzer);			
		}
	}
}
osThreadDef(task_buzzer, osPriorityNormal, 1, 0);	
/*
*	Button detection Task
*/
void task_led_status(void const *argument)
{
	int command;
	message_t *message_led;
	message_t *message_buzzer;
	message_t *message;
	message_led = (message_t*)osPoolAlloc(task_led_pool);	
	message_buzzer = (message_t*)osPoolAlloc(task_buzzer_pool);	
	for(;;)
	{
		task_led_evt = osMessageGet(task_led_quenue, 10);
		if(task_led_evt.status == osEventMessage) {
			message = (message_t*)task_led_evt.value.p;
			switch((uint32_t)message->canData[0])
			{
				case 0:     //power on just now
					command = 0;
					message_buzzer->canData[0] = WORK_MODE_IDLE;    
					osMessagePut(task_buzzer_quenue, (uint32_t)message_buzzer, osWaitForever);
					break;
				case 1:     //single button
					command = 1;
					message_buzzer->canData[0] = command;    
					osMessagePut(task_buzzer_quenue, (uint32_t)message_buzzer, osWaitForever);
					break;
				case 2:     //double button
					command = 2;
					message_buzzer->canData[0] = command;    
					osMessagePut(task_buzzer_quenue, (uint32_t)message_buzzer, osWaitForever);
					break;
				default:
					
					break;
				
			}
			osPoolFree(task_led_pool, message_led);
		}
		osDelay(10);
	}
}
osThreadDef(task_led_status, osPriorityNormal, 1, 0);	
/*
*	Button detection Task
*/
void task_button(void const *argument)
{			
		
		unsigned int button_dn_cnt, button_up_cnt, double_button;
		message_t *message_led;
		message_led = (message_t*)osPoolAlloc(task_led_pool); 
		for(;;) {
			button_dn_cnt = 0;
			button_up_cnt = 0;
			double_button = 1;
			/*check whether button pushed down or not */
			if(GPIO_ReadInputDataBit(WAKEUP_BANK,WAKEUP_BUTTEON) == KEY_ON ) 
			{	   
				/* button debounce */
				osDelay(100);	
				while(1) {
					if(GPIO_ReadInputDataBit(WAKEUP_BANK,WAKEUP_BUTTEON) == KEY_ON )  
					{	 
						osDelay(10);		
						button_dn_cnt++;
					}
					else {
						button_dn_cnt = 0;
					}
					if(button_dn_cnt > 10) {
						break;
					}
					
				}
				/*
				*	check the short or long type for button event
				*/
				button_dn_cnt = 0;
				if(GPIO_ReadInputDataBit(WAKEUP_BANK,WAKEUP_BUTTEON) == KEY_ON )  
				{	
						while(1) {
								if(GPIO_ReadInputDataBit(WAKEUP_BANK,WAKEUP_BUTTEON) == KEY_ON ) {
									button_dn_cnt++;
									osDelay(50);	
									if(button_dn_cnt > 500) {
										// here is a long time button event
										// check the power status of machine, 
										// if button event is POWER ON event, set power control pin to enable power supply
										if(uv_private.power_status == POWER_OFF) {
											GPIO_WriteBit(MAIN_PWR_BANK, MAIN_PWR_CTL, (BitAction)(POWER_ON));
											GPIO_WriteBit(MCU_PWR_BANK, MCU_PWR_CTL, (BitAction)(POWER_ON));
										}
										break;   
									}
								}
								else {
									//if button is released, exit
									break;
								}
						}					
				}
				
				if(button_dn_cnt > 500) {  //5S button
						// check the power status of machine, 
						if(uv_private.power_status == POWER_OFF) {
							// if button event is POWER ON event, set power control pin to enable power supply
							GPIO_WriteBit(MCU_PWR_BANK, MCU_PWR_CTL, (BitAction)(POWER_ON));
							GPIO_WriteBit(MAIN_PWR_BANK, MAIN_PWR_CTL, (BitAction)(POWER_ON));
							uv_private.power_status = POWER_ON;
							/*
							*	TO-DO
							* send signal to task_led_status
							*/
							message_led->canData[0] = WORK_MODE_IDLE;    
							osMessagePut(task_led_quenue, (uint32_t)message_led, osWaitForever);
						}
						else { 
							// if button event is POWER OFF event, clear power control pin to disable power supply
							uv_private.power_status = POWER_OFF;	
							GPIO_WriteBit(MCU_PWR_BANK, MCU_PWR_CTL, (BitAction)(POWER_OFF));
							osDelay(50);	
							GPIO_WriteBit(MAIN_PWR_BANK, MAIN_PWR_CTL, (BitAction)(POWER_OFF));											
						}
				}
				else {
					//to check whether double button-pushed event or not
					button_up_cnt = 0;
					//waiting for the button released
					while(1) {
							if(GPIO_ReadInputDataBit(WAKEUP_BANK,WAKEUP_BUTTEON) == KEY_OFF ) {
								button_up_cnt++;
								osDelay(10);
								if(button_up_cnt>30) break;
							}
							else {
								button_up_cnt=0;
							}
					}
					button_up_cnt = 0;
					double_button = 1;
					// check the time between two push event
					while(1) {
							if(GPIO_ReadInputDataBit(WAKEUP_BANK,WAKEUP_BUTTEON) == KEY_OFF ) {
								button_up_cnt++;
								osDelay(50);
								if(button_up_cnt>200) {  //the time is longer than 2S, so there isn't double pushed event
									break;
								}
							}
							else {  // button pushed within 2S, double key
								button_up_cnt=0;
								double_button = 2;
								break;
							}
					}
					//waiting for the button released
					if(double_button ==2) {
						while(GPIO_ReadInputDataBit(WAKEUP_BANK,WAKEUP_BUTTEON) == KEY_ON); 
					}
					message_led->canData[0] = double_button;    
					osMessagePut(task_led_quenue, (uint32_t)message_led, osWaitForever);
					
				}

				while(GPIO_ReadInputDataBit(WAKEUP_BANK,WAKEUP_BUTTEON) == KEY_ON);  
			
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
	uv_private.power_status = POWER_OFF;
	
	task_led_quenue = osMessageCreate(osMessageQ(task_led_quenue),NULL);
	task_led_pool = osPoolCreate(osPool(task_led_pool));
	
	task_buzzer_quenue = osMessageCreate(osMessageQ(task_buzzer_quenue),NULL);
	task_buzzer_pool = osPoolCreate(osPool(task_buzzer_pool));	
	
	task_button_id = osThreadCreate(osThread(task_button), NULL);  
	task_led_id = osThreadCreate(osThread(task_led_status), NULL);
	task_buzzer_id = osThreadCreate(osThread(task_buzzer), NULL);
  osDelay(osWaitForever);

}


