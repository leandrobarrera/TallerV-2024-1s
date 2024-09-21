/*
 * gpio_driver_hal.c
 *
 *  Created on: 18/03/2024
 *      Author: Leandro Barrera Salas
 */

#include "gpio_driver_hal.h"
#include "stm32f4xx.h"
#include "stm32_assert.h"

/* === Headers for private functions === */
void gpio_enable_clock_peripheral (GPIO_Handler_t *pGPIOHandler);
void gpio_config_mode (GPIO_Handler_t *pGPIOHandler);
void gpio_config_output_type (GPIO_Handler_t *pGPIOHandler);
void gpio_config_output_speed (GPIO_Handler_t *pGPIOHandler);
void gpio_config_pullup_pulldown(GPIO_Handler_t *pGPIOHandler);
void gpio_config_alternate_function (GPIO_Handler_t *pGPIOHandler);


/**
 * Para cualquier periferico, hay varios pasos que siempre se deben seguir en un
 * orden estricto para poder que el sistema permita configurar el periferico X.
 * Lo primero y mas importante es activar la señal del reloj principal hacia ese
 * elemento especifico (relacionado con el periferico RCC), a esto llamaremos
 * simplemente "activar el periferico o activar la señal del reloj del periferico)
 */
void gpio_Config (GPIO_Handler_t *pGPIOHandler){

	/* Verificamos que el pin seleccionado es correcto */
	assert_param(IS_GPIO_PIN(pGPIOHandler->pinConfig.GPIO_PinNumber));

	// 1) Activar el periferico
	gpio_enable_clock_peripheral(pGPIOHandler);

	// Despues de activado, podemos empezar a configurar a configurar.

	// 2) Configurando el registro GPIOx_MODER
	gpio_config_mode(pGPIOHandler);

	// 3) Configurando elregistro GPIOx_OTYPER
	gpio_config_output_type(pGPIOHandler);

	// 4) Configurando ahora la velocidad
	gpio_config_output_speed(pGPIOHandler);

	// 5) Configurando si se desea pull-up, pull-down o flotante.
	gpio_config_pullup_pulldown(pGPIOHandler);

	// 6) Configuracion de las funciones alternativas... se vera luego, mas adelante en el curso
	gpio_config_alternate_function(pGPIOHandler);

} // Fin del GPIO_config

/*
 * Enable clock signal for specific GPIOx port
 */
void gpio_enable_clock_peripheral (GPIO_Handler_t *pGPIOHandler){

	// Verificamos que el puerto configurado si es permitido
	assert_param(IS_GPIO_ALL_INSTANCE(pGPIOHandler -> pGPIOx));

	// Verificamos para GPIOA
		if(pGPIOHandler->pGPIOx == GPIOA){
			// Escribimos 1 (SET) en la posicion correspondiente en GPIOA
			RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN);
		}
		// Verificamos para GPIOB
		else if(pGPIOHandler ->pGPIOx == GPIOB){
			// Escribimos 1 (SET) en la posicion correspondiente en GPIOB
			RCC ->AHB1ENR |= (RCC_AHB1ENR_GPIOBEN);
		}
		// Verificamos para GPIOC
		else if(pGPIOHandler ->pGPIOx == GPIOC){
			// Escribimos 1 (SET) en la posicion correspondiente en GPIOC
			RCC ->AHB1ENR |= (RCC_AHB1ENR_GPIOCEN);
		}
		// Verificamos para GPIOD
		else if(pGPIOHandler ->pGPIOx == GPIOD){
			// Escribimos 1 (SET) en la posicion correspondiente en GPIOD
			RCC ->AHB1ENR |= (RCC_AHB1ENR_GPIODEN);
		}

		// Verificamos para GPIOE
		else if(pGPIOHandler ->pGPIOx == GPIOE){
			// Escribimos 1 (SET) en la posicion correspondiente en GPIOE
			RCC ->AHB1ENR |= (RCC_AHB1ENR_GPIOEEN);
		}
		// Verificamos para GPIOH
		else if(pGPIOHandler ->pGPIOx == GPIOH){
			// Escribimos 1 (SET) en la posicion correspondiente en GPIOH
			RCC ->AHB1ENR |= (RCC_AHB1ENR_GPIOHEN);
		}
}
/*
 * Configures the mode in which the pin will work:
 * - Input
 * - Output
 * - Analog
 * - Alternate function
 */
