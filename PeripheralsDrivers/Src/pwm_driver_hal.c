/*
 * pwm_driver_hal.c
 *
 *  Created on: 10/09/2024
 *      Author: lbarreras
 *
 * Modificado en 2024 para reflejar los cambios en los nombres de
 * las funciones y distribuir mejor las tareas.
 */

#include "pwm_driver_hal.h"

/**/
void pwm_Config(PWM_Handler_t *ptrPwmHandler){

	/* 1. Activar la señal de reloj del periférico requerido */
	if(ptrPwmHandler->ptrTIMx == TIM2){
		 RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	}
	else if(ptrPwmHandler->ptrTIMx == TIM3){
		 RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	}
	 else if(ptrPwmHandler->ptrTIMx == TIM4){
		 RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	 }
	 else if(ptrPwmHandler->ptrTIMx == TIM5){
		 RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
	 }
	 else if(ptrPwmHandler->ptrTIMx == TIM9){
		 RCC->APB2ENR |= RCC_APB2ENR_TIM9EN;
	 }
	 else if(ptrPwmHandler->ptrTIMx == TIM10){
		 RCC->APB2ENR |= RCC_APB2ENR_TIM10EN;
	 }
	 else if(ptrPwmHandler->ptrTIMx == TIM11){
		 RCC->APB2ENR |= RCC_APB2ENR_TIM11EN;
	 }
	else{
		__NOP();
	}

	/* 1. Cargamos la frecuencia deseada */
	pwm_Set_Frequency(ptrPwmHandler);

	/* 2. Cargamos el valor del dutty-Cycle*/
	pwm_Set_DuttyCycle(ptrPwmHandler);

	/* 2a. Estamos en UP_Mode, el limite se carga en ARR y se comienza en 0 */
	ptrPwmHandler->ptrTIMx->CR1 &= ~TIM_CR1_DIR;

	/* 3. Configuramos los bits CCxS del registro TIMy_CCMR1, de forma que sea modo salida
	 * (para cada canal hay un conjunto CCxS)
	 *
	 * 4. Además, en el mismo "case" podemos configurar el modo del PWM, su polaridad...
	 *
	 * 5. Y además activamos el preload bit, para que cada vez que exista un update-event
	 * el valor cargado en el CCRx será recargado en el registro "shadow" del PWM */
	switch(ptrPwmHandler->config.channel){
	case PWM_CHANNEL_1:{
		// Seleccionamos como salida el canal
		ptrPwmHandler->ptrTIMx->CCMR1 |= TIM_CCMR1_CC1S_Pos;

		// Configuramos el canal como PWM
		ptrPwmHandler->ptrTIMx->CCMR1 &= ~TIM_CCMR1_OC1M;
		ptrPwmHandler->ptrTIMx->CCMR1 |= (0b110 << TIM_CCMR1_OC1M_Pos);

		// Activamos la funcionalidad de pre-load
		ptrPwmHandler->ptrTIMx->CCMR1 |= TIM_CCMR1_OC1PE;
		break;
	}

	case PWM_CHANNEL_2:{
		// Seleccionamos como salida el canal
		ptrPwmHandler->ptrTIMx->CCMR1 &= ~TIM_CCMR1_CC2S;

		// Configuramos el canal como PWM
		ptrPwmHandler->ptrTIMx->CCMR1 &= ~TIM_CCMR1_OC2M;
		ptrPwmHandler->ptrTIMx->CCMR1 |= (0b110 << TIM_CCMR1_OC2M_Pos);

		// Activamos la funcionalidad de pre-load
		ptrPwmHandler->ptrTIMx->CCMR1 |= TIM_CCMR1_OC2PE;
		break;
	}

	case PWM_CHANNEL_3:{
		// Seleccionamos como salida el canal
		ptrPwmHandler->ptrTIMx->CCMR2 &= ~TIM_CCMR2_CC3S;

		// Configuramos el canal como PWM
		ptrPwmHandler->ptrTIMx->CCMR2 &= ~TIM_CCMR2_OC3M;
		ptrPwmHandler->ptrTIMx->CCMR2 |= (0b110 << TIM_CCMR2_OC3M_Pos);

		// Activamos la funcionalidad de pre-load
		ptrPwmHandler->ptrTIMx->CCMR2 |= TIM_CCMR2_OC3PE;
		break;
	}

	case PWM_CHANNEL_4:{
		// Seleccionamos como salida el canal
		ptrPwmHandler->ptrTIMx->CCMR2 &= ~TIM_CCMR2_CC4S;

		// Configuramos el canal como PWM
		ptrPwmHandler->ptrTIMx->CCMR2 &= ~TIM_CCMR2_OC4M;
		ptrPwmHandler->ptrTIMx->CCMR2 |= (0b110 << TIM_CCMR2_OC4M_Pos);

		// Activamos la funcionalidad de pre-load
		ptrPwmHandler->ptrTIMx->CCMR2 |= TIM_CCMR2_OC4PE;
		break;
	}

	default:{
		break;
	}



	}// fin del switch-case

	/* 6. Activamos la salida seleccionada */
	pwm_Enable_Output(ptrPwmHandler);
}

