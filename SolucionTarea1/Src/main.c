/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 Punto 1. ¿Cúal es este error o errores?
 -El error constaba en que el shift estaba invertido, y nos movía el dato pinNumber veces, cuando en realidad lo necesitamos
 en el bit 0, para que nos diera un 1 o 0 (dado que es el unico bit que lo permite). Además de que faltaba otra
 operación en el código para hacerlo leer el bit 0.
¿Cómo se pueden solucionar?
-Se cambió el sentido de los shitfs y ahora si arroja la informacion en el bit 0; luego agregamos una máscara (0b01) con un
AND (&), para extraer ese 1 o 0 lógico y verificar que el pin está recibiendo información o no.
 */


#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "stm32_assert.h"
#include "gpio_driver_hal.h"

// Headers definition
int add (int x, int y);

void retraso(void);
void contador(void);

/*Variables. Estas variables son las variables que almacenan el valor
 * si esta en off/on para Led0, Led1... en el bit0, bit1.... respectivamente
 * ademas se inicializan tambien variables para ayudar a recoger la informacion
 * del PinC13 (datoboton) y la variable conteo para la cuenta en binario del micro*/

uint8_t conteo = 0;
uint8_t Led0 = 0;
uint8_t Led1 = 0;
uint8_t Led2 = 0;
uint8_t Led3 = 0;
uint8_t Led4 = 0;
uint8_t Led5 = 0;
uint8_t datoboton = 0;



/* Definimos un pin de prueba. Los handler se entienden como algo literal
es un tipo de "variable" que usa a otra para su ejecucion. Las variables
llamadas handler contienen en su estructura */

GPIO_Handler_t userLed = {0}; //	PinA5
GPIO_Handler_t bit2 = {0}; //	PinC7
GPIO_Handler_t bit0 = {0}; //	PinA9
GPIO_Handler_t bit1 = {0}; //	PinCB6
GPIO_Handler_t bit3 = {0}; //	PinC6
GPIO_Handler_t bit4 = {0}; //	PinB9
GPIO_Handler_t bit5 = {0}; //	PinA6
GPIO_Handler_t DIR = {0}; //	PinA0
GPIO_Handler_t user = {0}; //	PinC13


/*
 * The main function, where everything happens.
 */
