/*
 * adc_driver_hal.c
 *
 *  Created on: 3/09/2024
 *      Author: leandro
 */

#include "adc_driver_hal.h"
#include "gpio_driver_hal.h"
#include "stm32f4xx.h"
#include "stm32_assert.h"

/* === Headers for private functions === */
static void adc_enable_clock_peripheral (void);
static void adc_set_resolution (ADC_Config_t *adcConfig);
static void adc_set_alignment(ADC_Config_t *adcConfig);
static void adc_set_sampling_and_hold(ADC_Config_t *adcConfig);
static void adc_one_channel_sequence (ADC_Config_t *adcConfig);
static void adc_config_interrupt (ADC_Config_t *adcConfig);

/* Variables y elementos que necesita internamente el driver para funcionar adecuadamente */
GPIO_Handler_t	handlerADCPin = {0};
uint16_t		adcRawData = 0;

/*
 *
 * */
void adc_ConfigSingleChannel (ADC_Config_t *adcConfig){

	/* 1. Configuramos el PinX para que cumpla la funcion del canal analogo deseado*/
	adc_ConfigAnalogPin(adcConfig->channel);

	/* 2. Activamos la senal de reloj para el ADC */
	adc_enable_clock_peripheral();

	//Limpiamos los registros antes de empezar a configurar
	ADC1->CR1 = 0;
	ADC1->CR2 = 0;

	/* Comenzamos la configuracion de ADC1 */

	/* 3. Resolucion del ADC  */
	adc_set_resolution(adcConfig);

	/* 4. Configuramos el modo Scan como desactivado */
	adc_ScanMode(SCAN_OFF);


	/* 5. Configuramos la alineacion de los datos (derecha o izquierda) */
	adc_set_alignment(adcConfig);

	/* 6. Desactivamos el "continuous mode */
	adc_StopContinuousConv ();

	/* 7. Aca se deberia configurar el sampling... */
	adc_set_sampling_and_hold(adcConfig);

	/* 8. Configuramos la secuencia y cuandos elementos hay en la secuencia */
	adc_one_channel_sequence(adcConfig);

	/* 9. Configurar el prescaler del ADC en 2:1 (el mas rapido que se puede tener) */
	ADC ->CCR &= ~ADC_CCR_ADCPRE;

	/* 10. Desactivamos las interrupciones globales */
	__disable_irq();

	/* 11. Configuramos la interrupcion (si se encuentra activa), ademas de inscribir//remover
	 * la interrupcion enel NVIC */
	adc_config_interrupt(adcConfig);

	/* 12. Activamos el moduloADC */
	adc_peripheralOnOFF(ADC_ON);

	/* 13. Activamos las interrupciones globales */
	__enable_irq();
}

/*
 * Enable Cloc signal for ADC peripheral
 *  */
static void adc_enable_clock_peripheral(void){
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
}


	/*
	 * Configures the resolution for the conversion
	 */
static void adc_set_resolution(ADC_Config_t *adcConfig){
	// se limpia el registro del res.
	ADC1->CR1 &= ~(0b11<< 24);
	switch(adcConfig->resolution){
	case 0:{
		ADC1->CR1 &= ~(0b11<< 24);
		break;

	}
	case 1:{
		ADC1->CR1 |= (0b01<< 24);
		break;
	}
	case 2:{
		ADC1->CR1 |= (0b10<< 24);
		break;
	}
	case 3:{
		ADC1->CR1 |= (0b11<< 24);
		break;
	}
	}
}



	/*
	 * Set the number of alignment: left or right
	 */
static void adc_set_alignment(ADC_Config_t *adcConfig){
	// se limpia el registro del alignment.
		ADC1->CR2 &= ~(0b1<< 11);
		if(adcConfig->dataAlignment == ALIGNMENT_RIGHT){
		ADC1->CR2 &= ~(0b1<< 11);

		}
		else{
			ADC1->CR2 |= (0b1<< 11);
		}

		/*
		 * Relacionando con el valor del tiempo de carga del capacitor HOLD
		 */
}



