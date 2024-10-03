/**
 ******************************************************************************
 * @file           : main.c
 * @author         : lbarreras
 * @brief          : Prueba para implementar la pantalla OLED (prueba de funcionamiento)
 ******************************************************************************
 **/

#include <stdint.h>
#include <stdbool.h>
#include <math.h>


#include "stm32f4xx.h"
#include "stm32_assert.h"
#include "systick_driver_hal.h"
#include "gpio_driver_hal.h"
#include "timer_driver_hal.h"
#include "usart_driver_hal.h"
#include "i2c_driver_hal.h"
#include "oled_driver.h"

// Definicion de los Handlers necesario
GPIO_Handler_t userLed = {0}; //Led de estado PinA5
Timer_Handler_t blinkTimer = {0};
GPIO_Handler_t pinSCL_I2C = {0};
GPIO_Handler_t pinSDA_I2C = {0};
I2C_Handler_t i2c_handler = {0};



/* Elementos para la comunicacion serial  llevar */
USART_Handler_t commSerial = {0};
GPIO_Handler_t pinTx = {0};
GPIO_Handler_t pinRx = {0};
uint8_t usart2DataReceived = 0;
char	bufferMsg[64] = {0};


//Definicion de las cabeceras de las funciones  del main
void configPeripherals(void);

/*
 * Funcion principal del sistema
 */
int main(void){

	/* Configuramos los periféricos */
	configPeripherals();



	/* Loop forever*/
	while (1){

		/* Prueba del USART */
		if (usart2DataReceived == 't'){

			usart_writeMsg(&commSerial, "\r\n");
			usart_writeMsg(&commSerial, "¿Probando? ¡Funciona!  \n\r");
			usart2DataReceived = '\0';
		}


		/* Prueba comunicación serial con la pantalla */
		if (usart2DataReceived == 'a'){

			usart_writeMsg(&commSerial, "\r\n");
			usart_writeMsg(&commSerial, "Intento de comunicación \r\n");
			usart2DataReceived = '\0';

			uint8_t auxRead = 0;


			usart_writeMsg(&commSerial, "\r\n");
			sprintf(bufferMsg, "Respuesta Slave: %u\r\n", (auxRead >> 6 & 1));
			usart_writeMsg(&commSerial, bufferMsg);

			usart2DataReceived = '\0';


		}

		/* Encender la OLED */
		if (usart2DataReceived == '1'){

			oled_onDisplay(&i2c_handler);
			usart_writeMsg(&commSerial, "\r\n");
			usart_writeMsg(&commSerial, "OLED encendida \r\n");

			usart2DataReceived = '\0';
		}

		/* Apagar la OLED */
		if (usart2DataReceived == '2'){

			oled_offDisplay(&i2c_handler);
			usart_writeMsg(&commSerial, "\r\n");
			usart_writeMsg(&commSerial, "OLED apagada \r\n");

			usart2DataReceived = '\0';
		}

		/* Configura la OLED */
		if (usart2DataReceived == 'c'){

			uint8_t array[26] = 	{0xAE, 0x00, 0x10,0x40, 0xB0, 0x81, 0xCF, 0xA1,
									 0xA6, 0xA8, 0x3F, 0xC8, 0xD3, 0x00, 0xD5, 0x80,
									 0xD9, 0xF1, 0xDA, 0x12, 0xDB, 0x20, 0x8D, 0x14,
									 0xAF, 0xAF};
			oled_sendCommand(&i2c_handler, array, 26);
			usart_writeMsg(&commSerial, "\r\n");
			usart_writeMsg(&commSerial, "Comando finalizado -> Debe leer 0\r\n");

			usart2DataReceived = '\0';
		}

		/* Apagar la OLED */
		if (usart2DataReceived == 'y'){

			usart_writeMsg(&commSerial, "\r\n");
			usart_writeMsg(&commSerial, "Pintando negro\r\n");

			usart2DataReceived = '\0';

			uint8_t array[128] = {0};
			array[127] = 0b11111111;

			oled_Config(&i2c_handler);

			oled_sendData(&i2c_handler, array, 128);

		}

		/* Apagar la OLED */
		if (usart2DataReceived == 'x'){
			usart_writeMsg(&commSerial, "\r\n");
			usart_writeMsg(&commSerial, "Préndalo\r\n");

			usart2DataReceived = '\0';

			 oled_Config(&i2c_handler);
			oled_onDisplay(&i2c_handler);
		}

if (usart2DataReceived == 'd'){
	usart_writeMsg(&commSerial, "\r\n");
	usart_writeMsg(&commSerial, "limpiando pantalla \r\n");

	usart2DataReceived = '\0';

	oled_Config(&i2c_handler);

	oled_clearDisplay(&i2c_handler);

	oled_offDisplay(&i2c_handler);
}


	} // Fin while()

	return 0;

} // Fin main()



