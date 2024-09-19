/*
 * DHT11.c
 *
 *  Created on: Sep 19, 2024
 *      Author: medam
 */
#include <DHT11.h>

/**
 * @brief Initialize DHT11 sensor structure
 * @param dht Pointer to the DHT11 handle structure
 * @param htim Pointer to the TIM handle
 * @param port GPIO port for DHT11
 * @param pin GPIO pin for DHT11
 */
void DHT11_Init(DHT11_HandleTypeDef *dht, TIM_HandleTypeDef *htim, GPIO_TypeDef* port, uint16_t pin) {
    dht->htim = htim;
    dht->port = port;
    dht->pin = pin;
}

void DHT11_SetGPIO_Mode(DHT11_HandleTypeDef *dht, uint8_t mode) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if (mode == OUTPUT) {
        GPIO_InitStruct.Pin = dht->pin;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(dht->port, &GPIO_InitStruct);
    } else if (mode == INPUT) {
        GPIO_InitStruct.Pin = dht->pin;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(dht->port, &GPIO_InitStruct);
    }
}




/**
 * @brief Read data from the DHT11 sensor
 * @param dht Pointer to the DHT11 handle structure
 * @return 1 if data read successfully, 0 otherwise
 */


/**
 * @brief Read DHT11 value
 * @param dht Struct for DHT11
 * @return 1 if read OK, 0 if there was an error
 */
uint8_t readDHT11(dht11_t *dht) {
    uint16_t mTime1 = 0, mTime2 = 0;
    uint8_t humVal = 0, tempVal = 0, parityVal = 0, genParity = 0;
    uint8_t mData[40];

    // Start communication
    set_dht11_gpio_mode(dht, OUTPUT); // Set pin direction as output
    HAL_GPIO_WritePin(dht->port, dht->pin, GPIO_PIN_RESET);
    HAL_Delay(18); // Wait 18 ms in low state
    __disable_irq(); // Disable all interrupts to ensure accurate timing
    HAL_TIM_Base_Start(dht->htim); // Start timer
    set_dht11_gpio_mode(dht, INPUT); // Set pin direction as input

    // Check DHT11 response
    __HAL_TIM_SET_COUNTER(dht->htim, 0); // Reset timer counter
    while (HAL_GPIO_ReadPin(dht->port, dht->pin) == GPIO_PIN_SET) {
        if ((uint16_t)__HAL_TIM_GET_COUNTER(dht->htim) > 500) {
            __enable_irq();
            return 0; // Timeout, invalid response
        }
    }
    __HAL_TIM_SET_COUNTER(dht->htim, 0);
    while (HAL_GPIO_ReadPin(dht->port, dht->pin) == GPIO_PIN_RESET) {
        if ((uint16_t)__HAL_TIM_GET_COUNTER(dht->htim) > 500) {
            __enable_irq();
            return 0; // Timeout, invalid response
        }
    }
    mTime1 = (uint16_t)__HAL_TIM_GET_COUNTER(dht->htim);
    __HAL_TIM_SET_COUNTER(dht->htim, 0);
    while (HAL_GPIO_ReadPin(dht->port, dht->pin) == GPIO_PIN_SET) {
        if ((uint16_t)__HAL_TIM_GET_COUNTER(dht->htim) > 500) {
            __enable_irq();
            return 0; // Timeout, invalid response
        }
    }
    mTime2 = (uint16_t)__HAL_TIM_GET_COUNTER(dht->htim);

    // Validate response pulse timings
    if ((mTime1 < 60 || mTime1 > 100) || (mTime2 < 60 || mTime2 > 100)) {
        __enable_irq();
        return 0; // Invalid response
    }

    // Read 40 bits of data
    for (int j = 0; j < 40; j++) {
        __HAL_TIM_SET_COUNTER(dht->htim, 0);
        while (HAL_GPIO_ReadPin(dht->port, dht->pin) == GPIO_PIN_RESET) {
            if ((uint16_t)__HAL_TIM_GET_COUNTER(dht->htim) > 500) {
                __enable_irq();
                return 0; // Timeout, invalid data
            }
        }
        __HAL_TIM_SET_COUNTER(dht->htim, 0);
        while (HAL_GPIO_ReadPin(dht->port, dht->pin) == GPIO_PIN_SET) {
            if ((uint16_t)__HAL_TIM_GET_COUNTER(dht->htim) > 500) {
                __enable_irq();
                return 0; // Timeout, invalid data
            }
        }
        mTime1 = (uint16_t)__HAL_TIM_GET_COUNTER(dht->htim);

        // Check bit duration and store data
        if (mTime1 > 20 && mTime1 < 30) {
            mData[j] = 0; // Low bit
        } else if (mTime1 > 60 && mTime1 < 80) {
            mData[j] = 1; // High bit
        } else {
            __enable_irq();
            return 0; // Invalid bit duration
        }
    }

    HAL_TIM_Base_Stop(dht->htim); // Stop timer
    __enable_irq(); // Enable all interrupts

    // Extract values from data buffer
    humVal = 0;
    for (int i = 0; i < 8; i++) {
        humVal = (humVal << 1) | mData[i];
    }

    tempVal = 0;
    for (int i = 16; i < 24; i++) {
        tempVal = (tempVal << 1) | mData[i];
    }

    parityVal = 0;
    for (int i = 32; i < 40; i++) {
        parityVal = (parityVal << 1) | mData[i];
    }

    // Verify checksum
    genParity = (humVal + tempVal) & 0xFF;
    if (genParity != parityVal) {
        return 0; // Checksum error
    }

    dht->temperature = tempVal;
    dht->humidity = humVal;

    return 1; // Read successful
}