void gpio_config_mode (GPIO_Handler_t *pGPIOHandler){

	uint32_t auxConfig = 0;

	/* Verificamos que si el modo que se ha seleccionado es permitido */
	assert_param(IS_GPIO_MODE(pGPIOHandler ->pinConfig.GPIO_PinMode));

	// Aca estamos leyendo la config, moviendo "PinNumber" veces a la izquierda ese valor (shift left)
	// y todo eso lo cargamos en la variable auxConfig
	auxConfig = (pGPIOHandler ->pinConfig.GPIO_PinMode << 2 * pGPIOHandler ->pinConfig.GPIO_PinNumber);

	// Antes de cargar el nuevo valor, limpiamos los bits especificos de ese registro (debemos escribir 0b00)
	// Para lo cual aplicamos una mascara y una operacion bitwise AND
	pGPIOHandler ->pGPIOx ->MODER &= ~(0b11 << 2 * pGPIOHandler ->pinConfig.GPIO_PinNumber);

	// Cargamos auxConfig en el registro MODER
	pGPIOHandler ->pGPIOx ->MODER |= auxConfig;
}

/*
 * Configurates which type of output the PinX will use:
 *  -Push-pull
 *  -openDrain
 */
void gpio_config_output_type (GPIO_Handler_t * pGPIOHandler){

	uint32_t auxConfig = 0;

	/* Verificamos que el tipo de salida corresponda a los que se pueden utilizar */
	assert_param(IS_GPIO_OUTPUT_TYPE(pGPIOHandler ->pinConfig.GPIO_PinOutputType));

	// De nuevo, leemos y movemos el valor de un numero "PinNumber" de veces ¿error?
	auxConfig = (pGPIOHandler ->pinConfig.GPIO_PinOutputType <<  pGPIOHandler ->pinConfig.GPIO_PinNumber);

	//limpiamos antes de cargar
	pGPIOHandler ->pGPIOx ->OTYPER &= ~(SET << pGPIOHandler ->pinConfig.GPIO_PinNumber);

	// cargamos el resultado sobre el registro adecuado
	pGPIOHandler ->pGPIOx ->OTYPER |= auxConfig;
}

/*
 * Selects between four differents possible speeds for output PinX.
 * - Low
 * - Medium
 * - Fast
 * - High Speed
 */
void gpio_config_output_speed (GPIO_Handler_t * pGPIOHandler){

	uint32_t auxConfig = 0;

	/**/
	assert_param(IS_GPIO_OSPEED(pGPIOHandler ->pinConfig.GPIO_PinOutputSpeed));

	auxConfig = (pGPIOHandler ->pinConfig.GPIO_PinOutputSpeed <<  2*pGPIOHandler ->pinConfig.GPIO_PinNumber);

	//limpiamos la posicion antes de cargar la nueva configuracion
	pGPIOHandler ->pGPIOx ->OSPEEDR &= ~(0b11 << 2 * pGPIOHandler ->pinConfig.GPIO_PinNumber);

	// cargamos el resultado sobre el registro adecuado
	pGPIOHandler ->pGPIOx ->OSPEEDR |= auxConfig;
}


/*
 * Turns ON/OFF the pull-up and pull-down resistor for each PinX in selected GPIO port
 */
void gpio_config_pullup_pulldown(GPIO_Handler_t *pGPIOHandler){

	uint32_t auxConfig = 0;

		/*verificamos si la configuracion cargada para las resistencias es correcta*/
		assert_param(IS_GPIO_PUPDR(pGPIOHandler ->pinConfig.GPIO_PinPuPdControl));

		auxConfig = (pGPIOHandler ->pinConfig.GPIO_PinPuPdControl <<  2*pGPIOHandler ->pinConfig.GPIO_PinNumber);

		//limpiamos la posicion antes de cargar la nueva configuracion
		pGPIOHandler ->pGPIOx->PUPDR &= ~(0b11 << 2 * pGPIOHandler ->pinConfig.GPIO_PinNumber);

		// cargamos el resultado sobre el registro adecuado
		pGPIOHandler ->pGPIOx->PUPDR |= auxConfig;
}

/*
 * Allows to configure other functions (more specialized) on the selected PinX.
 */