int main(void)
{
	/* Configuramos el pin. Aqui se configuran los pins uno por uno, para este driver
	 * los datos mas importantes a tener en cuenta es el puerto (A,B,C...), el numero del pin y el modo del pin.
	 * si le damos clic derecho encima a las configuraciones en azul de la izquierda nos lleva al hal.h
	 * donde se nos muestra la estructura interna del codigo y las distinas formas en las que podiamos
	 * asignar modos, velocidades, etc. */

	userLed.pGPIOx 							= 	GPIOA;
	userLed.pinConfig.GPIO_PinNumber		=	PIN_5;
	userLed.pinConfig.GPIO_PinMode			=	GPIO_MODE_OUT;
	userLed.pinConfig.GPIO_PinOutputType	=	GPIO_OTYPE_PUSHPULL;
	userLed.pinConfig.GPIO_PinOutputSpeed	=	GPIO_OSPEED_MEDIUM;
	userLed.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;

	/* asi se carga de forma recomendada la configuracion de un pin, es buena practica. Primero hacemos los ajustes correspondientes
	 * a la configuracion y luego la cargamos con el comando gpio_Config. La idea de hacerlo seguido es para no olvidar hacer estos
	 * dos pasos.
	 */


	gpio_Config(&userLed);




	/*bit 0*/
	bit0.pGPIOx 							= 	GPIOA;
	bit0.pinConfig.GPIO_PinNumber		=	PIN_9;
	bit0.pinConfig.GPIO_PinMode			=	GPIO_MODE_OUT;
	bit0.pinConfig.GPIO_PinOutputType	=	GPIO_OTYPE_PUSHPULL;
	bit0.pinConfig.GPIO_PinOutputSpeed	=	GPIO_OSPEED_MEDIUM;
	bit0.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;

	gpio_Config(&bit0);


	/*bit 1*/
	bit1.pGPIOx 							= 	GPIOB;
	bit1.pinConfig.GPIO_PinNumber		=	PIN_6;
	bit1.pinConfig.GPIO_PinMode			=	GPIO_MODE_OUT;
	bit1.pinConfig.GPIO_PinOutputType	=	GPIO_OTYPE_PUSHPULL;
	bit1.pinConfig.GPIO_PinOutputSpeed	=	GPIO_OSPEED_MEDIUM;
	bit1.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;

	gpio_Config(&bit1);

	/*bit 2*/
	bit2.pGPIOx 							= 	GPIOC;
	bit2.pinConfig.GPIO_PinNumber		=	PIN_7;
	bit2.pinConfig.GPIO_PinMode			=	GPIO_MODE_OUT;
	bit2.pinConfig.GPIO_PinOutputType	=	GPIO_OTYPE_PUSHPULL;
	bit2.pinConfig.GPIO_PinOutputSpeed	=	GPIO_OSPEED_MEDIUM;
	bit2.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;

	gpio_Config(&bit2);

	/*bit 3*/
	bit3.pGPIOx 							= 	GPIOC;
	bit3.pinConfig.GPIO_PinNumber		=	PIN_6;
	bit3.pinConfig.GPIO_PinMode			=	GPIO_MODE_OUT;
	bit3.pinConfig.GPIO_PinOutputType	=	GPIO_OTYPE_PUSHPULL;
	bit3.pinConfig.GPIO_PinOutputSpeed	=	GPIO_OSPEED_MEDIUM;
	bit3.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;

	gpio_Config(&bit3);

	/*bit 4*/
	bit4.pGPIOx 							= 	GPIOB;
	bit4.pinConfig.GPIO_PinNumber		=	PIN_9;
	bit4.pinConfig.GPIO_PinMode			=	GPIO_MODE_OUT;
	bit4.pinConfig.GPIO_PinOutputType	=	GPIO_OTYPE_PUSHPULL;
	bit4.pinConfig.GPIO_PinOutputSpeed	=	GPIO_OSPEED_MEDIUM;
	bit4.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;

	gpio_Config(&bit4);

	/*bit 5*/
	bit5.pGPIOx 							= 	GPIOA;
	bit5.pinConfig.GPIO_PinNumber		=	PIN_6;
	bit5.pinConfig.GPIO_PinMode			=	GPIO_MODE_OUT;
	bit5.pinConfig.GPIO_PinOutputType	=	GPIO_OTYPE_PUSHPULL;
	bit5.pinConfig.GPIO_PinOutputSpeed	=	GPIO_OSPEED_MEDIUM;
	bit5.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;

	gpio_Config(&bit5);

	/*bit DIRECCION*/
	DIR.pGPIOx 							= 	GPIOA;
	DIR.pinConfig.GPIO_PinNumber		=	PIN_0;
	DIR.pinConfig.GPIO_PinMode			=	GPIO_MODE_OUT;
	DIR.pinConfig.GPIO_PinOutputType	=	GPIO_OTYPE_PUSHPULL;
	DIR.pinConfig.GPIO_PinOutputSpeed	=	GPIO_OSPEED_FAST;
	DIR.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;

	gpio_Config(&DIR);


	/* Nota: se configura el boton de usuario como GPIOx (input), el DIR como output, y el datoboton se configuro como un booleano.
	 * hay que tener presente que en el manual se explica el funcionamiento del boton de usuario, cuando esta presionado ese circuito
	 * esta conectado a tierra, y cuando no el voltaje es VDD, es decir los 3.3V que el micro entrega. Con base en esto configuramos
	 * ese boton que esta regido por el pinC13 en modo IN (como abajo se muestra) y luego hacemos una lectura con la variable que
	 * llamamos userbotton que recolecta un 1 o 0 de ese input; y de forma analoga se cargan esta configuraciones para que DIR
	 * se encienda o se apague.
	 */

	//el boton tiene su propio puerto configurable
	/*info C13*/
	user.pGPIOx 						= 	GPIOC;
	user.pinConfig.GPIO_PinNumber		=	PIN_13;
	user.pinConfig.GPIO_PinMode			=	GPIO_MODE_IN;

	gpio_Config(&user);


/*Nota: se borraron otras lineas de codigo como el toggle, el set, reset, entendiendo que eran una guia
 * para poder visualizar el funcionamiento de las mismas, viendo que no corresponde su uso, fueron eliminadas.
 */







	/* Loop forever. En este loop y para los proximos de los siguientes drivers/tareas es importante tener en cuenta que
	 * lo que se adjunte en este ciclo son procesos que el micro realizara "infinitamente" y siguiendo este criterio, el ajuste
	 * de la configuracion y la cargada de la misma solo es necesario hacerla una unica vez, pero el conteo que debe realizarse en binario,
	 * el delay y saber si en algun momento esta presionado o no el boton azul (del usuario) son procesos que deben realizarse en bucle.*/
	while(1){

		retraso();


	/*  aqui no usamos funcion para saber si el dato que nos da el boton es 1 o 0, se puede configurar directamente. Recordar que el
	 * Bang (!) siempre va antes de la variable/valor. Esta funcion esta antes del contador de forma obligatoria para saber si cuenta
	 * de manera ascendente o descendente.  */

		datoboton = gpio_ReadPin(&user);
		gpio_WritePin (&DIR,!datoboton);


		contador();


		/* para este ejercicio tambien era valido hacer las funciones directamente en el while y no llamarlas, pero por decision del
		 * autor quedaba mas estetico y entendible llamarlas y crearlas por fuera */

	}
}

