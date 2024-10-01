/*
 * i2c_driver_hal.c
 *
 *  Created on: 14/09/2024
 *      Author: lbarreras
 */


#include <stdint.h>
#include "i2c_driver_hal.h"
#include "gpio_driver_hal.h"

/* === Headers for private functions === */

static void i2c_enable_clock_peripheral(I2C_Handler_t *pHandlerI2C);
static void i2c_soft_reset(I2C_Handler_t *pHandlerI2C);
static void i2c_set_main_clock(I2C_Handler_t *pHandlerI2C);
static void i2c_set_mode(I2C_Handler_t *pHandlerI2C);
static void i2c_enable_port(I2C_Handler_t *pHandlerI2C);
static void i2c_disable_port(I2C_Handler_t *pHandlerI2C);
static void i2c_stop_signal(I2C_Handler_t *pHandlerI2C);
static void i2c_start_signal(I2C_Handler_t *pHandlerI2C);
static void i2c_restart_signal(I2C_Handler_t *pHandlerI2C);
static void i2c_send_no_ack(I2C_Handler_t *pHandlerI2C);
static void i2c_send_ack(I2C_Handler_t *pHandlerI2C);
static void i2c_send_slave_address_rw(I2C_Handler_t *pHandlerI2C, uint8_t rw);
static void i2c_send_memory_address(I2C_Handler_t *pHandlerI2C, uint8_t memAddr);
static void i2c_send_close_comm(I2C_Handler_t *pHandlerI2C);
static void i2c_send_byte(I2C_Handler_t *pHandlerI2C, uint8_t dataToWrite);
static uint8_t i2c_read_byte(I2C_Handler_t *pHandlerI2C);



//static void i2c_config_interrupt(I2C_Handler_t *pHandlerI2C);

/*
 * Recordar que se debe configurar los pines para el I2C (SDA y SLC),
 * para lo cual se necesita el modulo GPIO y los pines configurados
 * en el modo Alternate Function.
 * Ademas, estos pines deben ser configurados como salidas open-drain
 * y con la resistencias en modo pull-up.
 */
void i2c_Config(I2C_Handler_t *pHandlerI2C){
	/* 1. Activamos la senal de reloj para el periferico */
	i2c_enable_clock_peripheral(pHandlerI2C);

	/* disable i2c port */
	i2c_disable_port(pHandlerI2C);

	/* 2. Reiniciamos el periferico para comenzar desde un estado conocido */
	i2c_soft_reset(pHandlerI2C);

	/* 3. Configuramos la senal de reloj principal, la cual es usada porr el periferico para generar la senal del bus del i2c */
	i2c_set_main_clock(pHandlerI2C);

	/* 4. Configuramos el modo i2c en el cual el sistema funciona
	 * aca tambien se incluye la velocidad de reloj
	 * y el tiempo maximo para el cambio de senal (T-Rise). */
	i2c_set_mode(pHandlerI2C);

	/* 5. Activamos el modulo de I2C */
	i2c_enable_port(pHandlerI2C);


}

/*
 * Activa la señal de reloj (RCC) para los I2C
 *  */
static void i2c_enable_clock_peripheral(I2C_Handler_t *pHandlerI2C)
{
	/* 1. Activamos la señal de reloj para el modulo I2C seleccionado */
	if (pHandlerI2C->pI2Cx == I2C1){
		RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	}
	else if (pHandlerI2C->pI2Cx == I2C2){
		RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
	}
	else if (pHandlerI2C->pI2Cx == I2C3){
		RCC->APB1ENR |= RCC_APB1ENR_I2C3EN;
	}
}

/**/
static void i2c_set_main_clock(I2C_Handler_t *pHandlerI2C)
{
	pHandlerI2C->pI2Cx->CR2 &= ~(0b111111 << I2C_CR2_FREQ_Pos); //limpiamos la configuracion previa
	pHandlerI2C->pI2Cx->CR2 |= (pHandlerI2C->i2c_mainClock << I2C_CR2_FREQ_Pos);
}