/* Función para activar el Timer y activar  el módulo PWM */
void pwm_Start_Signal(PWM_Handler_t *ptrPwmHandler) {
	ptrPwmHandler->ptrTIMx->CNT = 0;
	ptrPwmHandler->ptrTIMx->CR1 |= TIM_CR1_CEN;
	pwm_Enable_Output(ptrPwmHandler);
}

/* Función para desactivar el Timer y detener  el módulo PWM*/
void pwm_Stop_Signal(PWM_Handler_t *ptrPwmHandler) {
	ptrPwmHandler->ptrTIMx->CR1 &= ~TIM_CR1_CEN;
}

/* Función encargada de activar cada uno de los canales con los que cuenta el TimerX */
void pwm_Enable_Output(PWM_Handler_t *ptrPwmHandler) {
	switch (ptrPwmHandler->config.channel) {
	case PWM_CHANNEL_1: {
		// Activamos la salida del canal 1
		ptrPwmHandler->ptrTIMx->CCER |= TIM_CCER_CC1E;
		break;
	}
	case PWM_CHANNEL_2: {
		// Activamos la salida del canal 2
		ptrPwmHandler->ptrTIMx->CCER |= TIM_CCER_CC2E;
		break;
	}

	case PWM_CHANNEL_3: {
		// Activamos la salida del canal 3
		ptrPwmHandler->ptrTIMx->CCER |= TIM_CCER_CC3E;
		break;
	}

	case PWM_CHANNEL_4: {
		// Activamos la salida del canal 4
		ptrPwmHandler->ptrTIMx->CCER |= TIM_CCER_CC4E;
		break;
	}


	default: {
		break;
	}
	}
}


/* Función encargada de activar cada uno de los canales con los que cuenta el TimerX */
void pwm_Disable_Output(PWM_Handler_t *ptrPwmHandler) {
	switch (ptrPwmHandler->config.channel) {
	case PWM_CHANNEL_1: {
		// Desactivamos la salida del canal 1
		ptrPwmHandler->ptrTIMx->CCER &= ~TIM_CCER_CC1E;
		break;
	}
	case PWM_CHANNEL_2: {
		// Desactivamos la salida del canal 2
		ptrPwmHandler->ptrTIMx->CCER &= ~TIM_CCER_CC2E;
		break;
	}

	case PWM_CHANNEL_3: {
		// Desactivamos la salida del canal 3
		ptrPwmHandler->ptrTIMx->CCER &= ~TIM_CCER_CC3E;
		break;
	}

	case PWM_CHANNEL_4: {
		// Desactivamos la salida del canal 4
		ptrPwmHandler->ptrTIMx->CCER &= ~TIM_CCER_CC4E;
		break;
	}



	default: {
		break;
	}
	}
}
/*
 * La frecuencia es definida por el conjunto formado por el preescaler (PSC)
 * y el valor limite al que llega el Timer (ARR), con estos dos se establece
 * la frecuencia.
 * */
void pwm_Set_Frequency(PWM_Handler_t *ptrPwmHandler){

	// Cargamos el valor del prescaler, nos define la velocidad (en ns) a la cual
	// se incrementa el Timer
	ptrPwmHandler->ptrTIMx->PSC = ptrPwmHandler->config.prescaler;

	// Cargamos el valor del ARR, el cual es el límite de incrementos del Timer
	// antes de hacer un update y reload.
	ptrPwmHandler->ptrTIMx->ARR = ptrPwmHandler->config.periodo;
}


/* Función para actualizar la frecuencia, funciona de la mano con setFrequency */
void pwm_Update_Frequency(PWM_Handler_t *ptrPwmHandler, uint16_t newP){
	// Actualizamos el registro que manipula el periodo
	ptrPwmHandler->config.periodo = newP;

	// Llamamos a la fucnión que cambia la frecuencia
	pwm_Set_Frequency(ptrPwmHandler);
}

/* El valor del dutty debe estar dado en valores de %, entre 0% y 100%*/
void pwm_Set_DuttyCycle(PWM_Handler_t *ptrPwmHandler){

	// Seleccionamos el canal para configurar su dutty
	switch(ptrPwmHandler->config.channel){
	case PWM_CHANNEL_1:{
		ptrPwmHandler->ptrTIMx->CCR1 = ptrPwmHandler->config.duttyCicle-1;
		break;
	}
	case PWM_CHANNEL_2:{
		ptrPwmHandler->ptrTIMx->CCR2 = ptrPwmHandler->config.duttyCicle-1;
		break;
	}

	case PWM_CHANNEL_3:{
		ptrPwmHandler->ptrTIMx->CCR3 = ptrPwmHandler->config.duttyCicle-1;
		break;
	}

	case PWM_CHANNEL_4:{
		ptrPwmHandler->ptrTIMx->CCR4 = ptrPwmHandler->config.duttyCicle-1;
		break;
	}

	default:{
		break;
	}

	}// fin del switch-case

}


/* Función para actualizar el Duty, funciona de la mano con setDutyCycle */
void pwm_Update_DuttyCycle(PWM_Handler_t *ptrPwmHandler, uint16_t newDuty){
	// Actualizamos el registro que manipula el dutty
	ptrPwmHandler->config.duttyCicle = newDuty;

	// Llamamos a la fucnión que cambia el dutty y cargamos el nuevo valor
	pwm_Set_DuttyCycle(ptrPwmHandler);
}