static void adc_set_sampling_and_hold(ADC_Config_t *adcConfig){
	switch(adcConfig->channel){
	case 0:{
		switch(adcConfig->samplingPeriod){
		case 0:{
			ADC1->SMPR2 &= ~(0b111<<0);
			break;
		}
		case 1:{
			ADC1->SMPR2 |= (0b1<<0);
			break;
		}
		case 2:{
			ADC1->SMPR2 |= (0b010<<0);
			break;
		}
		case 3:{
			ADC1->SMPR2 |= (0b011<<0);
			break;
		}
		case 4:{
			ADC1->SMPR2 |= (0b100<<0);
			break;
		}
		case 5:{
			ADC1->SMPR2 |= (0b101<<0);
			break;
		}
		case 6:{
			ADC1->SMPR2 |= (0b110<<0);
			break;
		}
		case 7:{
			ADC1->SMPR2 |= (0b111<<0);
			break;
		}
		} break;
	}
	case 1:{
		switch(adcConfig->samplingPeriod){
		case 0:{
			ADC1->SMPR2 &= ~(0b111<<3);
			break;
		}
		case 1:{
			ADC1->SMPR2 |= (0b1<<3);
			break;
		}
		case 2:{
			ADC1->SMPR2 |= (0b010<<3);
			break;
		}
		case 3:{
			ADC1->SMPR2 |= (0b011<<3);
			break;
		}
		case 4:{
			ADC1->SMPR2 |= (0b100<<3);
			break;
		}
		case 5:{
			ADC1->SMPR2 |= (0b101<<3);
			break;
		}
		case 6:{
			ADC1->SMPR2 |= (0b110<<3);
			break;
		}
		case 7:{
			ADC1->SMPR2 |= (0b111<<3);
			break;
		}
		} break;
	}
	case 2:{
		switch(adcConfig->samplingPeriod){
		case 0:{
			ADC1->SMPR2 &= ~(0b111<<6);
			break;
		}
		case 1:{
			ADC1->SMPR2 |= (0b1<<6);
			break;
		}
		case 2:{
			ADC1->SMPR2 |= (0b010<<6);
			break;
		}
		case 3:{
			ADC1->SMPR2 |= (0b011<<6);
			break;
		}
		case 4:{
			ADC1->SMPR2 |= (0b100<<6);
			break;
		}
		case 5:{
			ADC1->SMPR2 |= (0b101<<6);
			break;
		}
		case 6:{
			ADC1->SMPR2 |= (0b110<<6);
			break;
		}
		case 7:{
			ADC1->SMPR2 |= (0b111<<6);
			break;
		}
		} break;
	}
	case 3:{
		switch(adcConfig->samplingPeriod){
		case 0:{
			ADC1->SMPR2 &= ~(0b111<<9);
			break;
		}
		case 1:{
			ADC1->SMPR2 |= (0b1<<9);
			break;
		}
		case 2:{
			ADC1->SMPR2 |= (0b010<<9);
			break;
		}
		case 3:{
			ADC1->SMPR2 |= (0b011<<9);
			break;
		}
		case 4:{
			ADC1->SMPR2 |= (0b100<<9);
			break;
		}
		case 5:{
			ADC1->SMPR2 |= (0b101<<9);
			break;
		}
		case 6:{
			ADC1->SMPR2 |= (0b110<<9);
			break;
		}
		case 7:{
			ADC1->SMPR2 |= (0b111<<9);
			break;
		}
		} break;
	}
	case 4:{
		switch(adcConfig->samplingPeriod){
		case 0:{
			ADC1->SMPR2 &= ~(0b111<<12);
			break;
		}
		case 1:{
			ADC1->SMPR2 |= (0b1<<12);
			break;
		}
		case 2:{
			ADC1->SMPR2 |= (0b010<<12);
			break;
		}
		case 3:{
			ADC1->SMPR2 |= (0b011<<12);
			break;
		}
		case 4:{
			ADC1->SMPR2 |= (0b100<<12);
			break;
		}
		case 5:{
			ADC1->SMPR2 |= (0b101<<12);
			break;
		}
		case 6:{
			ADC1->SMPR2 |= (0b110<<12);
			break;
		}
		case 7:{
			ADC1->SMPR2 |= (0b111<<12);
			break;
		}
		} break;
	}
	case 5:{
		switch(adcConfig->samplingPeriod){
		case 0:{
			ADC1->SMPR2 &= ~(0b111<<15);
			break;
		}
		case 1:{
			ADC1->SMPR2 |= (0b1<<15);
			break;
		}
		case 2:{
			ADC1->SMPR2 |= (0b010<<15);
			break;
		}
		case 3:{
			ADC1->SMPR2 |= (0b011<<15);
			break;
		}
		case 4:{
			ADC1->SMPR2 |= (0b100<<15);
			break;
		}
		case 5:{
			ADC1->SMPR2 |= (0b101<<15);
			break;
		}
		case 6:{
			ADC1->SMPR2 |= (0b110<<15);
			break;
		}
		case 7:{
			ADC1->SMPR2 |= (0b111<<15);
			break;
		}
		} break;
	}
	case 6:{
		switch(adcConfig->samplingPeriod){
		case 0:{
			ADC1->SMPR2 &= ~(0b111<<18);
			break;
		}
		case 1:{
			ADC1->SMPR2 |= (0b1<<18);
			break;
		}
		case 2:{
			ADC1->SMPR2 |= (0b010<<18);
			break;
		}
		case 3:{
			ADC1->SMPR2 |= (0b011<<18);
			break;
		}
		case 4:{
			ADC1->SMPR2 |= (0b100<<18);
			break;
		}
		case 5:{
			ADC1->SMPR2 |= (0b101<<18);
			break;
		}
		case 6:{
			ADC1->SMPR2 |= (0b110<<18);
			break;
		}
		case 7:{
			ADC1->SMPR2 |= (0b111<<18);
			break;
		}
		} break;
	}
	case 7:{
		switch(adcConfig->samplingPeriod){
		case 0:{
			ADC1->SMPR2 &= ~(0b111<<21);
			break;
		}
		case 1:{
			ADC1->SMPR2 |= (0b1<<21);
			break;
		}
		case 2:{
			ADC1->SMPR2 |= (0b010<<21);
			break;
		}
		case 3:{
			ADC1->SMPR2 |= (0b011<<21);
			break;
		}
		case 4:{
			ADC1->SMPR2 |= (0b100<<21);
			break;
		}
		case 5:{
			ADC1->SMPR2 |= (0b101<<21);
			break;
		}
		case 6:{
			ADC1->SMPR2 |= (0b110<<21);
			break;
		}
		case 7:{
			ADC1->SMPR2 |= (0b111<<21);
			break;
		}
		} break;
	}
	case 8:{
		switch(adcConfig->samplingPeriod){
		case 0:{
			ADC1->SMPR2 &= ~(0b111<<24);
			break;
		}
		case 1:{
			ADC1->SMPR2 |= (0b1<<24);
			break;
		}
		case 2:{
			ADC1->SMPR2 |= (0b010<<24);
			break;
		}
		case 3:{
			ADC1->SMPR2 |= (0b011<<24);
			break;
		}
		case 4:{
			ADC1->SMPR2 |= (0b100<<24);
			break;
		}
		case 5:{
			ADC1->SMPR2 |= (0b101<<24);
			break;
		}
		case 6:{
			ADC1->SMPR2 |= (0b110<<24);
			break;
		}
		case 7:{
			ADC1->SMPR2 |= (0b111<<24);
			break;
		}
		} break;
	}
	case 9:{
		switch(adcConfig->samplingPeriod){
		case 0:{
			ADC1->SMPR2 &= ~(0b111<<27);
			break;
		}
		case 1:{
			ADC1->SMPR2 |= (0b1<<27);
			break;
		}
		case 2:{
			ADC1->SMPR2 |= (0b010<<27);
			break;
		}
		case 3:{
			ADC1->SMPR2 |= (0b011<<27);
			break;
		}
		case 4:{
			ADC1->SMPR2 |= (0b100<<27);
			break;
		}
		case 5:{
			ADC1->SMPR2 |= (0b101<<27);
			break;
		}
		case 6:{
			ADC1->SMPR2 |= (0b110<<27);
			break;
		}
		case 7:{
			ADC1->SMPR2 |= (0b111<<27);
			break;
		}
		} break;
	}
	case 10:{
		switch(adcConfig->samplingPeriod){
		case 0:{
			ADC1->SMPR1 &= ~(0b111<<0);
			break;
		}
		case 1:{
			ADC1->SMPR1 |= (0b1<<0);
			break;
		}
		case 2:{
			ADC1->SMPR1 |= (0b010<<0);
			break;
		}
		case 3:{
			ADC1->SMPR1 |= (0b011<<0);
			break;
		}
		case 4:{
			ADC1->SMPR1 |= (0b100<<0);
			break;
		}
		case 5:{
			ADC1->SMPR1 |= (0b101<<0);
			break;
		}
		case 6:{
			ADC1->SMPR1 |= (0b110<<0);
			break;
		}
		case 7:{
			ADC1->SMPR1 |= (0b111<<0);
			break;
		}
		} break;
	}
	case 11:{
		switch(adcConfig->samplingPeriod){
		case 0:{
			ADC1->SMPR1 &= ~(0b111<<3);
			break;
		}
		case 1:{
			ADC1->SMPR1 |= (0b1<<3);
			break;
		}
		case 2:{
			ADC1->SMPR1 |= (0b010<<3);
			break;
		}
		case 3:{
			ADC1->SMPR1 |= (0b011<<3);
			break;
		}
		case 4:{
			ADC1->SMPR1 |= (0b100<<3);
			break;
		}
		case 5:{
			ADC1->SMPR1 |= (0b101<<3);
			break;
		}
		case 6:{
			ADC1->SMPR1 |= (0b110<<3);
			break;
		}
		case 7:{
			ADC1->SMPR1 |= (0b111<<3);
			break;
		}
		} break;
	}
	case 12:{
		switch(adcConfig->samplingPeriod){
		case 0:{
			ADC1->SMPR1 &= ~(0b111<<6);
			break;
		}
		case 1:{
			ADC1->SMPR1 |= (0b1<<6);
			break;
		}
		case 2:{
			ADC1->SMPR1 |= (0b010<<6);
			break;
		}
		case 3:{
			ADC1->SMPR1 |= (0b011<<6);
			break;
		}
		case 4:{
			ADC1->SMPR1 |= (0b100<<6);
			break;
		}
		case 5:{
			ADC1->SMPR1 |= (0b101<<6);
			break;
		}
		case 6:{
			ADC1->SMPR1 |= (0b110<<6);
			break;
		}
		case 7:{
			ADC1->SMPR1 |= (0b111<<6);
			break;
		}
		} break;
	}
	case 13:{
		switch(adcConfig->samplingPeriod){
		case 0:{
			ADC1->SMPR1 &= ~(0b111<<9);
			break;
		}
		case 1:{
			ADC1->SMPR1 |= (0b1<<9);
			break;
		}
		case 2:{
			ADC1->SMPR1 |= (0b010<<9);
			break;
		}
		case 3:{
			ADC1->SMPR1 |= (0b011<<9);
			break;
		}
		case 4:{
			ADC1->SMPR1 |= (0b100<<9);
			break;
		}
		case 5:{
			ADC1->SMPR1 |= (0b101<<9);
			break;
		}
		case 6:{
			ADC1->SMPR1 |= (0b110<<9);
			break;
		}
		case 7:{
			ADC1->SMPR1 |= (0b111<<9);
			break;
		}
		} break;
	}
	case 14:{
		switch(adcConfig->samplingPeriod){
		case 0:{
			ADC1->SMPR1 &= ~(0b111<<12);
			break;
		}
		case 1:{
			ADC1->SMPR1 |= (0b1<<12);
			break;
		}
		case 2:{
			ADC1->SMPR1 |= (0b010<<12);
			break;
		}
		case 3:{
			ADC1->SMPR1 |= (0b011<<12);
			break;
		}
		case 4:{
			ADC1->SMPR1 |= (0b100<<12);
			break;
		}
		case 5:{
			ADC1->SMPR1 |= (0b101<<12);
			break;
		}
		case 6:{
			ADC1->SMPR1 |= (0b110<<12);
			break;
		}
		case 7:{
			ADC1->SMPR1 |= (0b111<<12);
			break;
		}
		} break;
	}
	case 15:{
		switch(adcConfig->samplingPeriod){
		case 0:{
			ADC1->SMPR1 &= ~(0b111<<15);
			break;
		}
		case 1:{
			ADC1->SMPR1 |= (0b1<<15);
			break;
		}
		case 2:{
			ADC1->SMPR1 |= (0b010<<15);
			break;
		}
		case 3:{
			ADC1->SMPR1 |= (0b011<<15);
			break;
		}
		case 4:{
			ADC1->SMPR1 |= (0b100<<15);
			break;
		}
		case 5:{
			ADC1->SMPR1 |= (0b101<<15);
			break;
		}
		case 6:{
			ADC1->SMPR1 |= (0b110<<15);
			break;
		}
		case 7:{
			ADC1->SMPR1 |= (0b111<<15);
			break;
		}
		} break;
	}
	}




	/*	Configura el numero de elementos en la secuencia (solo un elemento)
	 * 	Configura cual es el canal adquiere la señal ADC
	 */
}
static void adc_one_channel_sequence(ADC_Config_t *adcConfig){
	ADC1->SQR1 &= ~(0b1111 << 20);
	ADC1->SQR3 &= ~(0b11111 << 0);
	switch(adcConfig->channel){
	case 0:{
		ADC1->SQR3 &= ~(0b11111);
		break;
	}
	case 1:{
		ADC1->SQR3 |= (0b1);
		break;
	}
	case 2:{
		ADC1->SQR3 |= (2);
		break;
	}
	case 3:{
		ADC1->SQR3 |= (3);
		break;
	}
	case 4:{
		ADC1->SQR3 |= (4);
		break;
	}
	case 5:{
		ADC1->SQR3 |= (5);
		break;
	}
	case 6:{
		ADC1->SQR3 |= (6);
		break;
	}
	case 7:{
		ADC1->SQR3 |= (7);
		break;
	}
	case 8:{
		ADC1->SQR3 |= (8);
		break;
	}
	case 9:{
		ADC1->SQR3 |= (9);
		break;
	}
	case 10:{
		ADC1->SQR3 |= (10);
		break;
	}
	case 11:{
		ADC1->SQR3 |= (11);
		break;
	}
	case 12:{
		ADC1->SQR3 |= (12);
		break;
	}
	case 13:{
		ADC1->SQR3 |= (13);
		break;
	}
	case 14:{
		ADC1->SQR3 |= (14);
		break;
	}
	case 15:{
		ADC1->SQR3 |= (15);
		break;
	}


	}



		/*
		 * Configura el enable de la interrupcion y la activacion del NVIC
		  */
}


