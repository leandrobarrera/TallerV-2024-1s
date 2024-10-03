/**
 **************************
 * @file           : main.c
 * @author         : lbarreras
 * @brief          : Main program body
 **************************
 */
#include <stdint.h>
#include <stdlib.h>  // Para la función rand()
#include <string.h>
#include "stm32f4xx.h"
#include "stdlib.h"
#include "math.h"
#include "gpio_driver_hal.h"
#include "timer_driver_hal.h"
#include "i2c_driver_hal.h"
//#include "ssd1306.h"
#include "oled_driver.h"
#include "systick_driver_hal.h"

GPIO_Handler_t gpio_buzzer = { 0 };
Timer_Handler_t blinkyTimer = { 0 };
GPIO_Handler_t botonDerecho = { 0 }; 		//	PinC8
GPIO_Handler_t botonCentro = { 0 }; 		//	PinB8
GPIO_Handler_t botonIzquierdo = { 0 }; 		//	PinB8
GPIO_Handler_t pinSda = { 0 };
GPIO_Handler_t pinScl = { 0 };
I2C_Handler_t oled = { 0 };
GPIO_Handler_t blinky = { 0 };

// Definimos las posiciones fijas de los tres cuadrados
#define POS1 24   // Cuadrado izquierdo
#define POS2 56   // Cuadrado central
#define POS3 88   // Cuadrado derecho



// Declaramos una variable para almacenar la puntuación
uint8_t score = 0;
uint8_t finish = 0;
uint8_t tiempoJuego = 0;

void initSystem(void);
void animateRandomSquares(I2C_Handler_t *ptrHandlerI2Ctr);
void clearScreen(I2C_Handler_t *ptrHandlerI2Ctr);
void drawLineOnPage6(I2C_Handler_t *ptrHandlerI2Ctr);;

int main(void) {

	initSystem();
	startOLED(&oled);
	clearDisplay(&oled);
	systick_Delay_ms(1000);
	drawLineOnPage6(&oled);

	while(1){
		if (gpio_ReadPin(&botonIzquierdo) || gpio_ReadPin(&botonCentro)
				|| gpio_ReadPin(&botonDerecho)) {
			finish = 0;
			animateRandomSquares(&oled);
		}
	}
}

void animateRandomSquares(I2C_Handler_t *ptrHandlerI2Ctr) {
	char pageBytes[128] = { 0 };  // Arreglo para una página (128 columnas)
	uint8_t startPage = 0;        // Página inicial
	uint8_t endPage = 7;          // Página final

	// Posiciones fijas de los cuadrados (16 columnas cada uno)
	uint8_t pos1 = 24;  // Posición fija del cuadrado 1 (centrado en el medio)
	uint8_t pos2 = pos1 + 32;  // Posición fija del cuadrado 2
	uint8_t pos3 = pos2 + 32;  // Posición fija del cuadrado 3

	while (!finish) {  // Bucle infinito para la animación
		score = 0;
		tiempoJuego ++;
		if(tiempoJuego == 10){
			finish = 1;
		}

		// Decisiones aleatorias para cada cuadrado
		int draw1 = rand() % 2;  // 0 o 1, si se dibuja o no el cuadrado 1
		int draw2 = rand() % 2;  // 0 o 1, si se dibuja o no el cuadrado 2
		int draw3 = rand() % 2;  // 0 o 1, si se dibuja o no el cuadrado 3

		// Mover los cuadrados desde la página 0 a la página 7
		for (uint8_t page = startPage; page <= endPage; page++) {

			if (page == 6) {

				if (draw1 && draw2 && draw3) {
					if (gpio_ReadPin(&botonIzquierdo)
							&& gpio_ReadPin(&botonCentro)
							&& gpio_ReadPin(&botonDerecho)) {
						score++;
					} else {
						score--;
					}
				} else if (draw1 && draw2) {
					if (gpio_ReadPin(&botonIzquierdo)
							&& gpio_ReadPin(&botonCentro)) {
						score++;
					} else {
						score--;
					}
				} else if (draw2 && draw3) {
					if (gpio_ReadPin(&botonCentro)
							&& gpio_ReadPin(&botonDerecho)) {
						score++;
					} else {
						score--;
					}
				} else if (draw3 && draw1) {
					if (gpio_ReadPin(&botonDerecho)
							&& gpio_ReadPin(&botonIzquierdo)) {
						score++;
					} else {
						score--;
					}
				} else if (draw1) {
					if (gpio_ReadPin(&botonIzquierdo)) {
						score++;
					} else {
						score--;
					}
				} else if (draw2) {
					if (gpio_ReadPin(&botonCentro)) {
						score++;
					} else {
						score--;
					}
				} else if (draw3) {
					if (gpio_ReadPin(&botonDerecho)) {
						score++;
					} else {
						score--;
					}
				}
			} else {
				if (gpio_ReadPin(&botonIzquierdo) || gpio_ReadPin(&botonCentro)
						|| gpio_ReadPin(&botonDerecho)) {
					score--;
				}
			}
			// Borrar la pantalla antes de redibujar
			clearScreen(ptrHandlerI2Ctr);
			drawLineOnPage6(&oled);

			// Dibujar los cuadrados si se ha decidido pintarlos
			if (draw1) {
				for (uint8_t i = 0; i < 16; i++) { // 16 columnas por cada cuadrado
					pageBytes[pos1 + i] = 0xFF;  // Cuadrado 1
				}
			}

			if (draw2) {
				for (uint8_t i = 0; i < 16; i++) { // 16 columnas por cada cuadrado
					pageBytes[pos2 + i] = 0xFF;  // Cuadrado 2
				}
			}

			if (draw3) {
				for (uint8_t i = 0; i < 16; i++) { // 16 columnas por cada cuadrado
					pageBytes[pos3 + i] = 0xFF;  // Cuadrado 3
				}
			}

			// Enviar los datos de la página actual
			setPage(ptrHandlerI2Ctr, page);
			setColumnAddress(ptrHandlerI2Ctr, 0); // Columna de inicio (ajústalo si es necesario)
			sendDataBytes(ptrHandlerI2Ctr, pageBytes, 128);

			// Pausa para crear el efecto de animación
			systick_Delay_ms(150); // Ajusta el valor para hacer la animación más rápida o más lenta

			// Borrar la página actual (apagamos los píxeles) para la próxima iteración
			memset(pageBytes, 0, sizeof(pageBytes));
		}

		// Borrar completamente la pantalla después de llegar a la página 7
		clearScreen(ptrHandlerI2Ctr);

		// Pequeña pausa antes de repetir la animación con nuevas decisiones aleatorias
		systick_Delay_ms(150);  // Pausa entre ciclos de animación
	}
}

