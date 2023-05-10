/*
	MIT License

	Copyright (c) 2023 Julian Scheffers

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Possible operating modes for pins.
typedef enum {
	// Set pin to high-impedance.
	IO_MODE_HIGH_Z,
	// Use pin as digital input.
	IO_MODE_INPUT,
	// Use pin as digital output.
	IO_MODE_OUTPUT,
	
	// Use pin as analog-to-digital input.
	IO_MODE_ANALOG_IN,
	// Use pin as digital-to-analog output.
	IO_MODE_ANALOG_OUT,
	
	// Use pin as PWM digital output.
	IO_MODE_PWM_OUT,
	
	// Use pin as SPI clock.
	IO_MODE_SPI_CLOCK,
	// Use pin as SPI receive (MISO for hosts, MOSI for devices).
	IO_MODE_SPI_RECV,
	// Use pin as SPI send (MOSI for hosts, MISO for devices).
	IO_MODE_SPI_SEND,
	// Use pin as SPI chip select (SPI device only).
	IO_MODE_SPI_CS,
	
	// Use pin as I2C SDA.
	IO_MODE_I2C_SDA,
	// Use pin as I2C SCL.
	IO_MODE_I2C_SCL,
	
	// Use pin as UART send.
	IO_MODE_UART_TX,
	// Use pin as UART receive.
	IO_MODE_UART_RX,
} io_mode_t;


// Possible pin pull directions.
typedef enum {
	// No pull.
	IO_NO_PULL,
	// Pull up.
	IO_PULLUP,
	// Pull down.
	IO_PULLDOWN,
} io_pull_t;


// Possible pin change events.
typedef enum {
	// Send interrupt on rising edge.
	IO_INTR_RISING,
	// Send interrupt on falling edge.
	IO_INTR_FALLING,
	// Send interrupt on any change.
	IO_INTR_CHANGE,
} io_change_t;


// Pin interrupt handler function signature.
typedef void(*io_isr_t)(int pin, bool new_val, bool old_val, void *cookie);


// Pin capability: High impedance.
#define IO_CAP_HIGH_Z		(1 << IO_MODE_HIGH_Z)
// Pin capability: Digital input.
#define IO_CAP_INPUT		(1 << IO_MODE_INPUT)
// Pin capability: Digital output.
#define IO_CAP_OUTPUT		(1 << IO_MODE_OUTPUT)

// Pin capability: Analog-to-digital input.
#define IO_CAP_ANALOG_IN	(1 << IO_MODE_ANALOG_IN)
// Pin capability: Digital-to-analog output.
#define IO_CAP_ANALOG_OUT	(1 << IO_MODE_ANALOG_OUT)

// Pin capability: PWM digital output.
#define IO_CAP_PWM_OUT		(1 << IO_MODE_PWM_OUT)

// Pin capability: SPI clock.
#define IO_CAP_SPI_CLOCK	(1 << IO_MODE_SPI_CLOCK)
// Pin capability: SPI receive (MISO for hosts, MOSI for devices).
#define IO_CAP_SPI_RECV		(1 << IO_MODE_SPI_RECV)
// Pin capability: SPI send (MOSI for hosts, MISO for devices).
#define IO_CAP_SPI_SEND		(1 << IO_MODE_SPI_SEND)
// Pin capability: SPI chip select (SPI device only).
#define IO_CAP_SPI_CS		(1 << IO_MODE_SPI_CS)

// Pin capability: I2C SDA.
#define IO_CAP_I2C_DATA		(1 << IO_MODE_I2C_SDA)
// Pin capability: I2C SCL.
#define IO_CAP_I2C_CLOCK	(1 << IO_MODE_I2C_SCL)

// Use pin as UART send.
#define IO_CAP_UART_TX		(1 << IO_MODE_UART_TX)
// Use pin as UART receive.
#define IO_CAP_UART_RX		(1 << IO_MODE_UART_RX)


// I2C capability: I2C host.
#define I2C_CAP_HOST		(1 << 0)
// I2C capability: I2C device.
#define I2C_CAP_DEV		(1 << 1)
// I2C capability: 10-bit addressing.
#define I2C_CAP_10BIT	(1 << 2)


// Get the number of present GPIO pins.
int io_cap_pin_count();
// Get the capabilities of a GPIO pin.
// Returns 0 if the pin does not exist.
uint32_t io_cap_pin(int pin);

// Get the number of hardware I2C interfaces.
int io_cap_i2c_count();
// Get the capabilities of an I2C interface.
// Returns 0 if the interface does not exist.
uint32_t io_cap_i2c(int interface);
// Get the highest clock speed in Hertz of an I2C interface.
// Returns 0 if the interface does not exist.
uint32_t io_cap_i2c_speed(int interface);

// TODO: SPI capabilities.


// Read value from pin.
// If not set to IO_MODE_INPUT, the resulting value is UNSPECIFIED.
bool io_read(int pin);
// Write value to pin.
// If not set to IO_MODE_OUTPUT, the resulting behaviour is UNSPECIFIED.
void io_write(int pin, bool value);
// Set mode of pin.
// Returns whether the operation was successful.
bool io_mode(int pin, io_mode_t mode);
// Set pull direction of an input pin.
// If not set to IO_MODE_INPUT, the resulting value is UNSPECIFIED.
// Returns whether the operation was successful.
bool io_pull(int pin, io_pull_t dir);

// Attach a pin change handler to a digital input pin.
// The cookie value passed to the ISR will be NULL.
// If a handler already exists, it will be replaced.
// Returns whether the operation was successful.
bool io_attach_isr(int pin, io_change_t when, io_isr_t isr);
// Attach a pin change handler to a digital input pin.
// The cookie value passed to the ISR will be the same as COOKIE.
// If a handler already exists, it will be replaced.
// Returns whether the operation was successful.
bool io_attach_isr_cookie(int pin, io_change_t when, io_isr_t isr, void *cookie);
// Detach a pin change handler.
// Returns whether the operation was successful and there was a handler present.
bool io_detach_isr(int pin);


// Set up an I2C interface as I2C host.
// Returns whether the operation was successful.
bool i2c_host_init(int interface, int sda, int scl);

// Start an I2C transaction (I2C host only).
// This operation may be delayed until i2c_host_stop is called.
// Returns whether the operation was successful.
bool i2c_host_start(int interface);
// Stop an I2C transaction (I2C host only).
// Returns whether the operation was successful.
bool i2c_host_stop(int interface);

// Write an I2C address (either 7-bit or 10-bit; I2C host only).
// This operation may be delayed until i2c_host_stop is called.
// Returns whether the operation was successful and acknowledged.
bool i2c_host_write_addr(int interface, int device, bool read_bit);
// Write an I2C byte (I2C host only).
// This operation may be delayed until i2c_host_stop is called.
// Returns whether the operation was successful and acknowledged.
bool i2c_host_write_byte(int interface, uint8_t send_byte);
// Read an I2C byte (I2C host only) and acknowledge it.
// This operation may be delayed until i2c_host_stop is called.
// Returns whether the operation was successful.
bool i2c_host_read_byte(int interface, uint8_t *recv_byte);
// Write an I2C byte (I2C host only).
// This operation may be delayed until i2c_host_stop is called.
// Returns whether the operation was successful and acknowledged.
bool i2c_host_write_bytes(int interface, const uint8_t *send_buf, size_t send_len);
// Read an I2C byte (I2C host only) and acknowledge it.
// This operation may be delayed until i2c_host_stop is called.
// Returns whether the operation was successful.
bool i2c_host_read_bytes(int interface, uint8_t *recv_buf, size_t recv_len);

// Write to an I2C device at a certain address (I2C host only).
// Automatically chooses 7-bit or 10-bit I2C device addressing.
// Returns whether the operation was successful.
int  i2c_host_write_to(int interface, int device, const uint8_t *send_buf, size_t send_len);
// Read from an I2C device at a certain address (I2C host only).
// Automatically chooses 7-bit or 10-bit I2C device addressing.
// Returns whether the operation was successful.
bool i2c_host_read_from(int interface, int device, uint8_t *recv_buf, size_t recv_len);

// TODO: I2C device API.


// TODO: SPI API.

#ifdef __cplusplus
} // extern "C"
#endif