/*
 * Definimos una función para inicializar las configuraciones
 * de los diferentes periféricos
 */
void configPeripherals(void){

	// 1. ===== PUERTOS Y PINES =====
	/* Configurando el pin para el Blinky */
	userLed.pGPIOx								= GPIOA;
	userLed.pinConfig.GPIO_PinNumber			= PIN_5;	// PinA5 -> Led2 del STM32F411
	userLed.pinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	userLed.pinConfig.GPIO_PinOutputType		= GPIO_OTYPE_PUSHPULL;
	userLed.pinConfig.GPIO_PinOutputSpeed		= GPIO_OSPEED_MEDIUM;
	userLed.pinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;

	/* Cargamos la configuración del pin */
	gpio_Config(&userLed);
	gpio_WritePin(&userLed, SET);

	/* Configurando los pines para el puerto serial
	 * Usamos el PinA2 para TX
	 */
	pinTx.pGPIOx								= GPIOA;
	pinTx.pinConfig.GPIO_PinNumber				= PIN_2;
	pinTx.pinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	pinTx.pinConfig.GPIO_PinAltFunMode			= AF7;
	pinTx.pinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	pinTx.pinConfig.GPIO_PinOutputSpeed			= GPIO_OSPEED_FAST;

	/* Cargamos la configuración */
	gpio_Config(&pinTx);


	/* Usamos el PinA3 para RX */
	pinRx.pGPIOx								= GPIOA;
	pinRx.pinConfig.GPIO_PinNumber				= PIN_3;
	pinRx.pinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	pinRx.pinConfig.GPIO_PinAltFunMode			= AF7;
	pinRx.pinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	pinRx.pinConfig.GPIO_PinOutputSpeed			= GPIO_OSPEED_FAST;

	/* Cargamos la configuración */
	gpio_Config(&pinRx);


	/* Usamos el PinB6 para SCL */
	pinSCL_I2C.pGPIOx								= GPIOB;
	pinSCL_I2C.pinConfig.GPIO_PinNumber				= PIN_6;
	pinSCL_I2C.pinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	pinSCL_I2C.pinConfig.GPIO_PinAltFunMode			= AF4;
	pinSCL_I2C.pinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	pinSCL_I2C.pinConfig.GPIO_PinOutputSpeed		= GPIO_OSPEED_FAST;
	pinSCL_I2C.pinConfig.GPIO_PinOutputType			= GPIO_OTYPE_OPENDRAIN;

	/* Cargamos la configuración */
	gpio_Config(&pinSCL_I2C);

	/* Usamos el PinB9 para SCL */
	pinSDA_I2C.pGPIOx								= GPIOB;
	pinSDA_I2C.pinConfig.GPIO_PinNumber				= PIN_9;
	pinSDA_I2C.pinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	pinSDA_I2C.pinConfig.GPIO_PinAltFunMode			= AF4;
	pinSDA_I2C.pinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	pinSDA_I2C.pinConfig.GPIO_PinOutputSpeed		= GPIO_OSPEED_FAST;
	pinSDA_I2C.pinConfig.GPIO_PinOutputType			= GPIO_OTYPE_OPENDRAIN;

	/* Cargamos la configuración */
	gpio_Config(&pinSDA_I2C);


	// 2. ===== TIMERS =====
	/* Configurando el Timer del Blinky*/
	blinkTimer.pTIMx								= TIM2;
	blinkTimer.TIMx_Config.TIMx_Prescaler			= 16000;	// Genera incrementos de 1 ms
	blinkTimer.TIMx_Config.TIMx_Period				= 250;		// De la mano con el pre-scaler, determina cuando se dispara una interrupción (500 ms)
	blinkTimer.TIMx_Config.TIMx_mode				= TIMER_UP_COUNTER;	// El Timer cuante ascendente
	blinkTimer.TIMx_Config.TIMx_InterruptEnable		= TIMER_INT_ENABLE;	// Se activa la interrupción

	/* Cargamos la configuración del Timer */
	timer_Config(&blinkTimer);

	/* Encendemos el Timer */
	timer_SetState(&blinkTimer, TIMER_ON);


	// 4. ===== USARTS =====
	/* Configurando el puerto serial USART2 */
	commSerial.ptrUSARTx					= USART2;
	commSerial.USART_Config.baudrate		= USART_BAUDRATE_115200;
	commSerial.USART_Config.datasize		= USART_DATASIZE_8BIT;
	commSerial.USART_Config.parity			= USART_PARITY_NONE;
	commSerial.USART_Config.stopbits		= USART_STOPBIT_1;
	commSerial.USART_Config.mode			= USART_MODE_RXTX;
	commSerial.USART_Config.enableIntRX		= USART_RX_INTERRUP_ENABLE;

	/* Cargamos la configuración de USART */
	usart_Config(&commSerial);

	/*
	 * Escribimos el caracter nulo para asegurarnos de empezar
	 * una transmisión "limpia"
	 */
	usart_WriteChar(&commSerial, '\0');


	// 6. ===== I2C =====
	/* Configuramos el I2C */
	i2c_handler.pI2Cx				= I2C1;
	i2c_handler.slaveAddress		= OLED_ADDRESS;
	i2c_handler.i2c_mode				= eI2C_MODE_FM;

	/* Cargamos la configuración del I2C */
	i2c_Config(&i2c_handler);


	// 7. ===== SYSTICK =====
	/* Configuramos el Systick */
	config_SysTick_ms(0);




}	// Fin de la configuración de los periféricos



