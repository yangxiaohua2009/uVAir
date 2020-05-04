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
#include "pwm_output.h"
#include "adc.h"
#include "stm32f10x_tim.h"
#include "timer_test.h"

extern __IO u16 ADC_ConvertedValue;	

struct myprivate uv_private;

typedef struct {
	char canData[10];
} message_t;

osThreadId task_button_id;                    /* assigned task id of task: t_uart_rx_id  */
osThreadId task_dispatcher_id; 
osThreadId task_buzzer_id; 
osThreadId task_air_id; 
osThreadId task_uv_id; 
osThreadId task_lamp_id; 
osThreadId task_bat_monitor_id;

osPoolDef(task_dispatcher_pool, 3, message_t); 
osPoolId task_dispatcher_pool;
osMessageQDef(task_dispatcher_quenue, 3, message_t); 
osMessageQId task_dispatcher_quenue;
osEvent task_dispatcher_evt;

osPoolDef(task_buzzer_pool, 3, message_t); 
osPoolId task_buzzer_pool;
osMessageQDef(task_buzzer_quenue, 3, message_t); 
osMessageQId task_buzzer_quenue;
osEvent task_buzzer_evt;

osPoolDef(task_air_pool, 3, message_t); 
osPoolId task_air_pool;
osMessageQDef(task_air_quenue, 3, message_t); 
osMessageQId task_air_quenue;
osEvent task_air_evt;

osPoolDef(task_uv_pool, 3, message_t); 
osPoolId task_uv_pool;
osMessageQDef(task_uv_quenue, 3, message_t); 
osMessageQId task_uv_quenue;
osEvent task_uv_evt;

osPoolDef(task_lamp_pool, 3, message_t); 
osPoolId task_lamp_pool;
osMessageQDef(task_lamp_quenue, 3, message_t); 
osMessageQId task_lamp_quenue;
osEvent task_lamp_evt;



