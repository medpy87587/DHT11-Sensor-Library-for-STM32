/*
 * DHT11.H
 *
 *  Created on: Sep 19, 2024
 *      Author: medam
 */

#ifndef INC_DHT11_H_
#define INC_DHT11_H_

#include "stm32f4xx_hal.h"


typedef struct{
	GPIO_Typedef* gpio;
	uint16_t pin;
	TIM_HandleTypeDef* htim;
	uint8_t temperature;
	uint8_t humidity;
}DHT11_HandleTypeDef;

void DHT11_Init(DHT11_HandleTypeDef *dht, TIM_HandleTypeDef *htim, GPIO_TypeDef* port, uint16_t pin);
void DHT11_SetGPIO_Mode(DHT11_HandleTypeDef *dht, uint8_t mode);
uint8_t DHT11_Read(DHT11_HandleTypeDef *dht);


#endif /* INC_DHT11_H_ */