/*
 * Función para enviar un  comando (configuración) a la pantalla
 */
void oled_sendCommand(I2C_Handler_t *ptrHandlerI2C, uint8_t *command, uint16_t length){

	/* 1. Generamos la condición de "Start" */
	i2c_StartTransaction(ptrHandlerI2C);

	/* 2. Enviamos la dirección del esclavo y la indicación de ESCRIBIR */
	i2c_SendSlaveAddressRW(ptrHandlerI2C, ptrHandlerI2C->slaveAddress, OLED_WRITE);

	/* 3. Enviamos la dirección de memoria que deseamos escribir */
	i2c_SendMemoryAddress(ptrHandlerI2C, CONTROL_BYTE_COMMAND);

	/* 4. Enviamos el valor que deseamos escribir en el registro seleccionado */
	for(uint16_t i = 0; i < length; i++){
		i2c_SendDataByte(ptrHandlerI2C, command[i]);
//		if(i < length){
//			i2c_SendDataByte(ptrHandlerI2C, CONTROL_BYTE_COMMAND);
//		}
	}

	/* 5. Generamos la condición Stop, para que el slave se detenga después de 1 byte */
	i2c_StopTransaction(ptrHandlerI2C);

}


/*
 * Función para enviar un dato a la memoria de la pantalla GDDRAM (Graphic Display Data RAM)
 */
void oled_sendData(I2C_Handler_t *ptrHandlerI2C, uint8_t *data, uint16_t length){

	/* 1. Generamos la condición de "Start" */
	i2c_StartTransaction(ptrHandlerI2C);

	/* 2. Enviamos la dirección del esclavo y la indicación de ESCRIBIR */
	i2c_SendSlaveAddressRW(ptrHandlerI2C, ptrHandlerI2C->slaveAddress, OLED_WRITE);

	/* 3. Enviamos la dirección de memoria que deseamos escribir */
	i2c_SendMemoryAddress(ptrHandlerI2C, CONTROL_BYTE_DATA);

	/* 4. Enviamos el valor que deseamos escribir en el registro seleccionado */
	for(uint16_t i = 0; i < length; i++){
		i2c_SendDataByte(ptrHandlerI2C, data[i]);
//		if(i < length){
//			i2c_SendDataByte(ptrHandlerI2C, CONTROL_BYTE_DATA);
//		}
	}

	/* 5. Generamos la condición Stop, para que el slave se detenga después de 1 byte */
	i2c_StopTransaction(ptrHandlerI2C);

}