/*
* 功能执行
*/
void perform(unsigned char state)
{
		message_t *message_air;
		message_t *message_uv;
		message_t *message_lamp;
		message_t *message_buzzer;	
		message_air = (message_t*)osPoolAlloc(task_air_pool);	
		message_lamp = (message_t*)osPoolAlloc(task_lamp_pool);	
		message_buzzer = (message_t*)osPoolAlloc(task_buzzer_pool);		
		message_uv = (message_t*)osPoolAlloc(task_uv_pool);	
	
		switch(uv_private.work_mode & 0x0f)
		{
			case WORK_MODE_IDLE:
					message_buzzer->canData[0] = uv_private.work_mode;    
					osMessagePut(task_buzzer_quenue, (uint32_t)message_buzzer, 0);	
					osSignalWait(1, osWaitForever);
				break;
			case WORK_MODE_UVAIR:
					message_buzzer->canData[0] = uv_private.work_mode; 
					message_buzzer->canData[1] = state;
					osMessagePut(task_buzzer_quenue, (uint32_t)message_buzzer, 0);
					osSignalWait(1, osWaitForever);
					osPoolFree(task_buzzer_pool, message_buzzer);
			
					if(uv_private.fan_state == STOP_STATE) {
						message_air->canData[0] = uv_private.work_mode; 
						message_air->canData[1] = state;
						osMessagePut(task_air_quenue, (uint32_t)message_air, 0);	
						osSignalWait(1, osWaitForever);	
						osPoolFree(task_air_pool, message_air);	
					}	
					if(uv_private.uv_state == STOP_STATE) {					
						message_uv->canData[0] = uv_private.work_mode; 
						message_uv->canData[1] = state;
						osMessagePut(task_uv_quenue, (uint32_t)message_uv, 0);	
						osSignalWait(1, osWaitForever);
						osPoolFree(task_uv_pool, message_uv);	
					}
					message_lamp->canData[0] = uv_private.work_mode; 
					message_lamp->canData[1] = uv_private.led_light_mode;
					message_lamp->canData[2] = uv_private.lightness;
					osMessagePut(task_lamp_quenue, (uint32_t)message_lamp, 0);	
					osSignalWait(1, osWaitForever);	
					osPoolFree(task_lamp_pool, message_lamp);	
				break;
			case WORK_MODE_AIR:
					message_buzzer->canData[0] = uv_private.work_mode; 
					message_buzzer->canData[1] = state;
					osMessagePut(task_buzzer_quenue, (uint32_t)message_buzzer, 0);
					osSignalWait(1, osWaitForever);
					osPoolFree(task_buzzer_pool, message_buzzer);	
			
					if(uv_private.fan_state == STOP_STATE) {
						message_air->canData[0] = uv_private.work_mode; 
						message_air->canData[1] = state;
						osMessagePut(task_air_quenue, (uint32_t)message_air, 0);	
						osSignalWait(1, osWaitForever);	
						osPoolFree(task_air_pool, message_air);								
					}				
					message_uv->canData[0] = uv_private.work_mode; 
					message_uv->canData[1] = STOP_STATE;
					osMessagePut(task_uv_quenue, (uint32_t)message_uv, 0);	
					osSignalWait(1, osWaitForever);		
					osPoolFree(task_uv_pool, message_uv);		
					
					message_lamp->canData[0] = uv_private.work_mode; 
					message_lamp->canData[1] = uv_private.led_light_mode;
					message_lamp->canData[2] = uv_private.lightness;
					osMessagePut(task_lamp_quenue, (uint32_t)message_lamp, 0);			
					osSignalWait(1, osWaitForever);		
					osPoolFree(task_lamp_pool, message_lamp);							
				break;
#if 1
			case WORK_MODE_UV:
					message_buzzer->canData[0] = uv_private.work_mode; 
					message_buzzer->canData[1] = state;
					osMessagePut(task_buzzer_quenue, (uint32_t)message_buzzer, 0);
					osSignalWait(1, osWaitForever);
					osPoolFree(task_buzzer_pool, message_buzzer);	
					message_air->canData[0] = uv_private.work_mode; 
					message_air->canData[1] = STOP_STATE;
					osMessagePut(task_air_quenue, (uint32_t)message_air, 0);	
					osSignalWait(1, osWaitForever);		
					osPoolFree(task_air_pool, message_air);	
			
					if(uv_private.uv_state == STOP_STATE) {					
						message_uv->canData[0] = uv_private.work_mode; 
						message_uv->canData[1] = state;
						osMessagePut(task_uv_quenue, (uint32_t)message_uv, 0);	
						osSignalWait(1, osWaitForever);
						osPoolFree(task_uv_pool, message_uv);							
					}
					message_lamp->canData[0] = uv_private.work_mode; 
					message_lamp->canData[1] = uv_private.led_light_mode;
					message_lamp->canData[2] = uv_private.lightness;
					osMessagePut(task_lamp_quenue, (uint32_t)message_lamp, 0);	
					osSignalWait(1, osWaitForever);		
					osPoolFree(task_lamp_pool, message_lamp);						
				break;
#endif
			case WORK_MODE_LAMP:
					message_buzzer->canData[0] = uv_private.work_mode; 
					message_buzzer->canData[1] = state;
					osMessagePut(task_buzzer_quenue, (uint32_t)message_buzzer, 0);
					osSignalWait(1, osWaitForever);				
					message_lamp->canData[0] = uv_private.work_mode; 
					message_lamp->canData[1] = uv_private.led_light_mode;
					message_lamp->canData[2] = uv_private.lightness;
					osMessagePut(task_lamp_quenue, (uint32_t)message_lamp, 0);	
					osSignalWait(1, osWaitForever);
					osPoolFree(task_lamp_pool, message_lamp);	
				break;
			default:
				break;
		}
	
}
void timer_callback(void const *param)
{
	perform(STOP_STATE);
}
/*
*	切换工作时间或照明亮度
*/
void switch_wtime_or_lightness(void)
{
	switch(uv_private.work_mode & 0x0f)
	{
		case WORK_MODE_IDLE:	//开机巡检模式
			uv_private.work_time = TIME_IDLE;
			break;
		case WORK_MODE_UVAIR: //紫外消杀
		case WORK_MODE_AIR: //通风净化
		case WORK_MODE_UV: //紫外直照
			switch(uv_private.work_time) 
			{
				case TIME_IDLE:   //开机巡检状态
					uv_private.work_time = TIME_15MIN;
					break;
				case TIME_15MIN:   //工作15min
					uv_private.work_time = TIME_30MIN;
					break;
				case TIME_30MIN:   //工作30min
					uv_private.work_time = TIME_FOREVER;
					break;
				case TIME_FOREVER:		//一直工作
					uv_private.work_time = TIME_15MIN;
					break;
				default:
					uv_private.work_time = TIME_IDLE;
					break;
			}
			break;
		case WORK_MODE_LAMP: //台灯
			switch(uv_private.lightness) 
			{
				case LIGHTNESS_IDLE:   //开机巡检状态, 照明关闭
					uv_private.lightness = LIGHTNESS_30;
					uv_private.led_light_mode = LED_LAMP_ON;
					break;
				case LIGHTNESS_30:   //照明开启，亮度30%
					uv_private.lightness = LIGHTNESS_60;
					uv_private.led_light_mode = LED_LAMP_ON;
				break;
				case LIGHTNESS_60:   //照明开启，亮度50%
					uv_private.lightness = LIGHTNESS_100;
					uv_private.led_light_mode = LED_LAMP_ON;
					break;
				case LIGHTNESS_100:		////照明开启，亮度100%
					uv_private.lightness = LIGHTNESS_IDLE;
					uv_private.led_light_mode = LED_LAMP_OFF;
					break;
				default:
					uv_private.lightness = LIGHTNESS_IDLE;
					uv_private.led_light_mode = LED_LAMP_OFF;
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
		case WORK_MODE_IDLE:   //开机巡检模式
			uv_private.work_mode = (uv_private.led_light_mode |  WORK_MODE_UVAIR); //跳转到紫外通风消杀模式
			break;
		case WORK_MODE_UVAIR:   //紫外消杀
			uv_private.work_mode =  (uv_private.led_light_mode | WORK_MODE_AIR); //跳转到通风净化模式
			break;
		case WORK_MODE_AIR:   //通风净化
			uv_private.work_mode = 	 (uv_private.led_light_mode | WORK_MODE_UV); //跳转到紫外直照
			break;
		case WORK_MODE_UV:   //紫外直照
			/*
			if(uv_private.led_light_mode == 0) {
				uv_private.led_light_mode  = LED_LAMP_ON;
			}
			else {
				uv_private.led_light_mode  = LED_LAMP_OFF;
			}
			*/
			uv_private.work_mode =  (uv_private.led_light_mode | WORK_MODE_LAMP); //跳转到台灯
			break;		
		case WORK_MODE_LAMP:   //台灯
			uv_private.work_mode =  (uv_private.led_light_mode | WORK_MODE_UVAIR); //跳转到紫外通风消杀模式
			break;	
		default:
			uv_private.work_mode = WORK_MODE_IDLE; //跳转到开机巡检
			break;	
	}
}
void task_bat_monitor(void const *argument)
{
	u32 AD_value;	
	for(;;)
	{
		 AD_value  = 3000000/4096*ADC_ConvertedValue/1000;
		if(AD_value >= 0xB20) {
			GPIO_WriteBit(BAT0_BANK, BAT0, (BitAction)(LED_ON));
			GPIO_WriteBit(BAT1_BANK, BAT1, (BitAction)(LED_ON));
			GPIO_WriteBit(BAT2_BANK, BAT2, (BitAction)(LED_ON));
			GPIO_WriteBit(BAT3_BANK, BAT3, (BitAction)(LED_ON));
			GPIO_WriteBit(BAT4_BANK, BAT4, (BitAction)(LED_ON));
		}
		if(AD_value > 0xAA0 && AD_value < 0xB20) {
			GPIO_WriteBit(BAT0_BANK, BAT0, (BitAction)(LED_ON));
			GPIO_WriteBit(BAT1_BANK, BAT1, (BitAction)(LED_ON));
			GPIO_WriteBit(BAT2_BANK, BAT2, (BitAction)(LED_ON));
			GPIO_WriteBit(BAT3_BANK, BAT3, (BitAction)(LED_ON));
			GPIO_WriteBit(BAT4_BANK, BAT4, (BitAction)(LED_OFF));
		}	
		if(AD_value >= 0xA20 && AD_value < 0xAA0) {
			GPIO_WriteBit(BAT0_BANK, BAT0, (BitAction)(LED_ON));
			GPIO_WriteBit(BAT1_BANK, BAT1, (BitAction)(LED_ON));
			GPIO_WriteBit(BAT2_BANK, BAT2, (BitAction)(LED_ON));
			GPIO_WriteBit(BAT3_BANK, BAT3, (BitAction)(LED_OFF));
			GPIO_WriteBit(BAT4_BANK, BAT4, (BitAction)(LED_OFF));
		}	
		if(AD_value >= 0x990 && AD_value < 0xA20) {
			GPIO_WriteBit(BAT0_BANK, BAT0, (BitAction)(LED_ON));
			GPIO_WriteBit(BAT1_BANK, BAT1, (BitAction)(LED_ON));
			GPIO_WriteBit(BAT2_BANK, BAT2, (BitAction)(LED_OFF));
			GPIO_WriteBit(BAT3_BANK, BAT3, (BitAction)(LED_OFF));
			GPIO_WriteBit(BAT4_BANK, BAT4, (BitAction)(LED_OFF));
		}		
		if(AD_value >= 0x910 && AD_value < 0x990) {
			GPIO_WriteBit(BAT0_BANK, BAT0, (BitAction)(LED_ON));
			GPIO_WriteBit(BAT1_BANK, BAT1, (BitAction)(LED_OFF));
			GPIO_WriteBit(BAT2_BANK, BAT2, (BitAction)(LED_OFF));
			GPIO_WriteBit(BAT3_BANK, BAT3, (BitAction)(LED_OFF));
			GPIO_WriteBit(BAT4_BANK, BAT4, (BitAction)(LED_OFF));
		}	
		if(AD_value >= 0x880 && AD_value < 0x910) {
			GPIO_WriteBit(BAT0_BANK, BAT0, (BitAction)(LED_OFF));
			GPIO_WriteBit(BAT1_BANK, BAT1, (BitAction)(LED_OFF));
			GPIO_WriteBit(BAT2_BANK, BAT2, (BitAction)(LED_OFF));
			GPIO_WriteBit(BAT3_BANK, BAT3, (BitAction)(LED_OFF));
			GPIO_WriteBit(BAT4_BANK, BAT4, (BitAction)(LED_OFF));
		}		
		if(AD_value < 0x880) {
			GPIO_WriteBit(BAT0_BANK, BAT0, (BitAction)(LED_OFF));
			GPIO_WriteBit(BAT1_BANK, BAT1, (BitAction)(LED_OFF));
			GPIO_WriteBit(BAT2_BANK, BAT2, (BitAction)(LED_OFF));
			GPIO_WriteBit(BAT3_BANK, BAT3, (BitAction)(LED_OFF));
			GPIO_WriteBit(BAT4_BANK, BAT4, (BitAction)(LED_OFF));			
		}
		osDelay(1000 * MS_FRAC);
	}
}
osThreadDef(task_bat_monitor, osPriorityLow, 1, 0);	
/*
*	lamp Task
*/
void task_lamp(void const *argument)
{
	message_t *message_lamp;
	message_t *message;
	message_lamp = (message_t*)osPoolAlloc(task_lamp_pool);	
	for(;;)
	{
		task_lamp_evt = osMessageGet(task_lamp_quenue, osWaitForever);
		if(task_lamp_evt.status == osEventMessage) {
			message = (message_t*)task_lamp_evt.value.p;
			
		  if( ((uint32_t)message->canData[1]) == LED_LAMP_ON){
				switch(((uint32_t)message->canData[2]))
				{
					case LIGHTNESS_IDLE:
						TM1_PWM1_LIGHTNESS_ENABLE(POWER_OFF);
						break;
					case LIGHTNESS_30:
						TM1_PWM1_LIGHTNESS_ENABLE(POWER_ON);
						TIM1->CCR2 = LIGHTNESS30_PWM_DUTY;
						break;
					case LIGHTNESS_60:
						TM1_PWM1_LIGHTNESS_ENABLE(POWER_ON);
						TIM1->CCR2 = LIGHTNESS60_PWM_DUTY;						
						break;
					case LIGHTNESS_100:
						TM1_PWM1_LIGHTNESS_ENABLE(POWER_ON);
						TIM1->CCR2 = LIGHTNESS100_PWM_DUTY;							
						break;
					default:
						TM1_PWM1_LIGHTNESS_ENABLE(POWER_OFF);
						break;
				}
			}
			else {
				TM1_PWM1_LIGHTNESS_ENABLE(POWER_OFF);
			}
			osSignalSet(task_dispatcher_id, 1);
			osPoolFree(task_lamp_pool, message_lamp);	
		}	
		osDelay(150 * MS_FRAC);
	}
}
osThreadDef(task_lamp, osPriorityBelowNormal, 1, 0);	
/*
*	uv Task
*/
void task_uv(void const *argument)
{
	unsigned char modetmp, statetmp;
	message_t *message_uv;
	message_t *message;
	message_uv = (message_t*)osPoolAlloc(task_uv_pool);	
	for(;;)
	{
		task_uv_evt = osMessageGet(task_uv_quenue, 0);
		if(task_uv_evt.status == osEventMessage) {
			message = (message_t*)task_uv_evt.value.p;
			modetmp = (uint32_t)message->canData[0] & 0x0f;
			statetmp = (uint32_t)message->canData[1];
			osSignalSet(task_dispatcher_id, 1);
			osPoolFree(task_uv_pool, message_uv);		
		}	
		switch(modetmp) 
		{
			case WORK_MODE_UVAIR:
			case WORK_MODE_UV:	
				/*
				*	ON/OFF UV 
				*/	
				if(GPIO_ReadInputDataBit(IRDA_BANK,IRDA) == IRDA_SUCCESS) { //if there are persons nearby the dev.
					GPIO_WriteBit(UVLAMP_CONTROL_BANK, UVLAMP_CONTROL, (BitAction)(UV_OFF));
				}
				else {
					if(statetmp == RUN_STATE) {
						GPIO_WriteBit(UVLAMP_CONTROL_BANK, UVLAMP_CONTROL, (BitAction)(UV_ON));
					}
					else {
						GPIO_WriteBit(UVLAMP_CONTROL_BANK, UVLAMP_CONTROL, (BitAction)(UV_OFF));
					}
				}
				break;
			case WORK_MODE_AIR:
				GPIO_WriteBit(UVLAMP_CONTROL_BANK, UVLAMP_CONTROL, (BitAction)(UV_OFF));
				break;
			default:
				break;
		}	
		osDelay(100 * MS_FRAC);
	}
}
osThreadDef(task_uv, osPriorityAboveNormal, 1, 0);	
/*
*	Air Task
*/
void task_air(void const *argument)
{
	unsigned char modetmp, statetmp, run;
	unsigned startv = 55000;
	message_t *message_air;
	message_t *message;
	message_air = (message_t*)osPoolAlloc(task_air_pool);	
	for(;;)
	{
		task_air_evt = osMessageGet(task_air_quenue, 0);
		if(task_air_evt.status == osEventMessage) {
			message = (message_t*)task_air_evt.value.p;
			modetmp = (uint32_t)message->canData[0] & 0x0f;
			statetmp = (uint32_t)message->canData[1];
			run = 0;
			if(statetmp == RUN_STATE) {
				startv = 55000;
			}
			osSignalSet(task_dispatcher_id, 1);
			osPoolFree(task_air_pool, message_air);		
		}	
		switch(modetmp) 
		{
			case WORK_MODE_UVAIR:
			case WORK_MODE_AIR:	
				/*
				*	ON/OFF the FAN
				*/			
				if(statetmp == RUN_STATE) {
					if(run == 0) {
						TM1_PWM1_FAN_ENABLE(POWER_ON);
						run = 1;
					}
					else {
						if(startv >0) {
							TIM1->CCR1 = startv;
							startv-=500;
						}
					}
				}
				else {
					if(run == 0) {
						TIM1->CCR1 = startv;
						startv+=500;
						if(startv == 55000) {
							run = 1;	
							TM1_PWM1_FAN_ENABLE(POWER_OFF);
						}
					}
				}
			default:
				break;
		}	
		osDelay(100 * MS_FRAC);
	}
}
osThreadDef(task_air, osPriorityNormal, 1, 0);	
/*
*	Buzzer Task
*/
void task_buzzer(void const *argument)
{
	unsigned char cnt, cnt1, workmode, state;
	message_t *message_buzzer;
	message_t *message;
	message_buzzer = (message_t*)osPoolAlloc(task_buzzer_pool);	
	for(;;)
	{
		#if 0
		task_buzzer_evt = osMessageGet(task_buzzer_quenue, 0);		
		#else
		task_buzzer_evt = osMessageGet(task_buzzer_quenue, osWaitForever);
		#endif
		if(task_buzzer_evt.status == osEventMessage) {
			message = (message_t*)task_buzzer_evt.value.p;
			cnt = 0;
			cnt1 = 0;
			workmode = (uint32_t)message->canData[0] & 0x0f;
			state = (uint32_t)message->canData[1];
			osSignalSet(task_dispatcher_id, 1);
			osPoolFree(task_buzzer_pool, message_buzzer);		
			STOP_TIME;
			osDelay(500 * MS_FRAC);
		}
		//TIM3->CCR1 = 3000;
			switch(workmode)
			{
				case WORK_MODE_IDLE: //idle mode
					//for(cnt = 0; cnt < 100; cnt++) {
				#if 0
						if(cnt < 100) {
							GPIO_WriteBit(BUZZER_BANK, BUZZER, (BitAction)(BUZZER_ON));
							osDelay(1 * MS_FRAC);
							GPIO_WriteBit(BUZZER_BANK, BUZZER, (BitAction)(BUZZER_OFF));
							osDelay(1 * MS_FRAC);
							cnt++;
						}
						else {
							osDelay(10 * MS_FRAC);
						}
				#else
					//	TM3_PWM3_BEEP_ENABLE(POWER_ON);
					//}
					uv_private.beep_freq = 500;
					uv_private.beep_cnt = 150;
					uv_private.beep_interval1 = 10000;
					uv_private.beep_interval2 = 5000;
					uv_private.beep_times1 = 0;
					uv_private.beep_times2 = 0;
					TIM2_Configuration(500);
					START_TIME;	
				#endif
					break;
				case WORK_MODE_UVAIR: //idle mode
				case WORK_MODE_UV: //idle mode	
					#if 0
					if(state == RUN_STATE) {
						if(cnt1 < 30) {
							if(cnt < 40) {
								cnt++;
								GPIO_WriteBit(BUZZER_BANK, BUZZER, (BitAction)(BUZZER_ON));
								osDelay(1 * MS_FRAC);
								GPIO_WriteBit(BUZZER_BANK, BUZZER, (BitAction)(BUZZER_OFF));
								osDelay(1 * MS_FRAC);								
							}
							else {
								cnt1++;
								cnt = 0;
								if(cnt1<15)
									osDelay(400 * MS_FRAC);
								else
									osDelay(200 * MS_FRAC);
							}
						}
						else {
							osDelay(10 * MS_FRAC);
						}
					}
					else {
						if(cnt < 100) {
							GPIO_WriteBit(BUZZER_BANK, BUZZER, (BitAction)(BUZZER_ON));
							osDelay(1 * MS_FRAC);
							GPIO_WriteBit(BUZZER_BANK, BUZZER, (BitAction)(BUZZER_OFF));
							osDelay(1 * MS_FRAC);
							cnt++;
						}
						else {
							osDelay(10 * MS_FRAC);
						}
					}
					#endif
					//TM3_PWM3_BEEP_ENABLE(POWER_ON);
					uv_private.beep_freq = 500;
					uv_private.beep_cnt = 150;
					uv_private.beep_interval1 = 65000;
					uv_private.beep_interval2 = 40000;
					uv_private.beep_times1 = 120;
					uv_private.beep_times2 = 60;
					TIM2_Configuration(500);
					START_TIME;	
					break;	
				case WORK_MODE_AIR: //idle mode
					#if 0
					if(state == RUN_STATE) {
						if(cnt < 100) {
							GPIO_WriteBit(BUZZER_BANK, BUZZER, (BitAction)(BUZZER_ON));
							osDelay(1 * MS_FRAC);
							GPIO_WriteBit(BUZZER_BANK, BUZZER, (BitAction)(BUZZER_OFF));
							osDelay(1 * MS_FRAC);
							cnt++;
						}
						else {
							osDelay(10 * MS_FRAC);
						}
					}
					else {
						if(cnt < 100) {
							GPIO_WriteBit(BUZZER_BANK, BUZZER, (BitAction)(BUZZER_ON));
							osDelay(1 * MS_FRAC);
							GPIO_WriteBit(BUZZER_BANK, BUZZER, (BitAction)(BUZZER_OFF));
							osDelay(1 * MS_FRAC);
							cnt++;
						}
						else {
							osDelay(10 * MS_FRAC);
						}
					}
					#else
					//TM3_PWM3_BEEP_ENABLE(POWER_ON);
					uv_private.beep_freq = 500;
					uv_private.beep_cnt = 150;
					uv_private.beep_interval1 = 10000;
					uv_private.beep_interval2 = 5000;
					uv_private.beep_times1 = 0;
					uv_private.beep_times2 = 0;
					TIM2_Configuration(500);
					START_TIME;	
					#endif
					break;
				case WORK_MODE_LAMP: //idle mode
					#if 0
					if(state == RUN_STATE) {
						if(cnt < 100) {
							GPIO_WriteBit(BUZZER_BANK, BUZZER, (BitAction)(BUZZER_ON));
							osDelay(1 * MS_FRAC/2);
							GPIO_WriteBit(BUZZER_BANK, BUZZER, (BitAction)(BUZZER_OFF));
							osDelay(1 * MS_FRAC/2);
							cnt++;
						}
						else {
							osDelay(10 * MS_FRAC);
						}
					}
					else {
						if(cnt < 100) {
							GPIO_WriteBit(BUZZER_BANK, BUZZER, (BitAction)(BUZZER_ON));
							osDelay(1 * MS_FRAC/2);
							GPIO_WriteBit(BUZZER_BANK, BUZZER, (BitAction)(BUZZER_OFF));
							osDelay(1 * MS_FRAC/2);
							cnt++;
						}
						else {
							osDelay(10 * MS_FRAC);
						}
					}			
					#else
					//TM3_PWM3_BEEP_ENABLE(POWER_ON);
					uv_private.beep_freq = 500;
					uv_private.beep_cnt = 150;
					uv_private.beep_interval1 = 10000;
					uv_private.beep_interval2 = 5000;
					uv_private.beep_times1 = 0;
					uv_private.beep_times2 = 0;
					TIM2_Configuration(500);
					START_TIME;					
					#endif
					break;
				default:
					break;
			}		
			osDelay(2 * MS_FRAC);			
	}
}
osThreadDef(task_buzzer, osPriorityNormal, 1, 0);	
/*
*	Button detection Task
*/
void task_dispatcher(void const *argument)
{
	unsigned char button_type, cnt, para, status, old_mode_status, old_status;
	unsigned char run;
	unsigned int delaytime;
	message_t *message_dispatcher;
	message_t *message;
	//osTimerDef(timer0_handle, timer_callback);	
	//osTimerId timer0 = osTimerCreate(osTimer(timer0_handle), osTimerOnce, (void *)0);
	
	message_dispatcher = (message_t*)osPoolAlloc(task_dispatcher_pool);	

	
	status = 1;
	old_mode_status = 0;
	old_status = 0;
	para = 0;
	button_type = 0x0;
	run = 0;
	for(;;)
	{
		task_dispatcher_evt = osMessageGet(task_dispatcher_quenue, 10);
		if(task_dispatcher_evt.status == osEventMessage) {
			message = (message_t*)task_dispatcher_evt.value.p;
			button_type = (uint32_t)message->canData[0];
			osSignalSet(task_button_id, 1);
			switch(button_type)
			{
				case 0:     //power on just now
					perform(RUN_STATE);
					status = 1;
					old_mode_status = 0;
					break;
				case 2:     //single button
					switch_workmode();
					break;
				case 1:     //double button
					switch_wtime_or_lightness();				
					break;
				default:
					
					break;
				
			}
			switch(uv_private.work_mode & 0x0f) {
				case WORK_MODE_IDLE:   //开机巡检模式
					status = 0;
					old_mode_status = 0;
					break;
				case WORK_MODE_UVAIR:   //紫外消杀
					status = UVAIR_LED;
					old_mode_status = UVAIR_LED;
					break;
				case WORK_MODE_AIR:   //通风净化
					status = AIR_LED;
					old_mode_status = AIR_LED;
					break;
				case WORK_MODE_UV:   //紫外直照
					status = UV_LED;
					old_mode_status = UV_LED;
					break;		
				case WORK_MODE_LAMP:   //台灯
					status = LAMP_LED;
					old_mode_status = LAMP_LED;
					break;	
				default:
					status = 0;
					old_mode_status = 0;
					break;	
			}		
			if((uv_private.work_mode & 0x0f) == WORK_MODE_LAMP) {
				para = uv_private.lightness;
			}
			else {
				para = uv_private.work_time;
			}
			switch(para) {
				case TIME_IDLE:   //LED OFF
					status = 0;
					old_status = 0;
					break;
				case TIME_15MIN:   //15MIN OR 30% LIGHTNESS
					status = LIGHTNESS30_15MIN_LED;
					old_status = LIGHTNESS30_15MIN_LED;
					break;
				case TIME_30MIN:   //30MIN OR 60% LIGHTNESS
					status = LIGHTNESS60_30MIN_LED;
					old_status = LIGHTNESS60_30MIN_LED;
					break;
				case TIME_FOREVER:   //forever OR 100% LIGHTNESS
					status = LIGHTNESS100_FOREVER_LED;
					old_status = LIGHTNESS100_FOREVER_LED;
					break;		
				default:
					status = 0;
					old_status = 0;
					break;	
			}					
			cnt = 0;
			run = 0;
			osPoolFree(task_dispatcher_pool, message_dispatcher);
		}
		switch(button_type)
		{
			case 0: //idle mode					

				LED_Round_robin_blink(0, LED_STATUS_NUM, status);		
				cnt++;
				if(cnt < LED_STATUS_NUM) {
					
					status = status << 1;
				}
				else {
					cnt = 0;
					status = 1;
				}
				//osDelay(40 * MS_FRAC);
				break;
			case 2: //idle mode
				LED_Round_robin_blink(MODE_LED_NUM, LED_STATUS_NUM, old_status);
				if(cnt < LED_BLINK_CNT) {
					if(cnt == 0) perform(STOP_STATE);
					LED_Round_robin_blink(0, MODE_LED_NUM, status);
					status = ~status;
					cnt++;
				}
				else {
					if(run == 0) {
						LED_Round_robin_blink(0, MODE_LED_NUM, old_mode_status);
						/*
						*	TO-DO, send to signal to other task
						*/
						run = 1;	
						perform(RUN_STATE);
						GPIO_WriteBit(POWERMODE_CONTROL_BANK, POWERMODE_CONTROL, (BitAction)(POWER_ON));
						switch(uv_private.work_mode & 0x0f) {
							case WORK_MODE_IDLE:   //开机巡检模式
								uv_private.fan_state = STOP_STATE;
								uv_private.uv_state	= STOP_STATE;
								GPIO_WriteBit(FAN_PWR_BANK, FAN_PWR, (BitAction)(POWER_OFF));
								break;
							case WORK_MODE_UVAIR:   //紫外消杀
								uv_private.fan_state = RUN_STATE;
								uv_private.uv_state	= RUN_STATE;
								GPIO_WriteBit(FAN_PWR_BANK, FAN_PWR, (BitAction)(POWER_ON));
								break;
							case WORK_MODE_AIR:   //通风净化
								uv_private.fan_state = RUN_STATE;
								uv_private.uv_state	= STOP_STATE;
								GPIO_WriteBit(FAN_PWR_BANK, FAN_PWR, (BitAction)(POWER_ON));
								break;
							case WORK_MODE_UV:   //紫外直照
								uv_private.fan_state = STOP_STATE;
								uv_private.uv_state	= RUN_STATE;
								GPIO_WriteBit(FAN_PWR_BANK, FAN_PWR, (BitAction)(POWER_OFF));
								break;		
							case WORK_MODE_LAMP:   //台灯

								break;	
							default:
								break;	
						}	
						
						#if 1
						if((uv_private.work_mode & 0x0f) != WORK_MODE_LAMP) {
							switch(uv_private.work_time)
							{
								case TIME_IDLE:
									delaytime = 0;
									break;
								case TIME_15MIN:
									delaytime = 15 * 60 * 20;
									break;								
								case TIME_30MIN:
									delaytime = 30 * 60 * 20;
									break;
								case TIME_FOREVER:
									delaytime = 0;
									break;								
								default:
									break;
							}
						}
					#endif
					}
					else {
						if((uv_private.work_mode & 0x0f) != WORK_MODE_LAMP) {
							switch(uv_private.work_time)
							{
								case TIME_IDLE:
									delaytime = 0;
									break;
								case TIME_15MIN:
								case TIME_30MIN:
									if(delaytime>1) {
										delaytime--;
									}
									if(delaytime==1) 
									{
										delaytime = 0;
										perform(STOP_STATE);
										uv_private.fan_state = STOP_STATE;
										uv_private.uv_state	= STOP_STATE;
#if 0
										switch(uv_private.work_mode & 0x0f) {
											case WORK_MODE_IDLE:   //开机巡检模式
												uv_private.fan_state = STOP_STATE;
												uv_private.uv_state	= STOP_STATE;
												break;
											case WORK_MODE_UVAIR:   //紫外消杀
												uv_private.fan_state = STOP_STATE;
												uv_private.uv_state	= STOP_STATE;
												break;
											case WORK_MODE_AIR:   //通风净化
												uv_private.fan_state = STOP_STATE;
												uv_private.uv_state	= STOP_STATE;
												break;
											case WORK_MODE_UV:   //紫外直照
												uv_private.fan_state = STOP_STATE;
												uv_private.uv_state	= STOP_STATE;
												break;		
											case WORK_MODE_LAMP:   //台灯

												break;	
											default:
											break;	
										}	
#endif
									}
									break;								
								case TIME_FOREVER:
									delaytime = 0;
									break;								
								default:
									break;
							}
						}
					}
				}
				break;
			case 1: //idle mode
				LED_Round_robin_blink(0, MODE_LED_NUM, old_mode_status);
				if(cnt < LED_BLINK_CNT) {
					LED_Round_robin_blink(MODE_LED_NUM, LED_STATUS_NUM, status);
					status = ~status;
					cnt++;
				}
				else {
					if(run == 0) {
						LED_Round_robin_blink(MODE_LED_NUM, LED_STATUS_NUM, old_status);
						/*
						*	TO-DO, send to signal to other task
						*/
						run = 1;
								
						perform(RUN_STATE);
						GPIO_WriteBit(POWERMODE_CONTROL_BANK, POWERMODE_CONTROL, (BitAction)(POWER_ON));
						switch(uv_private.work_mode & 0x0f) {
							case WORK_MODE_IDLE:   //开机巡检模式
								uv_private.fan_state = STOP_STATE;
								uv_private.uv_state	= STOP_STATE;
								GPIO_WriteBit(FAN_PWR_BANK, FAN_PWR, (BitAction)(POWER_OFF));
								break;
							case WORK_MODE_UVAIR:   //紫外消杀
								uv_private.fan_state = RUN_STATE;
								uv_private.uv_state	= RUN_STATE;
								GPIO_WriteBit(FAN_PWR_BANK, FAN_PWR, (BitAction)(POWER_ON));
								break;
							case WORK_MODE_AIR:   //通风净化
								uv_private.fan_state = RUN_STATE;
								uv_private.uv_state	= STOP_STATE;
								GPIO_WriteBit(FAN_PWR_BANK, FAN_PWR, (BitAction)(POWER_ON));
								break;
							case WORK_MODE_UV:   //紫外直照
								uv_private.fan_state = STOP_STATE;
								uv_private.uv_state	= RUN_STATE;
								GPIO_WriteBit(FAN_PWR_BANK, FAN_PWR, (BitAction)(POWER_OFF));
								break;		
							case WORK_MODE_LAMP:   //台灯
								//GPIO_WriteBit(FAN_PWR_BANK, FAN_PWR, (BitAction)(POWER_OFF));
								break;	
							default:
								break;	
						}							
						#if 0
						if((uv_private.work_mode & 0x0f) != WORK_MODE_LAMP) {
							switch(uv_private.work_time)
							{
								case TIME_IDLE:
									break;
								case TIME_15MIN:
									osTimerStart(timer0,15 * 60 * 1000);
									break;								
								case TIME_30MIN:
									osTimerStart(timer0,30 * 60 * 1000);
									break;
								case TIME_FOREVER:

									break;								
								default:
									break;
							}
						}
					#endif
					}
					else {
						if((uv_private.work_mode & 0x0f) != WORK_MODE_LAMP) {
							switch(uv_private.work_time)
							{
								case TIME_IDLE:
									delaytime = 0;
									break;
								case TIME_15MIN:
								case TIME_30MIN:
									if(delaytime>1) {
										delaytime--;
									}
									if(delaytime==1) 
									{
										delaytime = 0;
										perform(STOP_STATE);
										uv_private.fan_state = STOP_STATE;
										uv_private.uv_state	= STOP_STATE;
#if 0
										switch(uv_private.work_mode & 0x0f) {
											case WORK_MODE_IDLE:   //开机巡检模式
												uv_private.fan_state = STOP_STATE;
												uv_private.uv_state	= STOP_STATE;
												break;
											case WORK_MODE_UVAIR:   //紫外消杀
												uv_private.fan_state = STOP_STATE;
												uv_private.uv_state	= STOP_STATE;
												break;
											case WORK_MODE_AIR:   //通风净化
												uv_private.fan_state = STOP_STATE;
												uv_private.uv_state	= STOP_STATE;
												break;
											case WORK_MODE_UV:   //紫外直照
												uv_private.fan_state = STOP_STATE;
												uv_private.uv_state	= STOP_STATE;
												break;		
											case WORK_MODE_LAMP:   //台灯

												break;	
											default:
											break;	
										}	
#endif										
									}
									break;								
								case TIME_FOREVER:
									delaytime = 0;
									break;								
								default:
									break;
							}
						}						
					}
				}			
				break;
			default:
				break;
		}					
		osDelay(50 * MS_FRAC);
	}
}
osThreadDef(task_dispatcher, osPriorityNormal, 1, 0);	
/*
*	Button detection Task
*/
void task_button(void const *argument)
{			
		
		unsigned int button_dn_cnt, button_up_cnt, double_button;
		message_t *message_dispatcher;
		message_dispatcher = (message_t*)osPoolAlloc(task_dispatcher_pool); 
		for(;;) {
			button_dn_cnt = 0;
			button_up_cnt = 0;
			double_button = 1;
			/*check whether button pushed down or not */
			if(GPIO_ReadInputDataBit(WAKEUP_BANK,WAKEUP_BUTTEON) == KEY_ON ) 
			{	   
				/* button debounce */
				osDelay(100 * MS_FRAC);	
				while(1) {
					if(GPIO_ReadInputDataBit(WAKEUP_BANK,WAKEUP_BUTTEON) == KEY_ON )  
					{	 
						osDelay(2 * MS_FRAC);		
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
									osDelay(10 * MS_FRAC);	
									if(button_dn_cnt > 300) {
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
				
				if(button_dn_cnt > 300) {  //5S button
						// check the power status of machine, 
						if(uv_private.power_status == POWER_OFF) {
							// if button event is POWER ON event, set power control pin to enable power supply
							GPIO_WriteBit(MCU_PWR_BANK, MCU_PWR_CTL, (BitAction)(POWER_ON));
							GPIO_WriteBit(MAIN_PWR_BANK, MAIN_PWR_CTL, (BitAction)(POWER_ON));
							GPIO_WriteBit(POWERMODE_CONTROL_BANK, POWERMODE_CONTROL, (BitAction)(POWER_OFF));
							GPIO_WriteBit(FAN_PWR_BANK, FAN_PWR, (BitAction)(POWER_OFF));
							uv_private.power_status = POWER_ON;
							/*
							*	TO-DO
							* send signal to task_dispatcher
							*/
							message_dispatcher->canData[0] = WORK_MODE_IDLE;    
							osMessagePut(task_dispatcher_quenue, (uint32_t)message_dispatcher, 0);
						}
						else { 
							// if button event is POWER OFF event, clear power control pin to disable power supply
							GPIO_WriteBit(POWERMODE_CONTROL_BANK, POWERMODE_CONTROL, (BitAction)(POWER_OFF));
							GPIO_WriteBit(FAN_PWR_BANK, FAN_PWR, (BitAction)(POWER_OFF));
							osDelay(2 * MS_FRAC);	
							uv_private.power_status = POWER_OFF;	
							GPIO_WriteBit(MCU_PWR_BANK, MCU_PWR_CTL, (BitAction)(POWER_OFF));
							osDelay(2 * MS_FRAC);	
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
								osDelay(1 * MS_FRAC);
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
								osDelay(2 * MS_FRAC);
								if(button_up_cnt>1000) {  //the time is longer than 2S, so there isn't double pushed event
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
					message_dispatcher->canData[0] = double_button;    
					osMessagePut(task_dispatcher_quenue, (uint32_t)message_dispatcher, 0);
					
				}

				while(GPIO_ReadInputDataBit(WAKEUP_BANK,WAKEUP_BUTTEON) == KEY_ON);  
				osSignalWait(1, osWaitForever);	
				osPoolFree(task_dispatcher_pool, message_dispatcher);
			
		}
		osDelay(10 * MS_FRAC);	
	}
}
osThreadDef(task_button, osPriorityHigh, 1, 0);
unsigned char BEEP_STATUS = 0;
void TIM2_IRQHandler(void)
{
	if ( TIM_GetITStatus(TIM2 , TIM_IT_Update) != RESET ) 
	{	
		TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update);
		STOP_TIME;
		if(uv_private.beep_cnt){
			//TIM2_Configuration(500);
			GPIO_WriteBit(BUZZER_BANK, BUZZER, (BitAction)(BEEP_STATUS));		
			if(BEEP_STATUS == 0 )
			{
				BEEP_STATUS = 0xff;
			}
			else
			{
				BEEP_STATUS = 0;
			}
			uv_private.beep_cnt--;
		  TIM2_Configuration(500);
			START_TIME;
		}
		else {
			if(uv_private.beep_times1) {
					uv_private.beep_cnt = 150;
					uv_private.beep_times1--;
					TIM2_Configuration(uv_private.beep_interval1);
					START_TIME;
			}
			else {
				if(uv_private.beep_times2) {
					uv_private.beep_cnt = 150;
					uv_private.beep_times2--;
					TIM2_Configuration(uv_private.beep_interval2);
					START_TIME;
				}
				else {
					STOP_TIME;
				}
			}
		}
	}		 	
}



void EXTI0_IRQHandler(void)
{
	unsigned int button_dn_cnt1, button_up_cnt1;
	EXTI->PR=1<<0;
	osDelay(100 * MS_FRAC);	
	while(1) {
		if(GPIO_ReadInputDataBit(WAKEUP_BANK,WAKEUP_BUTTEON) == KEY_ON )  
		{	 
			osDelay(2 * MS_FRAC);		
			button_dn_cnt1++;
			button_up_cnt1 = 0;
		}
		else {
			button_dn_cnt1 = 0;
			button_up_cnt1++;
		}
		if(button_dn_cnt1 > 10) {
			break;
		}
		if(button_up_cnt1 > 10) {
			break;
		}					
	}
}

int main(void)
{   
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	
  SystemInit();	// 配置系统时钟为72M 	
  LED_GPIO_Config(); //LED 端口初始化   	
  Key_GPIO_Config();//按键端口初始化
	/*
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3); 
                          
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
	EXTI_InitStructure.EXTI_Line = EXTI_Line0; 
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;  
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;  
	EXTI_Init(&EXTI_InitStructure);
	*/
	TIM1_PWM_Init();
	//TIM3_PWM_Init();
	ADC1_Init();
	uv_private.power_status = POWER_OFF;
	uv_private.work_time = TIME_15MIN;
	uv_private.lightness = LIGHTNESS_IDLE;
	uv_private.led_light_mode = LED_LAMP_OFF;	
	uv_private.fan_state = STOP_STATE;
	uv_private.uv_state = STOP_STATE;
	uv_private.beep_freq = 500;
	uv_private.beep_cnt = 150;
	uv_private.beep_interval1 = 10000;
	uv_private.beep_interval2 = 5000;
	uv_private.beep_times1 = 0;
	uv_private.beep_times2 = 0;
	task_dispatcher_quenue = osMessageCreate(osMessageQ(task_dispatcher_quenue),NULL);
	task_dispatcher_pool = osPoolCreate(osPool(task_dispatcher_pool));
	
	task_buzzer_quenue = osMessageCreate(osMessageQ(task_buzzer_quenue),NULL);
	task_buzzer_pool = osPoolCreate(osPool(task_buzzer_pool));	
	
	task_air_quenue = osMessageCreate(osMessageQ(task_air_quenue),NULL);
	task_air_pool = osPoolCreate(osPool(task_air_pool));		

	task_uv_quenue = osMessageCreate(osMessageQ(task_uv_quenue),NULL);
	task_uv_pool = osPoolCreate(osPool(task_uv_pool));		
	
	task_lamp_quenue = osMessageCreate(osMessageQ(task_lamp_quenue),NULL);
	task_lamp_pool = osPoolCreate(osPool(task_lamp_pool));		
	
	task_button_id = osThreadCreate(osThread(task_button), NULL);  
	task_dispatcher_id = osThreadCreate(osThread(task_dispatcher), NULL);
	task_buzzer_id = osThreadCreate(osThread(task_buzzer), NULL);
	task_air_id = osThreadCreate(osThread(task_air), NULL);
	task_uv_id = osThreadCreate(osThread(task_uv), NULL);	
	task_lamp_id = osThreadCreate(osThread(task_lamp), NULL);	
	task_bat_monitor_id = osThreadCreate(osThread(task_bat_monitor), NULL);	

	TIM2_NVIC_Configuration(); /* TIM2 定时配置 */
  TIM2_Configuration(2000); 	
	STOP_TIME;	 /* TIM2 开始计时 */

  osDelay(osWaitForever);

}