/**/
static void i2c_set_mode(I2C_Handler_t *pHandlerI2C)
{
	/* Borramos la informacion de ambos registros (aunque esto lo debe hacer el reset) */
	pHandlerI2C->pI2Cx->CCR = 0;
	pHandlerI2C->pI2Cx->TRISE = 0;

	if (pHandlerI2C->i2c_mode == eI2C_MODE_SM){

		//Estamos en modo "standar" (SM Mode)
		// Seleccionamos el modo estandar
		pHandlerI2C->pI2Cx->CCR &= ~I2C_CCR_FS;

		// Configuramos el registro que se encarga de generar la señal del reloj
		pHandlerI2C->pI2Cx->CCR |= (I2C_MODE_SM_SPEED << I2C_CCR_CCR_Pos);

		pHandlerI2C->pI2Cx->TRISE |= I2C_MAX_RISE_TIME_SM;
	}else{
		pHandlerI2C->pI2Cx->CCR |= I2C_CCR_FS;
		pHandlerI2C->pI2Cx->CCR |= (I2C_MODE_FM_SPEED << I2C_CCR_CCR_Pos);
		pHandlerI2C->pI2Cx->TRISE |= (I2C_MAX_RISE_TIME_FM);

	}

}

/*
 * Activa el puerto, sin esto el sistema no funciona.
 * El puerto debe estar desactivado para poder ser configurado, si ste intenta
 * configurar cuando esta activo se genera un error
 */

static void i2c_enable_port(I2C_Handler_t *pHandlerI2C)
{
	pHandlerI2C->pI2Cx->CR1 |= I2C_CR1_PE;
}

/*
 * Desactiva el puerto, haciendo que el sistema no funciona y habilita para que se pueda configurar.
 * El puerto debe estar desactivado para poder ser configurado, si se intenta
 * configurar cuando esta activo se genera un error
 * */
static void i2c_disable_port(I2C_Handler_t *pHandlerI2C)
{
	pHandlerI2C->pI2Cx->CR1 &= ~I2C_CR1_PE;
}

/*
 * Esta funcion se encarga de llevar a 0 a todos los bits relacionados con el periferico I2C
 * que se esta utilizando
 * */
static void i2c_soft_reset(I2C_Handler_t *pHandlerI2C)
{
	pHandlerI2C->pI2Cx->CR1 |= I2C_CR1_SWRST;
	__NOP();
	__NOP();
	pHandlerI2C->pI2Cx->CR1 &= ~I2C_CR1_SWRST;
}

/* 8. Generamos la condicion de stop */
static void i2c_stop_signal(I2C_Handler_t *pHandlerI2C)
{
	/* 7. Generamos la condicion de stop */
	pHandlerI2C->pI2Cx->CR1 |= I2C_CR1_STOP;
}

/*
 * Funcion que genera la señal START de un ciclo de comunicacion I2C.
 * El codigo esta relacionado con la figura 164 de la (Pag 481) del manual de referencia
 * del MCU.
 *
 * 1. Configuramos bit I2C CR1 POS
 * 2. Generamos la senal "start"
 * 2a. Esperamos que la bandera del evento "start" se levante.
 * 3. Leemos el registro SR1
 * Estos pasos hacen parte del evento EV5 de la figura 164.
 * */
