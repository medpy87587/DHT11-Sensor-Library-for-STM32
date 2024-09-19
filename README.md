# DHT11 Sensor Library for STM32

## Overview

This library provides a simple and efficient interface for interacting with the DHT11 temperature and humidity sensor using STM32 microcontrollers. It utilizes a timer for precise timing measurements and supports reading temperature and humidity data with minimal setup.

## Features

- **Easy Integration**: Simple functions to initialize the sensor and read data.
- **Timer-Based**: Uses a hardware timer for accurate timing of the DHT11 communication protocol.
- **GPIO Configuration**: Configures GPIO pins as input or output as required by the DHT11 protocol.
- **Data Validation**: Includes checks for valid response and data integrity.

## Installation

1. Clone the repository or download the library files.
2. Include `mk_dht11.h` in your STM32 project.
3. Link the `mk_dht11.c` file to your project.
4. Ensure the STM32 HAL library is included in your project.

## Usage

### Initialization

To initialize the DHT11 sensor, use the `init_dht11` function:

```c
dht11_t myDHT11;
init_dht11(&myDHT11, &htim2, GPIOA, GPIO_PIN_2);
```

## Reading Data
To read temperature and humidity data, call the readDHT11 function:

```c
if (readDHT11(&myDHT11)) {
    uint8_t temperature = myDHT11.temperature;
    uint8_t humidity = myDHT11.humidity;
    // Process the temperature and humidity values
} else {
    // Handle read error
}
```
## GPIO Modes
Set the GPIO mode for the DHT11 pin using set_dht11_gpio_mode:

```c 
set_dht11_gpio_mode(&myDHT11, OUTPUT); // Set pin as OUTPUT
set_dht11_gpio_mode(&myDHT11, INPUT);  // Set pin as INPUT
```
## Dependencies
STM32 HAL library

## Contributing
Contributions are welcome! Please open an issue or submit a pull request for any enhancements or bug fixes.