/*
 * Función para hacer la matriz de bytes de una letra
 */
void setLetter(uint8_t letter, uint8_t *letterArray){
	/* Inicializamos el arreglo donde se codifica la letra */

	switch(letter){
	case 'A':
	letterArray[0] = 0b01111000;
	letterArray[1] = 0b00010100;
	letterArray[2] = 0b00010010;
	letterArray[3] = 0b00010100;
	letterArray[4] = 0b01111000;
	break;

	case 'a':
	letterArray[0] = 0b00110000;
	letterArray[1] = 0b01001000;
	letterArray[2] = 0b01001000;
	letterArray[3] = 0b00101000;
	letterArray[4] = 0b01111000;
	break;

	case 'o':
	letterArray[0] = 0b00110000;
	letterArray[1] = 0b01001000;
	letterArray[2] = 0b01001000;
	letterArray[3] = 0b00110000;
	letterArray[4] = 0b00000000;
	break;

	case 'B':
	letterArray[0] = 0b01111110;
	letterArray[1] = 0b01001010;
	letterArray[2] = 0b01001010;
	letterArray[3] = 0b01001010;
	letterArray[4] = 0b00110100;
	break;

	case 'b':
	letterArray[0] = 0b01111110;
	letterArray[1] = 0b01010000;
	letterArray[2] = 0b01010000;
	letterArray[3] = 0b01010000;
	letterArray[4] = 0b00100000;
	break;

	case 'C':
	letterArray[0] = 0b00111100;
	letterArray[1] = 0b01000010;
	letterArray[2] = 0b01000010;
	letterArray[3] = 0b01000010;
	letterArray[4] = 0b00100100;
	break;

	case 'c':
	letterArray[0] = 0b00110000;
	letterArray[1] = 0b01001000;
	letterArray[2] = 0b01001000;
	letterArray[3] = 0b01001000;
	letterArray[4] = 0b00100000;
	break;

	case 'D':
	letterArray[0] = 0b01111110;
	letterArray[1] = 0b01000010;
	letterArray[2] = 0b01000010;
	letterArray[3] = 0b01000010;
	letterArray[4] = 0b00111100;
	break;

	case 'd':
	letterArray[0] = 0b00100000;
	letterArray[1] = 0b01010000;
	letterArray[2] = 0b01010000;
	letterArray[3] = 0b01010000;
	letterArray[4] = 0b01111110;
	break;

	case 'E':
	letterArray[0] = 0b01111110;
	letterArray[1] = 0b01001010;
	letterArray[2] = 0b01001010;
	letterArray[3] = 0b01001010;
	letterArray[4] = 0b01000010;
	break;

	case 'e':
	letterArray[0] = 0b00111000;
	letterArray[1] = 0b01010100;
	letterArray[2] = 0b01010100;
	letterArray[3] = 0b01010100;
	letterArray[4] = 0b01001000;
	break;

	case 'F':
	letterArray[0] = 0b01111110;
	letterArray[1] = 0b00001010;
	letterArray[2] = 0b00001010;
	letterArray[3] = 0b00001010;
	letterArray[4] = 0b00000010;
	break;

	case 'f':
	letterArray[0] = 0b01111000;
	letterArray[1] = 0b00010100;
	letterArray[2] = 0b00010100;
	letterArray[3] = 0b00000100;
	letterArray[4] = 0b00001000;
	break;

	case 'G':
	letterArray[0] = 0b00111100;
	letterArray[1] = 0b01000010;
	letterArray[2] = 0b01010010;
	letterArray[3] = 0b01010010;
	letterArray[4] = 0b00110100;
	break;

	case 'g':
	letterArray[0] = 0b00100000;
	letterArray[1] = 0b01001000;
	letterArray[2] = 0b01010100;
	letterArray[3] = 0b01010100;
	letterArray[4] = 0b00111000;
	break;

	case 'H':
	letterArray[0] = 0b01111110;
	letterArray[1] = 0b00001000;
	letterArray[2] = 0b00001000;
	letterArray[3] = 0b00001000;
	letterArray[4] = 0b01111110;
	break;

	case 'I':
	letterArray[0] = 0b01000010;
	letterArray[1] = 0b01000010;
	letterArray[2] = 0b01111110;
	letterArray[3] = 0b01000010;
	letterArray[4] = 0b01000010;
	break;

	case 'J':
	letterArray[0] = 0b00110010;
	letterArray[1] = 0b01000010;
	letterArray[2] = 0b01000010;
	letterArray[3] = 0b00111110;
	letterArray[4] = 0b00000010;
	break;

	case 'K':
	letterArray[0] = 0b01111110;
	letterArray[1] = 0b00010000;
	letterArray[2] = 0b00011000;
	letterArray[3] = 0b00100100;
	letterArray[4] = 0b01000010;
	break;

	case 'L':
	letterArray[0] = 0b01111110;
	letterArray[1] = 0b01000000;
	letterArray[2] = 0b01000000;
	letterArray[3] = 0b01000000;
	letterArray[4] = 0b01000000;
	break;

	case 'l':
	letterArray[0] = 0b01111110;
	letterArray[1] = 0b01000000;
	letterArray[2] = 0b00000000;
	letterArray[3] = 0b00000000;
	letterArray[4] = 0b00000000;
	break;

	case 'M':
	letterArray[0] = 0b01111110;
	letterArray[1] = 0b00000100;
	letterArray[2] = 0b00001000;
	letterArray[3] = 0b00000100;
	letterArray[4] = 0b01111110;
	break;

	case 'm':
	letterArray[0] = 0b01111110;
	letterArray[1] = 0b00000100;
	letterArray[2] = 0b00001000;
	letterArray[3] = 0b00000100;
	letterArray[4] = 0b01111110;
	break;

	case 'N':
	letterArray[0] = 0b01111110;
	letterArray[1] = 0b00000100;
	letterArray[2] = 0b00011000;
	letterArray[3] = 0b00100000;
	letterArray[4] = 0b01111110;
	break;

	case 'O':
	letterArray[0] = 0b00111100;
	letterArray[1] = 0b01000010;
	letterArray[2] = 0b01000010;
	letterArray[3] = 0b01000010;
	letterArray[4] = 0b00111100;
	break;

	case 'P':
	letterArray[0] = 0b01111110;
	letterArray[1] = 0b00010010;
	letterArray[2] = 0b00010010;
	letterArray[3] = 0b00010010;
	letterArray[4] = 0b00001100;
	break;

	case 'p':
	letterArray[0] = 0b01111110;
	letterArray[1] = 0b00010010;
	letterArray[2] = 0b00010010;
	letterArray[3] = 0b00010010;
	letterArray[4] = 0b00001100;
	break;

	case 'Q':
	letterArray[0] = 0b00011100;
	letterArray[1] = 0b00100010;
	letterArray[2] = 0b00110010;
	letterArray[3] = 0b01110010;
	letterArray[4] = 0b01011100;
	break;

	case 'R':
	letterArray[0] = 0b01111110;
	letterArray[1] = 0b00010010;
	letterArray[2] = 0b00010010;
	letterArray[3] = 0b00110010;
	letterArray[4] = 0b01001100;
	break;

	case 'r':
	letterArray[0] = 0b01111000;
	letterArray[1] = 0b00010000;
	letterArray[2] = 0b00010000;
	letterArray[3] = 0b00110000;
	letterArray[4] = 0b00000000;
	break;

	case 'S':
	letterArray[0] = 0b00100100;
	letterArray[1] = 0b01001010;
	letterArray[2] = 0b01001010;
	letterArray[3] = 0b01001010;
	letterArray[4] = 0b00110100;
	break;

	case 's':
	letterArray[0] = 0b00100100;
	letterArray[1] = 0b01001010;
	letterArray[2] = 0b01001010;
	letterArray[3] = 0b01001010;
	letterArray[4] = 0b00110100;
	break;

	case 'T':
	letterArray[0] = 0b00000010;
	letterArray[1] = 0b00000010;
	letterArray[2] = 0b01111110;
	letterArray[3] = 0b00000010;
	letterArray[4] = 0b00000010;
	break;

	case 't':
	letterArray[0] = 0b00000000;
	letterArray[1] = 0b00000100;
	letterArray[2] = 0b01111110;
	letterArray[3] = 0b01000100;
	letterArray[4] = 0b00000000;
	break;

	case 'U':
	letterArray[0] = 0b00111110;
	letterArray[1] = 0b01000000;
	letterArray[2] = 0b01000000;
	letterArray[3] = 0b01000000;
	letterArray[4] = 0b00111110;
	break;

	case 'u':
	letterArray[0] = 0b00111000;
	letterArray[1] = 0b01000000;
	letterArray[2] = 0b01000000;
	letterArray[3] = 0b01000000;
	letterArray[4] = 0b00111000;
	break;

	case 'V':
	letterArray[0] = 0b00011110;
	letterArray[1] = 0b00100000;
	letterArray[2] = 0b01000000;
	letterArray[3] = 0b00100000;
	letterArray[4] = 0b00011110;
	break;

	case 'W':
	letterArray[0] = 0b00111110;
	letterArray[1] = 0b01000000;
	letterArray[2] = 0b00111000;
	letterArray[3] = 0b01000000;
	letterArray[4] = 0b00111111;
	break;

	case 'X':
	letterArray[0] = 0b01100010;
	letterArray[1] = 0b00010110;
	letterArray[2] = 0b00001000;
	letterArray[3] = 0b00010110;
	letterArray[4] = 0b01100010;
	break;

	case 'Y':
	letterArray[0] = 0b00000110;
	letterArray[1] = 0b00001000;
	letterArray[2] = 0b01110000;
	letterArray[3] = 0b00001000;
	letterArray[4] = 0b00000110;
	break;

	case 'Z':
	letterArray[0] = 0b01100010;
	letterArray[1] = 0b01010010;
	letterArray[2] = 0b01001010;
	letterArray[3] = 0b01000110;
	letterArray[4] = 0b01000010;
	break;

	case '!':
	letterArray[0] = 0b01011110;
	letterArray[1] = 0b00000000;
	letterArray[2] = 0b00000000;
	letterArray[3] = 0b00000000;
	letterArray[4] = 0b00000000;
	break;

	case '0':
	letterArray[0] = 0b00111100;
	letterArray[1] = 0b01000110;
	letterArray[2] = 0b01011010;
	letterArray[3] = 0b01100010;
	letterArray[4] = 0b00111100;
	break;

	case '1':
	letterArray[0] = 0b00001000;
	letterArray[1] = 0b00000100;
	letterArray[2] = 0b01111110;
	letterArray[3] = 0b00000000;
	letterArray[4] = 0b00000000;
	break;

	case '2':
	letterArray[0] = 0b01001100;
	letterArray[1] = 0b01100010;
	letterArray[2] = 0b01010010;
	letterArray[3] = 0b01001010;
	letterArray[4] = 0b01000100;
	break;

	case '3':
	letterArray[0] = 0b00100100;
	letterArray[1] = 0b01000010;
	letterArray[2] = 0b01001010;
	letterArray[3] = 0b01001010;
	letterArray[4] = 0b00110100;
	break;

	case '4':
	letterArray[0] = 0b00010000;
	letterArray[1] = 0b00011000;
	letterArray[2] = 0b00010100;
	letterArray[3] = 0b01111110;
	letterArray[4] = 0b00010000;
	break;

	case '5':
	letterArray[0] = 0b01001110;
	letterArray[1] = 0b01001010;
	letterArray[2] = 0b01001010;
	letterArray[3] = 0b01001010;
	letterArray[4] = 0b00110010;
	break;

	case '6':
	letterArray[0] = 0b00111100;
	letterArray[1] = 0b01010010;
	letterArray[2] = 0b01010010;
	letterArray[3] = 0b01010010;
	letterArray[4] = 0b00100100;
	break;

	case '7':
	letterArray[0] = 0b01000010;
	letterArray[1] = 0b00100010;
	letterArray[2] = 0b00010010;
	letterArray[3] = 0b00001010;
	letterArray[4] = 0b00000110;
	break;

	case '8':
	letterArray[0] = 0b00110100;
	letterArray[1] = 0b01001010;
	letterArray[2] = 0b01001010;
	letterArray[3] = 0b01001010;
	letterArray[4] = 0b00110100;
	break;

	case '9':
	letterArray[0] = 0b00100100;
	letterArray[1] = 0b01001010;
	letterArray[2] = 0b01001010;
	letterArray[3] = 0b01001010;
	letterArray[4] = 0b00111100;
	break;

	case '/':
	letterArray[0] = 0b01000000;
	letterArray[1] = 0b00100000;
	letterArray[2] = 0b00011000;
	letterArray[3] = 0b00000100;
	letterArray[4] = 0b00000010;
	break;

	case ':':
	letterArray[0] = 0b00000000;
	letterArray[1] = 0b00000000;
	letterArray[2] = 0b00101000;
	letterArray[3] = 0b00000000;
	letterArray[4] = 0b00000000;
	break;

	case '-':
	letterArray[0] = 0b00000000;
	letterArray[1] = 0b00010000;
	letterArray[2] = 0b00010000;
	letterArray[3] = 0b00010000;
	letterArray[4] = 0b00000000;
	break;

	case '_':
	letterArray[0] = 0b01000000;
	letterArray[1] = 0b01000000;
	letterArray[2] = 0b01000000;
	letterArray[3] = 0b01000000;
	letterArray[4] = 0b01000000;
	break;

	case '.':
	letterArray[0] = 0b00000000;
    letterArray[1] = 0b00000000;
    letterArray[2] = 0b00000000;
    letterArray[3] = 0b01100000;
    letterArray[4] = 0b01100000;
    break;

	case 167:
	letterArray[0] = 0b00001110;
	letterArray[1] = 0b00010001;
	letterArray[2] = 0b00010001;
	letterArray[3] = 0b00010001;
	letterArray[4] = 0b00001110;
	break;

	case '=':
	letterArray[0] = 0b00010000;
	letterArray[1] = 0b00010000;
	letterArray[2] = 0b00010000;
	letterArray[3] = 0b00010000;
	letterArray[4] = 0b00010000;
	break;

	case ' ':
	letterArray[0] = 0b00000000;
	letterArray[1] = 0b00000000;
	letterArray[2] = 0b00000000;
	letterArray[3] = 0b00000000;
	letterArray[4] = 0b00000000;
	break;

	default:{
	__NOP();
	break;
	}
	}

}