static void i2c_start_signal(I2C_Handler_t *pHandlerI2C)
{
	/* 0. Definimos una variable auxiliar */
	uint8_t auxByte = 0;
	(void) auxByte;

	// Errata del MCU... solucion en el foro de ST.
	// usuario "ERol.1"
	//URL

	/* 0. Reset, para tener el periferico en un estado conocido. */
	//i2c_soft_reset(pHandlerI2C);

	/*1. Configuramos el control para genera el bit ACK.
	 * Este bit posicion lo que hace es controla si nuestro bit ACK genera dicha
	 * señal para el byte que se esta leyendo actualmente (I2C_CR1_POS = 0) o para el byte
	 * que llegar posteriormente (I2C_CR1_POS = 1)
	 * Lo mas logico es trabajar con el byte que se esta recibiendo actualmente.
	 * */
	pHandlerI2C->pI2Cx->CR1 &= ~I2C_CR1_POS;

	/*2. Generamos la senal de "start" */
	pHandlerI2C->pI2Cx->CR1 |= I2C_CR1_START;

	/* 2a. Esperamos que la bandera del evento start se levante.
	 * Este bit se hace 1 si y solo si la senal de start se genera satisfactoriamente.
	 * Mientras esperamos el valor del SB es 0, por ende la negacion <(!) es un 1
	 * */
	while(!(pHandlerI2C->pI2Cx->SR1 & I2C_SR1_SB)){
		__NOP();
	}

	/* El sistema espero que el registro SR1 sea leido, para continuar con el siguiente paso, que es
	 * enviar la señal del esclavo (siempre)
	 * Pag 479 del manual
	 * Esta condicion de leer el registro SR1 es necesaria y siempre se debe hacer.
	 * */
	auxByte = pHandlerI2C->pI2Cx->SR1;


}

/* Segun el manual, hacer una señal de start luego de la transferencia de un byte
 * nos genera una senal de restart, por lo tanto es suficiente con un nuevo start
 * */
static void i2c_restart_signal(I2C_Handler_t *pHandlerI2C)
{
	/* 2. Generamos una senal de "start" */
	i2c_start_signal(pHandlerI2C);
}

/*
 * Activamos la indicacion para no-ACK, la cual es utilizada en el protocolo
 * como indicacion para el Slave de terminar de transmitir un conjunto de datos
 * */
static void i2c_send_no_ack(I2C_Handler_t *pHandlerI2C)
{
	/* (Debemos escribir cero en la posicion ACK del registro de control 1) */
	pHandlerI2C->pI2Cx->CR1 &= ~I2C_CR1_ACK;
}

/* Activamos la indicacion ACK,la cual es utilizada por el protocolo I2C
 * como indicacion para el Slave, para que continue enviando informacion, o sea
 * para que nevie el siguiente byte en memoria.
 * */
static void i2c_send_ack(I2C_Handler_t *pHandlerI2C)
{
	pHandlerI2C->pI2Cx->CR1 |= I2C_CR1_ACK;
}

/* Este es el paso siguien a la señal start (o restart) y siempre es la direccion del
 * esclavo mas la indicaicon de si se desea leer (1) o escribir (0)
 * Se debe tener en cuenta que la direccion del esclavo sera movida una casilla de derecha a izquierda
 * para generar el "espacio" para la indicacion R/W, por lo cual la direccion del esclavo tiene
 * que ser escrita correctamente desde el inicio, si esto no se hace el sistema enviara una direccion
 * que simplemente el esclavo no va a entender.
 * esto es, se hace la operacion: (SlaveAddress <<1) | R/W
 *
 * Descrito entre lso eventos Ev6 y EV6 de la figura 164.
 * */
static void i2c_send_slave_address_rw(I2C_Handler_t *pHandlerI2C, uint8_t rw)
{
	/* 0. Definimos una variable auxiliar */
	uint8_t auxByte = 0;
	(void) auxByte;

	/* 3. Enviamos la direccion del esclavo y el bit que indica que deseamos escribir (0)
	 * (en el paso siguiente se envia la direccion de memoria que se desea escribir)*/
	pHandlerI2C->pI2Cx->DR = (pHandlerI2C->slaveAddress << 1) | rw;

	/* 3.1. Esperamos hasta que  la bandera del evento "addr" se levante
	 * (esto nos indica que la direccion fue enviada satisfactoriamente) */
	while (!(pHandlerI2C->pI2Cx->SR1 & I2C_SR1_ADDR)){
		__NOP();
	}

	/* 3.2 Debemos limpiar la bandera de la recepcion de ACK de la "addr", para lo cual
	 * debemos leer en secuencia primero el I2C SR1 y luego I2C_SR2 */
	auxByte = pHandlerI2C->pI2Cx->SR1;
	auxByte = pHandlerI2C->pI2Cx->SR2;
	/* El bit TRA SR2 nos indica si el equipo quedo en modo transmision
	 * o en modo recepcion, lo cual esta definido por el tipo de  seleccion de R/W que se envio
	 * Pagina 480 del manual.
	 * */

	/* Esperamos hasta que el byte sea montado en el DSR, quedando el DR libre de nuevo
	 * Este paso no es claramente especificado en el manual, pero es la forma de verificar
	 * que se pueda continuar, ya que el DR quedara libre.
	 * */
	__NOP(); //para verificar en modo debug.
}