static void	adc_config_interrupt(ADC_Config_t *adcConfig){
	if(adcConfig->interruptState == ADC_INT_ENABLE){
		ADC1->CR1 |= (1 << 5);
		__NVIC_EnableIRQ(ADC_IRQn);
		__NVIC_SetPriority(ADC_IRQn, 4);
	}
	else{
		ADC1->CR1 &= ~(1 << 5);
		__NVIC_DisableIRQ(ADC_IRQn);

	}


		/*
		 * Controla la activacion y desactivacion del modulo ADC desde el registro
		 * Cr2 del adc.
		 * */


	}
void adc_peripheralOnOFF(uint8_t state){
	if(state == ADC_ON){
		ADC1->CR2 |= (1);
	}
	else{
		ADC1->CR2 &= ~(1);
	}


	/*	Enables and disables the Scan mode...
	 * 	Funciona de la mano con la secuencia de varios canales.
	 * 	No es necesario para el caso de solo un canal simple.
	 */
}
void adc_ScanMode(uint8_t state){





	if(state == SCAN_ON){
			ADC1->CR1 |= (1<<8);
		}
		else{
			ADC1->CR1 &= ~(1<<8);
		}


		/*
		 * Funcion que comienza la conversion ADC simple
		 */
}
void adc_StartSingleConv(void){
	ADC1->CR2 |= (1<<30);

		/*
		 * Funcion que comienza la conversion ADC continua
		 */
}