/*
 * Función para escribir una palabra en un segmento específico de la pantalla
 */
void oled_setString(I2C_Handler_t *ptrHandlerI2C, uint8_t *string, uint8_t display_mode, uint8_t length, uint8_t start_column, uint8_t start_page){

	/* Ubicamos el area donde se escribirá la palabra
	 * - Una letra ocupa 5 columnas (5 páginas -> 8 bits por página)
	 */
	uint8_t array[6] = 	{0x21,start_column, (start_column+(6*length)-1),
			0x22, start_page, start_page};
	oled_sendCommand(ptrHandlerI2C, array, 6);

	/*
	 * Escribimos el string letra por letra
	 */
	uint8_t letterArray[6];

	if(display_mode == NORMAL_DISPLAY){
		letterArray[5] = 0x00;
		uint8_t i = 0;
		while( i < length){
			setLetter(string[i], letterArray);
			oled_sendData(ptrHandlerI2C, letterArray, 6);
			i++;
		}
	}
	else if(display_mode == INVERSE_DISPLAY){
		letterArray[5] = 0xFF;
		uint8_t i = 0;
		while( i < length){
			setLetter(string[i], letterArray);
			for(uint8_t x = 0; x < 6; x++){
				letterArray[x-1] = ~letterArray[x-1];
			}
			oled_sendData(ptrHandlerI2C, letterArray, 6);
			i++;
		}
	}

}

