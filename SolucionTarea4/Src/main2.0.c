/*
 * main2.0.c
 *
 *  Created on: 18/09/2024
 *      Author: lbarreras
 */

#include  <stdio.h>
#include <stdint.h>
#include  <stdbool.h>
#include <string.h>
#include <stm32f4xx.h>
#include "stm32f4xx.h"
#include "stm32_assert.h"
#include "gpio_driver_hal.h"
#include "timer_driver_hal.h"
#include "adc_driver_hal.h"
#include "usart_driver_hal.h"
#include "exti_driver_hal.h"
#include "pwm_driver_hal.h"

//Handlers GPIO, para los pines. Lo de toda la vida.
GPIO_Handler_t userLed = {0}; 		//	PinA5
GPIO_Handler_t LedRed = {0}; 		//	PinC8
GPIO_Handler_t LedGreen = {0}; 		//	PinB8
GPIO_Handler_t LedBlue = {0}; 		//	PinB8
GPIO_Handler_t pinTX = {0}; 		//	PinTX
GPIO_Handler_t pinRX = {0}; 		//	PinRX

// buffers para USART
char bufferMsg[128] = {0};

//Handlers para los tres timers que se pidieron
PWM_Handler_t red_pwm = {0};
PWM_Handler_t blue_pwm = {0};
PWM_Handler_t green_pwm = {0};
Timer_Handler_t blinkyTimer = {0};

//Handlers para el usart
USART_Handler_t usart2 = {0};


//variable
uint8_t dutty_Red = 0;
uint8_t dutty_Green = 0;
uint8_t dutty_Blue = 0;

void initSystem (void);


int main(void){

	initSystem();



	while(1){

	}
}