void adc_StartContinuousConv(void){
	ADC1->CR2 |= (1<<30);
	ADC1->CR2 |= (1<<1);
}
void adc_StopContinuousConv(void){
	ADC1->CR2 &= ~(1<<1);
}

/*
 * Funcion que me retorna el ultimo dato adquirido por la ADC
 */
uint16_t adc_GetValue(void){
	return adcRawData;
}


/*
 *  Esta la ISR de la interrupcion por conversion ADC
 */

void ADC_IRQHandler(void){
	if(ADC1->SR & (1<<1)){
	adcRawData = ADC1->DR;
	adc_CompleteCallback ();
	}

}
__attribute__((weak)) void adc_CompleteCallback(void){
		__NOP();
	}
void adc_ConfigAnalogPin(uint8_t adcChannel){
switch (adcChannel){
	case 0: {
		handlerADCPin.pGPIOx = GPIOA;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_0;
		gpio_Config(&handlerADCPin);
		break;
	}
	case 1: {
		handlerADCPin.pGPIOx = GPIOA;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_1;
		gpio_Config(&handlerADCPin);
		break;
	}
	case 2: {
		handlerADCPin.pGPIOx = GPIOA;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_2;
		gpio_Config(&handlerADCPin);
		break;
	}
	case 3: {
		handlerADCPin.pGPIOx = GPIOA;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_3;
		gpio_Config(&handlerADCPin);
		break;
	}
	case 4: {
		handlerADCPin.pGPIOx = GPIOA;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_4;
		gpio_Config(&handlerADCPin);
		break;
	}
	case 5: {
		handlerADCPin.pGPIOx = GPIOA;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_5;
		gpio_Config(&handlerADCPin);
		break;
	}
	case 6: {
		handlerADCPin.pGPIOx = GPIOA;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_6;
		gpio_Config(&handlerADCPin);
		break;
	}
	case 7: {
		handlerADCPin.pGPIOx = GPIOA;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_7;
		gpio_Config(&handlerADCPin);
		break;
	}
	case 8: {
		handlerADCPin.pGPIOx = GPIOB;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_0;
		gpio_Config(&handlerADCPin);
		break;
	}
	case 9: {
		handlerADCPin.pGPIOx = GPIOB;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_1;
		gpio_Config(&handlerADCPin);
		break;
	}
	case 10: {
		handlerADCPin.pGPIOx = GPIOC;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_0;
		gpio_Config(&handlerADCPin);
		break;
	}
	case 11: {
		handlerADCPin.pGPIOx = GPIOC;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_1;
		gpio_Config(&handlerADCPin);
		break;
	}
	case 12: {
		handlerADCPin.pGPIOx = GPIOC;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_2;
		gpio_Config(&handlerADCPin);
		break;
	}
	case 13: {
		handlerADCPin.pGPIOx = GPIOC;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_3;
		gpio_Config(&handlerADCPin);
		break;
	}
	case 14: {
		handlerADCPin.pGPIOx = GPIOC;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_4;
		gpio_Config(&handlerADCPin);
		break;
	}
	case 15: {
		handlerADCPin.pGPIOx = GPIOC;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_5;
		gpio_Config(&handlerADCPin);
		break;
	}

}
}

/* Configuracion para hacer conversiones en multpiles canales y en orden especifico */

/* Configuracion para trigger externo */


