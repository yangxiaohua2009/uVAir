#ifndef __PWM_OUTPUT_H
#define	__PWM_OUTPUT_H

#include "stm32f10x.h"


void TIM1_PWM_Init(void);
void TIM3_PWM_Init(void);
void pwm_led_out (void);
void TM1_PWM1_FAN_ENABLE(unsigned char onoff);
void TM1_PWM1_LIGHTNESS_ENABLE(unsigned char onoff);
void TM1_PWM1_CH3_ENABLE(unsigned char onoff);
void TM3_PWM3_BEEP_ENABLE(unsigned char onoff);
#endif /* __PWM_OUTPUT_H */