static void i2c_send_byte(I2C_Handler_t *pHandlerI2C, uint8_t dataToWrite)
{
	/* 5. Cargamos el valor que deseamos escribir */
	pHandlerI2C->pI2Cx->DR = dataToWrite;

	/* 5.1. Esperamos hasta que el byte sea montado en el DSR, quedando el DR libre de nuevo */
	while(!(pHandlerI2C->pI2Cx->SR1 & I2C_SR1_TXE)){
		__NOP();
	}
}

/* Esta funcion se aproveha del hecho de que enviar la direccion de memoria que se desea leer
 *  es igual a enviar un byte X cualquiera, por lo cual simplemente se llama a la funcion enviar un
 * byte generico, el cual es la posicion de memoria que se desea leer
 */
void i2c_send_memory_address(I2C_Handler_t *pHandlerI2C, uint8_t memAddr){
	i2c_send_byte(pHandlerI2C, memAddr);
}

/* Esta funcion evalua que el ultimo byte transmitido haya sido enviado completamente por el TSR,
 *  luego de esto, transmite la senal de stop
 * */
void i2c_send_close_comm(I2C_Handler_t *pHandlerI2C)
{
	/* 5.1 Esperamos hasta que el ultimo byte sea transmitido completamente por el TSR
	 * lo cual activa el bit BTF, quedando tanto BTF como TXE en 1
	 * */
//	while (!(pHandlerI2C->pI2Cx->SR1 & I2C_SR1_RXNE)){
//		__NOP();
//	}

	/* Enviamos la señal de detencion */
	i2c_stop_signal(pHandlerI2C);

}

uint8_t i2c_read_byte(I2C_Handler_t *pHandlerI2C)
{
	/* 9. Esperamos hasta que el byte entrante sea recibido */
	while(!(pHandlerI2C->pI2Cx->SR1 & I2C_SR1_RXNE)){
		__NOP();
	}

	pHandlerI2C->i2c_data = pHandlerI2C->pI2Cx->DR;

	return pHandlerI2C->i2c_data;
}

/* === Funciones Publicas del Driver ========== */

/**/
uint8_t i2c_ReadSingleRegister(I2C_Handler_t *pHandlerI2C, uint8_t regToRead){
	/* 0. Creamos una variable auxiliar para recibir el dato que leemos */
	uint8_t auxRead = 0;

	/* 1. Generamos la condicion Start */
	i2c_start_signal(pHandlerI2C);

	/* 2. Enviamos la direccion del esclavo y la indicacion de ESCRIBIR */
	i2c_send_slave_address_rw(pHandlerI2C, eI2C_WRITE_DATA);

	/* 3. Enviamos la direccion de memoria que deseamos leer */
	i2c_send_memory_address(pHandlerI2C, regToRead);

	/* 4. Creamos una condicion re reStart */
	i2c_restart_signal(pHandlerI2C);

	/* 5. Enviamos la direccion del esclavo y la indicacion de LEER */
	i2c_send_slave_address_rw(pHandlerI2C, eI2C_READ_DATA);

	/* 8. Leemos el dato que envia el esclavo */
	auxRead = i2c_read_byte(pHandlerI2C);

	/* 6. Generamos la condicion de NoAck, para quel Master no responda y el slave solo envie 1 byte */
	i2c_send_no_ack(pHandlerI2C);

	/* 7. Generamos la condicion Stop, para que el slave se detenga despues de 1 byte */
	i2c_stop_signal(pHandlerI2C);

	return auxRead;

}