/*
 * Función para limpiar toda la pantalla OLED
 */
void oled_clearDisplay(I2C_Handler_t *ptrHandlerI2C){

	/* Establecemos los límites de las columnas y páginas de la pantalla */
	oled_setColumn(ptrHandlerI2C, 0, 127);
	oled_setPage(ptrHandlerI2C, 0, 7);

	/* Pintamos todos los píxeles de negro (All -> 0x00) */
	uint8_t array[1024] = {0};
	oled_sendData(ptrHandlerI2C, array, 1024);

	/* Ubicamos el puntero al inicio de la primera columna y la primera página */
	array[0] = 0x40;
	oled_sendCommand(ptrHandlerI2C, array, 1);
}

/*
 * Función para encender la OLED (Carga lo que haya quedado almacenado en la GDDRAM
 */
void oled_onDisplay(I2C_Handler_t *ptrHandlerI2C){
	/* Comando para encender la pantalla OLED */
	uint8_t array[1] = {0xAF};
	oled_sendCommand(ptrHandlerI2C, array, 1);
}


/*
 * Función para apagar la OLED (Sleep mode)
 */
void oled_offDisplay(I2C_Handler_t *ptrHandlerI2C){
	/* Comando para apagar la pantalla OLED */
	uint8_t array[1] = {0xAE};
	oled_sendCommand(ptrHandlerI2C, array, 1);
}