void initSystem (void){

	/*	LedRed*/
	LedRed.pGPIOx 							= 	GPIOA;
	LedRed.pinConfig.GPIO_PinNumber			=	PIN_0;
	LedRed.pinConfig.GPIO_PinMode			=	GPIO_MODE_ALTFN;
	LedRed.pinConfig.GPIO_PinOutputType		=	GPIO_OTYPE_PUSHPULL;
	LedRed.pinConfig.GPIO_PinOutputSpeed	=	GPIO_OSPEED_MEDIUM;
	LedRed.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;
	LedRed.pinConfig.GPIO_PinAltFunMode 	=	AF1;


	gpio_Config(&LedRed);

	/*	LedGreen	*/
	LedGreen.pGPIOx 						= 	GPIOA;
	LedGreen.pinConfig.GPIO_PinNumber		=	PIN_1;
	LedGreen.pinConfig.GPIO_PinMode			=	GPIO_MODE_ALTFN;
	LedGreen.pinConfig.GPIO_PinOutputType	=	GPIO_OTYPE_PUSHPULL;
	LedGreen.pinConfig.GPIO_PinOutputSpeed	=	GPIO_OSPEED_MEDIUM;
	LedGreen.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;
	LedGreen.pinConfig.GPIO_PinAltFunMode 		=	AF1;


	gpio_Config(&LedGreen);

	/*	LedBlue	*/
	LedBlue.pGPIOx 							= 	GPIOB;
	LedBlue.pinConfig.GPIO_PinNumber		=	PIN_10;
	LedBlue.pinConfig.GPIO_PinMode			=	GPIO_MODE_ALTFN;
	LedBlue.pinConfig.GPIO_PinOutputType	=	GPIO_OTYPE_PUSHPULL;
	LedBlue.pinConfig.GPIO_PinOutputSpeed	=	GPIO_OSPEED_MEDIUM;
	LedBlue.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;
	LedBlue.pinConfig.GPIO_PinAltFunMode 		=	AF1;


	gpio_Config(&LedBlue);

	pinRX.pGPIOx = GPIOA;
	pinRX.pinConfig.GPIO_PinNumber		 = PIN_3;
	pinRX.pinConfig.GPIO_PinMode		=	GPIO_MODE_ALTFN;
	pinRX.pinConfig.GPIO_PinOutputType	=	GPIO_OTYPE_PUSHPULL;
	pinRX.pinConfig.GPIO_PinOutputSpeed	=	GPIO_OSPEED_MEDIUM;
	pinRX.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;
	pinRX.pinConfig.GPIO_PinAltFunMode 	=	AF7;

	gpio_Config(&pinRX);

	pinTX.pGPIOx = GPIOA;
	pinTX.pinConfig.GPIO_PinNumber		 = PIN_2;
	pinTX.pinConfig.GPIO_PinMode		=	GPIO_MODE_ALTFN;
	pinTX.pinConfig.GPIO_PinOutputType	=	GPIO_OTYPE_PUSHPULL;
	pinTX.pinConfig.GPIO_PinOutputSpeed	=	GPIO_OSPEED_MEDIUM;
	pinTX.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;
	pinTX.pinConfig.GPIO_PinAltFunMode 	=	AF7;

	gpio_Config(&pinTX);

	/*	userLed	*/
	userLed.pGPIOx 							= 	GPIOA;
	userLed.pinConfig.GPIO_PinNumber		=	PIN_5;
	userLed.pinConfig.GPIO_PinMode			=	GPIO_MODE_OUT;
	userLed.pinConfig.GPIO_PinOutputType	=	GPIO_OTYPE_PUSHPULL;
	userLed.pinConfig.GPIO_PinOutputSpeed	=	GPIO_OSPEED_MEDIUM;
	userLed.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;

	gpio_Config(&userLed);


	blinkyTimer.pTIMx 							= TIM5;
	blinkyTimer.TIMx_Config.TIMx_Prescaler		=16000;  //	Genera incrementos de 1ms
	blinkyTimer.TIMx_Config.TIMx_Period			=250;  //	el prescaler lo ajusta 1ms, entonces lo quiero a 250ms, y es la multiplicacion de uno con el otro.
	blinkyTimer.TIMx_Config.TIMx_mode			=TIMER_UP_COUNTER;  //
	blinkyTimer.TIMx_Config.TIMx_InterruptEnable	=TIMER_INT_ENABLE;  //


	/*	Configuramos el Timer */
	timer_Config(&blinkyTimer);

	//	Encedemos el Timer.
	timer_SetState(&blinkyTimer,SET);

	dutty_Red = 125; //se pone un numero porque el dutty maneja un rango, y hay que respetarlo, de no ser así no recibimos señal alguna del pwm.
	red_pwm.ptrTIMx = TIM2;
	red_pwm.config.channel = PWM_CHANNEL_1;
	red_pwm.config.periodo = 250;
	red_pwm.config.prescaler = 160;
	red_pwm.config.duttyCicle = dutty_Red;

	pwm_Config(&red_pwm);
	pwm_Start_Signal(&red_pwm);

	dutty_Green = 30; //se pone un numero porque el dutty maneja un rango, y hay que respetarlo, de no ser así no recibimos señal alguna del pwm.
	green_pwm.ptrTIMx = TIM2;
	green_pwm.config.channel = PWM_CHANNEL_2;
	green_pwm.config.periodo = 250;
	green_pwm.config.prescaler = 160;
	green_pwm.config.duttyCicle = dutty_Green;

	pwm_Config(&green_pwm);
	pwm_Start_Signal(&green_pwm);

	dutty_Blue = 15; //se pone un numero porque el dutty maneja un rango, y hay que respetarlo, de no ser así no recibimos señal alguna del pwm.
	blue_pwm.ptrTIMx = TIM2;
	blue_pwm.config.channel = PWM_CHANNEL_3;
	blue_pwm.config.periodo = 250;
	blue_pwm.config.prescaler = 160;
	blue_pwm.config.duttyCicle = dutty_Blue;

	pwm_Config(&blue_pwm);
	pwm_Start_Signal(&blue_pwm);

	/* Configuramos el USART2
	 * usamos las configs que el profe nos dejo en el documento de la tarea. */

	usart2.ptrUSARTx = USART2;
	usart2.USART_Config.baudrate =	USART_BAUDRATE_115200;
	usart2.USART_Config.datasize = USART_DATASIZE_8BIT;
	usart2.USART_Config.parity = USART_PARITY_NONE;
	usart2.USART_Config.stopbits = USART_STOPBIT_1;
	usart2.USART_Config.enableIntRX = USART_RX_INTERRUP_ENABLE;
	usart2.USART_Config.mode = USART_MODE_RXTX;

	usart_Config(&usart2);




}