/*
 * Esta funcion sirve para detectar problemas de parametros
 * incorrectos al momento de ejecutar el programa.
 * */
void assert_failed(uint8_t * file, uint32_t line){
	while(1){

		//problems...
	}
}

//funcion del delay

void retraso (void){

		for (uint32_t contador = 0; contador < 1250000; contador++) {

		}

	}

// funcion para contar en binario ascendiendo o descendiendo

void contador (void){

	/* de manera eficiente hacemos que compare con que el datoboton, que nos indica si el boton de usuario
	 * esta presionado o no, y al mismo tiempo tome la decision de sumar o restar */

if (datoboton == 1) {

	if (conteo < 60) {
	    conteo ++; }
	else {
	    conteo = 0;
	}


}
if (datoboton == 0) {

	if (conteo <= 60) {
	    conteo --; }
	else {
	    conteo = 60;
	}
}

/* las operaciones bitwise fueron lo mas clave del ejercicio, la variable Led0 y las otras correspondientes arrojan un 1 o un 0
 * se usa un AND (&) y shifteos; conteo es el numero decimal (pero que igual el micro lee en binario) que se le aplican estas
 * transformaciones para obtener ese 1 o 0 que nos dice si el led/bit esta encendido o apagado (ya que la representacion lo pide de esa manera)
 * luego de obtener este resultado, con la funcion WritePin (recordar que en los registros se nos dice si se nos permite escribir o no, cuando aparece
 * una 'r' de read, o 'w' de write en el respectivo registro) la cual recibe de argumentos (&variable, set/reset) podemos jugar con este segundo
 * argumento ya que el set/reset son lo mismo que 1 o 0, asi que insertamos nuestra variable en ese espacio dando asi el la accion de encender o
 * apagar ese pin.
 */

Led0 = (conteo & 0b1);
Led1 = (conteo >>1) & 0b1;
Led2 = (conteo >>2) & 0b1;
Led3 = (conteo >>3) & 0b1;
Led4 = (conteo >>4) & 0b1;
Led5 = (conteo >>5) & 0b1;

gpio_WritePin(&bit0, Led0);
gpio_WritePin(&bit1, Led1);
gpio_WritePin(&bit2, Led2);
gpio_WritePin(&bit3, Led3);
gpio_WritePin(&bit4, Led4);
gpio_WritePin(&bit5, Led5);

}