/*
 * Función para cargar la configuración inicial de la OLED
 */
void oled_Config(I2C_Handler_t *ptrHandlerI2C){

	/* 1. Cargamos la secuencia de comandos */
	uint8_t buffer[30] = 	{0xAE, 0x20, 0x00, 0x21, 0, 127, 0x22, 0, 7, 0x40, 0x81, 0xCF, 0xA1,
							 0xA6, 0xA8, 0x3F, 0xC8, 0xD3, 0x00, 0xD5, 0x80,
							 0xD9, 0xF1, 0xDA, 0x12, 0xDB, 0x20, 0x8D, 0x14,
							 0xAF};

	/* 2. Se envía la configuración por I2C */
	oled_sendCommand(ptrHandlerI2C, buffer, 30);
}


/*
 * Función para definir el modo de direccionamiento de memoria de la pantalla.
 * Cada modo, define la manera en que se hará la escritura de los píxeles
 * en la pantalla y, por tanto, el orden como debemos enviar la información (sendData)
 */
void oled_setAddressingMode(I2C_Handler_t *ptrHandlerI2C, uint8_t mode){
	uint8_t array[2] = {0};
	array[0] = 0x20;
	array[1] = mode;
	oled_sendCommand(ptrHandlerI2C, array, 2);
}


