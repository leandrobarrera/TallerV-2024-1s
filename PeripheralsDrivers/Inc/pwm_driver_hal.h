/*
 * pwm_driver_hal.h
 *
 *  Created on: 10/09/2024
 *      Author: lbarreras
 *
 * Modificado en 2024 para reflejar los cambios en los nombres de
 * las funciones y distribuir mejor las tareas.
 */

#ifndef PWM_DRIVER_HAL_H_
#define PWM_DRIVER_HAL_H_

#include "stm32f4xx.h"

enum{
	PWM_CHANNEL_1 = 0,
	PWM_CHANNEL_2,
	PWM_CHANNEL_3,
	PWM_CHANNEL_4,
};

enum{
	PWM_DUTTY_0_PERCENT = 0,
	PWM_DUTT_100_PERCENT
};

enum{
	PWM_ACTIVE_HIGH = 0,
	PWM_ACTIVE_LOW
};

/**/
typedef struct
{
	uint8_t 	channel;		// Canal PWM relacionado con el TIMER
	uint32_t 	prescaler;		// A que velocidad se incrementa en el Timer
	uint16_t 	periodo;		// Indica el numero de veces que el Timer se incrementa, el periodo de la frecuencia viene dado
	uint16_t 	duttyCicle;		// Valor en porcentaje (%) del tiempo que la señal esta en alto
	uint8_t 	polarity;		// Polaridad de la señal de salida.
}PWM_Config_t;

/**/
typedef struct
{
	TIM_TypeDef		*ptrTIMx;		// Timer al que esta asociado el PWM
	PWM_Config_t	config;		//	Configuracion incial del PWM
}PWM_Handler_t;

/* Prototipo para funciones */
void pwm_Config(PWM_Handler_t *ptrPwmHandler);
void pwm_Set_Frequency(PWM_Handler_t *ptrPwmHandler);
void pwm_Update_Frequency(PWM_Handler_t *ptrPwmHandler, uint16_t newFreq);
void pwm_Set_DuttyCycle(PWM_Handler_t *ptrPwmHandler);
void pwm_Update_DuttyCycle(PWM_Handler_t *ptrPwmHandler, uint16_t newDuty);
void pwm_Enable_Output(PWM_Handler_t *ptrPwmHandler);
void pwm_Disable_Output(PWM_Handler_t *ptrPwmHandler);
void pwm_Start_Signal(PWM_Handler_t *ptrPwmHandler);
void pwm_Stop_Signal(PWM_Handler_t *ptrPwmHandler);



#endif /* PWM_DRIVER_HAL_H_ */