void clearScreen(I2C_Handler_t *ptrHandlerI2Ctr) {
	char emptyPage[128] = { 0 };  // Página vacía (todos los píxeles apagados)

	// Borrar todas las páginas de la pantalla
	for (uint8_t page = 0; page < 8; page++) {
		setPage(ptrHandlerI2Ctr, page);
		setColumnAddress(ptrHandlerI2Ctr, 0); // Comenzar desde la primera columna
		sendDataBytes(ptrHandlerI2Ctr, emptyPage, 128); // Enviar una página vacía
	}
}

void drawLineOnPage6(I2C_Handler_t *ptrHandlerI2Ctr) {
	char lineBytes[128] = { 0xFF }; // Inicializar el arreglo con 0xFF para encender todos los píxeles

	// Establecer la página 6
	setPage(ptrHandlerI2Ctr, 6);
	setColumnAddress(ptrHandlerI2Ctr, 0);  // Comenzar desde la columna 0

	// Enviar la línea a la página 6
	sendDataBytes(ptrHandlerI2Ctr, lineBytes, 128);
}

void initSystem(void) {
//
//	SCB->CPACR |= (0xF<<20);

	//Timer del Blinky
	blinkyTimer.pTIMx = TIM5;
	blinkyTimer.TIMx_Config.TIMx_Prescaler = 16000; //	Genera incrementos de 1ms
	blinkyTimer.TIMx_Config.TIMx_Period = 250; //	el prescaler lo ajusta 1ms, entonces lo quiero a 250ms, y es la multiplicacion de uno con el otro.
	blinkyTimer.TIMx_Config.TIMx_mode = TIMER_UP_COUNTER;  //
	blinkyTimer.TIMx_Config.TIMx_InterruptEnable = TIMER_INT_ENABLE;  //

	/*	Configuramos el Timer */
	timer_Config(&blinkyTimer);

	//	Encedemos el Timer.
	timer_SetState(&blinkyTimer, SET);


	/*	botonDerecho	*/
	botonDerecho.pGPIOx = GPIOC;
	botonDerecho.pinConfig.GPIO_PinNumber = PIN_8;
	botonDerecho.pinConfig.GPIO_PinMode = GPIO_MODE_IN;

	gpio_Config(&botonDerecho);

	/*	botonCentro	*/
	botonCentro.pGPIOx = GPIOC;
	botonCentro.pinConfig.GPIO_PinNumber = PIN_6;
	botonCentro.pinConfig.GPIO_PinMode = GPIO_MODE_IN;

	gpio_Config(&botonCentro);

	/*	botonIzquierdo*/
	botonIzquierdo.pGPIOx = GPIOC;
	botonIzquierdo.pinConfig.GPIO_PinNumber = PIN_5;
	botonIzquierdo.pinConfig.GPIO_PinMode = GPIO_MODE_IN;

	gpio_Config(&botonIzquierdo);

	pinScl.pGPIOx = GPIOB;
	pinScl.pinConfig.GPIO_PinNumber = PIN_10;
	pinScl.pinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	pinScl.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_MEDIUM;
	pinScl.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_OPENDRAIN;
	pinScl.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	pinScl.pinConfig.GPIO_PinAltFunMode = AF4;

	gpio_Config(&pinScl);

	pinSda.pGPIOx = GPIOB;
	pinSda.pinConfig.GPIO_PinNumber = PIN_3;
	pinSda.pinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	pinSda.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_MEDIUM;
	pinSda.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_OPENDRAIN;
	pinSda.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	pinSda.pinConfig.GPIO_PinAltFunMode = AF9;

	gpio_Config(&pinSda);

	blinky.pGPIOx = GPIOA;
	blinky.pinConfig.GPIO_PinNumber = PIN_5;
	blinky.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	blinky.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	blinky.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	blinky.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	gpio_Config(&blinky);

	gpio_WritePin(&blinky, 1);

	oled.pI2Cx = I2C2;
	oled.i2c_mode = I2C_MODE_SM_SPEED;
	oled.i2c_mainClock = I2C_MAIN_CLOCK_16_Mhz;
	oled.slaveAddress = 0x3C;

	i2c_Config(&oled);

	config_SysTick_ms(0);

}

void Timer5_Callback(void){
	gpio_TooglePin(&blinky);
}

void assert_failed(uint8_t *file, uint32_t line) {
	while (1) {
		//Problems
	}
}
