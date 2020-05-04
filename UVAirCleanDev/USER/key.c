/*************************************
 * 文件名  ：key.c
 * 描述    ：按键应用函数库        
 * 实验平台：MINI STM32开发板 基于STM32F103C8T6
 * 硬件连接： PA0 - key1          
 * 库版本  ：ST3.0.0 

**********************************************************************************/

#include "key.h" 
#include "uvdev.h"
#include "cmsis_os.h"    
 /*不精确的延时 */
 void Delay(__IO u32 nCount)
{
  for(; nCount != 0; nCount--);
} 

 /*	 配置按键用到的I/O口 */
void Key_GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE); //开启按键端口PA的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); //开启按键端口PB的时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE); //开启按键端口PC的时钟
	
	GPIO_InitStructure.GPIO_Pin = WAKEUP_BUTTEON; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //端口配置为上拉输入
	GPIO_Init(WAKEUP_BANK, &GPIO_InitStructure);	//初始化端口
	
	GPIO_InitStructure.GPIO_Pin = MCU_PWR_CTL; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //端口配置为上拉输入
	GPIO_Init(MCU_PWR_BANK, &GPIO_InitStructure);	//初始化端口
	
	GPIO_InitStructure.GPIO_Pin = MAIN_PWR_CTL; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //端口配置为上拉输入
	GPIO_Init(MAIN_PWR_BANK, &GPIO_InitStructure);	//初始化端口	

	GPIO_InitStructure.GPIO_Pin = IRDA; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //端口配置为上拉输入
	GPIO_Init(IRDA_BANK, &GPIO_InitStructure);	//初始化端口

}