/*
 * Función para establecer el constraste de la pantalla
 */
void oled_setContrastControl(I2C_Handler_t *ptrHandlerI2C, uint8_t contrast){
	uint8_t array[2] = {0};
	array[0] = 0x81;
	array[1] = contrast;
	oled_sendCommand(ptrHandlerI2C, array, 2);

}


/*
 * Función para indicar si el SEG0 está ubicado en la columna 0 o en la columna 127
 */
void oled_setSegmentRemap(I2C_Handler_t *ptrHandlerI2C, uint8_t column_address){
	oled_sendCommand(ptrHandlerI2C, &column_address, 1);
}


/*
 * Función que envía un comando para configurar la pantalla en modo Normal
 */
void oled_setNormalDisplay(I2C_Handler_t *ptrHandlerI2C){
	oled_sendCommand(ptrHandlerI2C, (uint8_t *)0xA6, 1);
}


/* ===== Las siguientes funciones establecen la manera en cómo funcionará el =====
 * ===== puntero de filas y columnas para los modos de direccionamiento de   =====
 * ===== memoria Horizontal y Vertical										 =====
 */
/*
 * Envía el valor de la columna inicial y final para los modos Horizontal y Vertical del
 * Direccionamiento de memoria
 */
void oled_setColumn(I2C_Handler_t *ptrHandlerI2C, uint8_t start_column, uint8_t end_column){
	uint8_t array[3] = {0x21, start_column, end_column};
	oled_sendCommand(ptrHandlerI2C, array, 3);
}


/*
 * Función para enviar el valor inicial y final de las páginas que se usaran, para los modos
 * horizontal y vertical en el Direccionamiento de memoria
 */
void oled_setPage(I2C_Handler_t *ptrHandlerI2C, uint8_t start_page, uint8_t end_page){
	uint8_t array[3] = {0x22, start_page, end_page};
	oled_sendCommand(ptrHandlerI2C, array, 3);
}

/* ===== CALLBACKS ===== */

/* Callback de Timer 2 (Controla el userLed) */
void Timer2_Callback(void){
	gpio_TooglePin(&userLed);
}


void usart2_RxCallback(void){
	usart2DataReceived = usart_getRxData();
}

/*
 * Función assert para detectar problemas de paŕametros incorrectos
 */
void assert_failed(uint8_t* file, uint32_t line){
	while(1){
		// problems...
	}
}
