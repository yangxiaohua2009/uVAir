/******************* *******************
 * �ļ���  ��led.c
 * ����    ��led Ӧ�ú�����
 *          
 * ʵ��ƽ̨��MINI STM32������ ����STM32F103C8T6
 * Ӳ�����ӣ�-----------------
 *          |   PB13 - LED1   |
 *          |      |
 *          |                 |
 *           ----------------- 
 * ��汾  ��ST3.0.0   																										  

*********************************************************/
#include "led.h"
#include "uvdev.h"

const struct IO_MAP LED_CTL_TABLE[OUTPUT_NUM] ={
	{UVAIR_MODE_BANK, UVAIR_MODE},
	{AIR_MODE_BANK, AIR_MODE},
	{UV_MODE_BANK, UV_MODE},
	{SEC15_LIGHTNESS30_BANK, SEC15_LIGHTNESS30},
	{SEC30_LIGHTNESS60_BANK, SEC30_LIGHTNESS60},
	{FOREVER_LIGHTNESS100_BANK, FOREVER_LIGHTNESS100},
	{BAT0_BANK, BAT0},
	{BAT1_BANK, BAT1},
	{BAT2_BANK, BAT2},
	{BAT3_BANK, BAT3},
	{BAT4_BANK, BAT4},	
	{BUZZER_BANK, BUZZER},
	{POWERMODE_CONTROL_BANK, POWERMODE_CONTROL},
	{UVLAMP_CONTROL_BANK,UVLAMP_CONTROL},
	{FAN_PWR_BANK,FAN_PWR},
};

/***************  ����LED�õ���I/O�� *******************/
void LED_GPIO_Config(void)	
{
	unsigned char cnt = 0;
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE); // ʹ��PC�˿�ʱ��  
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE); // ʹ��PA�˿�ʱ��  
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE); // ʹ��PB�˿�ʱ��  
	for(cnt = 0; cnt < OUTPUT_NUM; cnt++)
	{
		GPIO_InitStructure.GPIO_Pin = LED_CTL_TABLE[cnt].IONUM;	
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(LED_CTL_TABLE[cnt].IOBANK, &GPIO_InitStructure);  //��ʼ��PC�˿�
		GPIO_ResetBits(LED_CTL_TABLE[cnt].IOBANK, LED_CTL_TABLE[cnt].IONUM);	 // �ر�����LED
	}
}

void LED_Round_robin_blink(unsigned first_index, unsigned end_index, unsigned char status)
{
	unsigned char cnt = 0;
	for(cnt=first_index; cnt < end_index; cnt++)
	{
		if(status & 0x01) {
			GPIO_WriteBit(LED_CTL_TABLE[cnt].IOBANK, LED_CTL_TABLE[cnt].IONUM, (BitAction)(0));
		}
		else {
			GPIO_WriteBit(LED_CTL_TABLE[cnt].IOBANK, LED_CTL_TABLE[cnt].IONUM, (BitAction)(1));
		}
		status = status >> 1;
	}
}


