/*
 * oled.h
 *
 *  Created on: 2/10/2024
 *      Author: lbarreras
 */

#ifndef OLED_H_
#define OLED_H_

#define OLED_ADRESS 0x3C // direccion del esclavo
#define OLED_CONTROLBYTE_CONFIG  0x00 // configurara la oled, comando de configuracion d ela oled, tamaño,etc,
#define OLED_CONTROLBYTE_DISPLAY 0x40 // comando para escribir

//#define COL_1 0x00  // Columna 1
//#define COL_2 0x20  // Columna 2
//#define COL_3 0x40  // Columna 3
//#define COL_4 0x60  // Columna 4


void sendDataBytes(I2C_Handler_t *ptrHandlerI2C, char *dataToSent, uint8_t sizeArray);
void sendCommandByte ( I2C_Handler_t *ptrHandlerI2C, char command);

void startOLED(I2C_Handler_t *ptrHandlerI2Ctr);
void toggleDisplay(I2C_Handler_t *ptrHandlerI2Ctr);
void clearDisplay (I2C_Handler_t *ptrHandlerI2Ctr);
void stopOLED (I2C_Handler_t *ptrHandlerI2Ctr);
void setPage (I2C_Handler_t *ptrHandlerI2Ctr, uint8_t page);
void setColumnAddress (I2C_Handler_t *ptrHandlerI2Ctr, uint8_t page);
char* letterTochar (uint8_t character);
void drawMSG(I2C_Handler_t *ptrHandlerI2Ctr, char *msg);
void drawSinglePageMSG (I2C_Handler_t *ptrHandlerI2Ctr, char *msg, uint8_t page);

#endif /* OLED_H_ */
