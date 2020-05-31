/*
*  Maro Definition
*/
#define MS_FRAC		 5
#define POWER_OFF  0
#define POWER_ON   1

#define RUN_STATE 1
#define STOP_STATE 0

#define LED_OFF  1
#define LED_ON   0

#define BUZZER_OFF 0
#define BUZZER_ON  1

#define UV_OFF 0
#define UV_ON  1

#define LAMP_OFF 0
#define LAMP_ON  1

#define IRDA_SUCCESS  0
#define IRDA_FAIL			1
/*
* LAMP ON/OFF FLAG
*/
#define LED_LAMP_ON 0x10
#define LED_LAMP_OFF 0x00
/*
* STATUS LED BLINK CYCLES WHEN SYSTEM READY TO RUN
*/
#define LED_BLINK_CNT	60

/*
* PWM DEFINITON
* TO GET 120HZ PWM FREQUENCY
*/
#define DEV_BUZZER_PERIOD   6000
#define DEV_BUZZER_PRESCALE	9

#define DEV_PWM_PERIOD   60000
#define DEV_PWM_PRESCALE	9

#define LIGHTNESS30_PWM_DUTY  10000
#define LIGHTNESS60_PWM_DUTY  35000
#define LIGHTNESS100_PWM_DUTY  59900

#define FAN_PWM_DUTY	50000
#define FAN_SPEED_HIGH 0
#define FAN_SPEED_MID  20000
#define FAN_SPEED_SLOW   40000
#define FAN_IDLE		 49900	
/*
* OUTPUT LED NUMBER
*/
#define OUTPUT_NUM 15
#define LED_STATUS_NUM 6
#define MODE_LED_NUM   3
#define BAT_STATUS_NUM 5
/*
* LED STATUS DEFINITION
*/
#define UVAIR_LED    1
#define AIR_LED    2
#define UV_LED    4
#define LAMP_LED    UVAIR_LED | AIR_LED | UV_LED

#if 0
#define LIGHTNESS30_15MIN_LED    8
#define LIGHTNESS60_30MIN_LED    0x10
#define LIGHTNESS100_FOREVER_LED   LIGHTNESS30_15MIN_LED | LIGHTNESS60_30MIN_LED// 0x20
#else
#define LIGHTNESS30_15MIN_LED    1
#define LIGHTNESS60_30MIN_LED    2
#define LIGHTNESS100_FOREVER_LED   LIGHTNESS30_15MIN_LED | LIGHTNESS60_30MIN_LED// 0x20
#endif
/*
* BUTTON AND POWER CONTROL DEFINITON
*/
#define WAKEUP_BUTTEON GPIO_Pin_0
#define WAKEUP_BANK GPIOA

#define MCU_PWR_CTL  GPIO_Pin_1
#define MCU_PWR_BANK GPIOA

#define MAIN_PWR_CTL  GPIO_Pin_2
#define MAIN_PWR_BANK GPIOA

#define POWERMODE_CONTROL		GPIO_Pin_2
#define POWERMODE_CONTROL_BANK GPIOB

#define UVLAMP_CONTROL		GPIO_Pin_1
#define UVLAMP_CONTROL_BANK GPIOB
/*
* MODE STATUS DEFINITION
*/
#define UVAIR_MODE GPIO_Pin_7
#define UVAIR_MODE_BANK GPIOB

#define AIR_MODE  GPIO_Pin_8
#define AIR_MODE_BANK GPIOB

#define UV_MODE  GPIO_Pin_12
#define UV_MODE_BANK GPIOA

/*
* STATUS LED DEFINITION
*/
#define SEC15_LIGHTNESS30 GPIO_Pin_5
#define SEC15_LIGHTNESS30_BANK GPIOB

#define SEC30_LIGHTNESS60 GPIO_Pin_6
#define SEC30_LIGHTNESS60_BANK GPIOB

#define FOREVER_LIGHTNESS100 GPIO_Pin_12
#define FOREVER_LIGHTNESS100_BANK GPIOB

/*
* BAT LED DEFINITION
*/
#define BAT0 GPIO_Pin_9
#define BAT0_BANK GPIOB

#define BAT1 GPIO_Pin_13
#define BAT1_BANK GPIOC

#define BAT2 GPIO_Pin_14
#define BAT2_BANK GPIOC

#define BAT3 GPIO_Pin_15
#define BAT3_BANK GPIOC

#define BAT4 GPIO_Pin_11
#define BAT4_BANK GPIOA

/*
* IRDA DETECTION
*/
#define IRDA GPIO_Pin_7
#define IRDA_BANK GPIOA
/*
* BUZZER 
*/
#define BUZZER GPIO_Pin_0
#define BUZZER_BANK GPIOB
/*
* BAT DETECT 
*/
#define BAT_MONITOR GPIO_Pin_3
#define BAT_MONITOR_BANK GPIOA

/*
* FAN POWER ON/OFF DETECT 
*/
#define FAN_PWR GPIO_Pin_4
#define FAN_PWR_BANK GPIOA

/*
* WORK MODE DEFINITION
*/
#define WORK_MODE_IDLE			0
#define WORK_MODE_UVAIR			1
#define WORK_MODE_AIR				2
#define WORK_MODE_UV				3
#define WORK_MODE_LAMP			4

#define TIME_IDLE						0
#define TIME_15MIN					1
#define TIME_30MIN					2
#define TIME_FOREVER				3

#define LIGHTNESS_IDLE			0
#define LIGHTNESS_30				1
#define LIGHTNESS_60				2
#define LIGHTNESS_100				3



struct IO_MAP{
	GPIO_TypeDef* IOBANK;
	int IONUM;
};

struct myprivate{
	unsigned char power_status;
	unsigned char work_mode;
	unsigned char led_light_mode;
	unsigned char work_time;
	unsigned char lightness;
	unsigned char fan_state;
	unsigned int fan_target_speed;
	unsigned int fan_cur_speed;
	unsigned char uv_state;
	unsigned char uvdensity;
	unsigned int beep_freq;
	unsigned int beep_cnt;
	unsigned int beep_interval1;
	unsigned int beep_interval2;
	unsigned int beep_times1;
	unsigned int beep_times2;
};