uint8_t i2c_ReadManyRegisters(I2C_Handler_t *pHandlerI2C, uint8_t regToread, uint8_t *bufferRxData, uint8_t numberOfBytes){

	/* 1. Generamos la condicion Start */
	i2c_start_signal(pHandlerI2C);

	/* 2. Enviamos la direccion del esclavo y la indicacion de ESCRIBIR */
	i2c_send_slave_address_rw(pHandlerI2C, eI2C_WRITE_DATA);

	/* 3. Enviamos la direccion de memoria que deseamos leer */
	i2c_send_memory_address(pHandlerI2C, regToread);

	/* 4. Creamos una condicion re reStart */
	i2c_restart_signal(pHandlerI2C);

	/* Activamos el envio de ACK */
	i2c_send_slave_address_rw(pHandlerI2C, eI2C_READ_DATA);

	/* 8. Leemos el dato que envia el esclavo */
	i2c_send_ack(pHandlerI2C);
	while(numberOfBytes > 0){
		/* Cuando numberOfBytes es igual a 1, significa que esl ultimo y hay que parar */
		if (numberOfBytes == 1){
			/* 6. Generamos la condicion de NoAck, para quel Master no responda y el slave solo envie 1 byte */
			i2c_send_no_ack(pHandlerI2C);

			/* 7. Generamos la condicion Stop, para que el slave se detenga despues de 1 byte */
			i2c_send_close_comm(pHandlerI2C);

			*bufferRxData = i2c_read_byte(pHandlerI2C);

		}else {
			/* Si no es el ultimo, entonces captura el valor, incrementa el puntero y reduce en 1 */
			*bufferRxData = i2c_read_byte(pHandlerI2C);
			bufferRxData++;
		}
		/* Reducimos en 1 el conteo de los bytes a recibir */
		numberOfBytes--;
	}
	return numberOfBytes;

}

void i2c_WriteSingleRegisters(I2C_Handler_t *pHandlerI2C, uint8_t regToWrite, uint8_t newValue){

	/* 1. Generamos la condicion Start */
	i2c_start_signal(pHandlerI2C);

	/* 2. Enviamos la direccion del esclavo y la indicacion de ESCRIBIR */
	i2c_send_slave_address_rw(pHandlerI2C, eI2C_WRITE_DATA);

	/* 3. Enviamos la direccion de memoria que deseamos leer */
	i2c_send_memory_address(pHandlerI2C, regToWrite);

	/* 4. Creamos una condicion re reStart */
	i2c_send_byte(pHandlerI2C, newValue);

	/* 5. Enviamos la direccion del esclavo y la indicacion de LEER */
	i2c_send_close_comm(pHandlerI2C);
}

/**/
void i2c_WriteManyRegisters(I2C_Handler_t *pHandlerI2C, uint8_t regToWrite, uint8_t *bufferRxData, uint8_t numberOfBytes){

	/* 1. Generamos la condicion Start */
	i2c_start_signal(pHandlerI2C);

	/* 2. Enviamos la direccion del esclavo y la indicacion de ESCRIBIR */
	i2c_send_slave_address_rw(pHandlerI2C, eI2C_WRITE_DATA);

	/* 3. Enviamos la direccion de memoria que deseamos leer */
	i2c_send_memory_address(pHandlerI2C, regToWrite);

	while (numberOfBytes > 0){
		/* 4. Enviamos el valor que deseamos escribir en el registro seleccionado */
		i2c_send_byte(pHandlerI2C, *bufferRxData);
		bufferRxData++;
		numberOfBytes--;
	}

	/* 5. Generamos la condicion Stop, para que el slave se detenga despues de 1 byte */
	i2c_send_close_comm(pHandlerI2C);
}



