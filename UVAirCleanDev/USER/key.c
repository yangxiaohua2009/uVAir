/*************************************
 * �ļ���  ��key.c
 * ����    ������Ӧ�ú�����        
 * ʵ��ƽ̨��MINI STM32������ ����STM32F103C8T6
 * Ӳ�����ӣ� PA0 - key1          
 * ��汾  ��ST3.0.0 

**********************************************************************************/

#include "key.h" 
#include "uvdev.h"
#include "cmsis_os.h"    
 /*����ȷ����ʱ */
 void Delay(__IO u32 nCount)
{
  for(; nCount != 0; nCount--);
} 

 /*	 ���ð����õ���I/O�� */
void Key_GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE); //���������˿�PA��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); //���������˿�PB��ʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE); //���������˿�PC��ʱ��
	
	GPIO_InitStructure.GPIO_Pin = WAKEUP_BUTTEON; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //�˿�����Ϊ��������
	GPIO_Init(WAKEUP_BANK, &GPIO_InitStructure);	//��ʼ���˿�
	
	GPIO_InitStructure.GPIO_Pin = MCU_PWR_CTL; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //�˿�����Ϊ��������
	GPIO_Init(MCU_PWR_BANK, &GPIO_InitStructure);	//��ʼ���˿�
	
	GPIO_InitStructure.GPIO_Pin = MAIN_PWR_CTL; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //�˿�����Ϊ��������
	GPIO_Init(MAIN_PWR_BANK, &GPIO_InitStructure);	//��ʼ���˿�	

	GPIO_InitStructure.GPIO_Pin = IRDA; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //�˿�����Ϊ��������
	GPIO_Init(IRDA_BANK, &GPIO_InitStructure);	//��ʼ���˿�

}