void gpio_config_alternate_function (GPIO_Handler_t *pGPIOHandler){

	uint32_t auxPosition = 0;

	if (pGPIOHandler ->pinConfig.GPIO_PinMode == GPIO_MODE_ALTFN){

		// Seleccionamos primero si se debe utilizar el registro bajo (AFRL) o el alto (AFRH)
		if(pGPIOHandler ->pinConfig.GPIO_PinNumber < 8 ){
			// Estamos en registro AFRL, que controla los pines del PIN_0 al PIN_7
			auxPosition = 4 * pGPIOHandler ->pinConfig.GPIO_PinNumber;

			// Limpiamos primero la posicion del registro que deseamos escribir a continuacion
			pGPIOHandler ->pGPIOx ->AFR[0] &= ~(0b1111 << auxPosition);

			// Y escribimos el valor configurado en la posicion seleccionada
			pGPIOHandler ->pGPIOx ->AFR[0] |= (pGPIOHandler ->pinConfig.GPIO_PinAltFunMode << auxPosition);
		}
		else {
			// Estamos en el registro AFRH, que controla los pines del PIN_8 al PIN_15
			auxPosition = 4 * (pGPIOHandler ->pinConfig.GPIO_PinNumber -8);

			// Limpiamos primero la posicion del registro que deseamos escribir a continuacion
			pGPIOHandler ->pGPIOx ->AFR[1] &= (0b1111 << auxPosition);

			// Y escribimos el valor configurado en la posicion seleccionada
			pGPIOHandler ->pGPIOx ->AFR[1] |= (pGPIOHandler ->pinConfig.GPIO_PinAltFunMode << auxPosition);

		}
	}
}

/**
 * Funcion utilizada para cambiar de estado el pin entregado en el handler, asignado
 * el valor entregadoe en la variable newState
 */
void gpio_WritePin(GPIO_Handler_t *pPinHandler, uint8_t newState){

	/*verificamos si la accion que deseamos realizar es permitida */
	assert_param(IS_GPIO_PIN_ACTION(newState));

	// limpiamos la posicion que deseamos
	// pPinHandler ->GPIOx -> ODR &= ~(SET << pPinHandler ->pinConfig.GPIO_PinNumber);
	if (newState == SET){
		// Trabajando con la parte baja del registro
		pPinHandler -> pGPIOx -> BSRR |= (SET << pPinHandler ->pinConfig.GPIO_PinNumber);
	}
	else {
		// Trabajando con la parte alta del registro
		pPinHandler -> pGPIOx -> BSRR |= (SET << (pPinHandler ->pinConfig.GPIO_PinNumber + 16));
	}
}

/**
 * Funcion para leer el estado de un pin especifico.
 */
uint32_t gpio_ReadPin (GPIO_Handler_t *pPinHandler){
		// Creamos una variable auxiliar la cual luego retomaremos
		uint32_t pinValue = 0;


		/* PinValue es una variable a la que se le asigna el IDR (Input Data Register, 32 bits, 16 reservados)
		 * y luego lo shiftea PinNumber veces, es decir, si escogemos el pin5, lo shiftea 5 veces.
		 *
		 */
		pinValue = (pPinHandler ->pGPIOx -> IDR >> pPinHandler->pinConfig.GPIO_PinNumber);



		/* Luego de moverlo, aplicamos una mascara porque nos importa el bit 0, que es el que nos devuelve un 1 o un 0
		 * lo que nos indicaria si el pin esta recibiendo informacion, para obtener el bit 0 se aplica
		 * la operacion AND (&) */


		pinValue &= 0b1 ;

		/* teniendo presente que la variable pinValue contiene el IDR y su lectura sabemos que
		estamos leyendo el valor del pinX, y nos retornara un 0 o un 1 logico.
		 si nos retorna un 1 quiere decir que si esta recibiendo informacion,
		de lo contrario no lo esta.. */

		return pinValue;
}

/**/
void gpio_TooglePin (GPIO_Handler_t *pPinHandler){

	/* se usan condicionales para cambiar  el valor del pin si la funcion GPIO_ReadPin retorna un 0 entonces se
	 *cambiara ese valor por un 1 con la funcion GPIO_WritePin y el valor SET, mientras que con el else pasara lo
	 *contrario
	 */

	if (gpio_ReadPin(pPinHandler) == 0){
			gpio_WritePin(pPinHandler, SET);

	}
	else{
		(gpio_WritePin(pPinHandler, RESET));
	}
	}