void i2c_StartTransaction(I2C_Handler_t *ptrHandlerI2C){

	/* Solución a aparente problema al enciar al dirección del esclavo */
	ptrHandlerI2C->pI2Cx->CR1 &= ~I2C_CR1_STOP;

	/* 1. Verificamos que la línea no está ocupada - bit "Busy" del reg CR2 */
	while(ptrHandlerI2C->pI2Cx->SR2 & I2C_SR2_BUSY){	// El ciclo se mantiene hasta que esté desocupada
		__NOP();
	}

	/* 2. Generamos la señal "start" */
	ptrHandlerI2C->pI2Cx->CR1 |= I2C_CR1_START;

	/* 2a. Esperamos a que la bandera del evento "start" se levante.
	 * Mientras esperamos, el valor de SB es 0, entonces la negación (!) es 1.
	 */
	while(!(ptrHandlerI2C->pI2Cx->SR1 & I2C_SR1_SB)){
		__NOP();
	}

}


	/*
	 * Función para enviar la dirección del Slave con el que nos queremos
	 * comunicar, y se indica si queremos Leer o Escribir en el dispositivo
	 * con el que nos comunicamos a través del I2C.
	 */
	void i2c_SendSlaveAddressRW(I2C_Handler_t *ptrHandlerI2C, uint8_t slaveAddress, uint8_t readOrWrite){

		/* 0. Definimos una variable auxiliar para leer los
		 * registros para la secuencia de la bandera del ADDR
		 */
		uint8_t auxByte = 0;
		(void) auxByte;

		/* 1. Enviamos la dirección del Slave. En el bit menos significativo ponemos
		 * el valor Lectura (1) o Escritura (0)
		 */
		ptrHandlerI2C->pI2Cx->DR = ((slaveAddress << 1) | readOrWrite);

		/* 1a. Esperamos hasta que la bandera del evento ADDR se levante
		 * (esto nos indica que la dirección fue enviada satisfactoriamente,
		 * junto con el bit de Lectura o Escritura)
		 */
		while(!(ptrHandlerI2C->pI2Cx->SR1 & I2C_SR1_ADDR)){
			__NOP();
		}

		/* 2. Debemos limpiar la bandera de la recepción de ACK del ADDR, para
		 * lo cual debemos leer en secuencia el I2C_SR1 y luego el I2C_SR2
		 */
		auxByte = ptrHandlerI2C->pI2Cx->SR1;
		auxByte = ptrHandlerI2C->pI2Cx->SR2;

	} // Fin del i2c_SendSlaveAddressRW



	/*
	 * Función para enviar la dirección de memoria
	 */
	void i2c_SendMemoryAddress(I2C_Handler_t *ptrHandlerI2C, uint8_t memAddress){
		/* Enviamos la dirección de memoria que deseamos leer */
		ptrHandlerI2C->pI2Cx->DR = memAddress;

		/* Esperamos hasta que el byte sea transmitido */
		while(!(ptrHandlerI2C->pI2Cx->SR1 & I2C_SR1_TXE)){
			__NOP();
		}
	}



	/*
	 * Función para enviar un solo Byte al dispositivo Slave
	 */
	void i2c_SendDataByte(I2C_Handler_t *ptrHandlerI2C, uint8_t dataToWrite){
		/* Cargamos el valor que deseamos escribir */
		ptrHandlerI2C->pI2Cx->DR = dataToWrite;

		/* Esperamos hasta que el byte sea transmitido */
		while(!(ptrHandlerI2C->pI2Cx->SR1 & I2C_SR1_BTF)){
			__NOP();
		}
	}



	/*
	 * Función para la parada de la transmisión por I2C
	 */
	void i2c_StopTransaction(I2C_Handler_t *ptrHandlerI2C){
		ptrHandlerI2C->pI2Cx->CR1 |= I2C_CR1_STOP;
	}

