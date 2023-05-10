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

#include "gpio.h"

// Get the number of present GPIO pins.
int io_cap_pin_count() {}
// Get the capabilities of a GPIO pin.
// Returns 0 if the pin does not exist.
uint32_t io_cap_pin(int pin) {}

// Get the number of hardware I2C interfaces.
int io_cap_i2c_count() {}
// Get the capabilities of an I2C interface.
// Returns 0 if the interface does not exist.
uint32_t io_cap_i2c(int interface) {}
// Get the highest clock speed in Hertz of an I2C interface.
// Returns 0 if the interface does not exist.
uint32_t io_cap_i2c_speed(int interface) {}

// TODO: SPI capabilities.


// Read value from pin.
// If not set to IO_MODE_INPUT, the resulting value is UNSPECIFIED.
bool io_read(int pin) {}
// Write value to pin.
// If not set to IO_MODE_OUTPUT, the resulting behaviour is UNSPECIFIED.
void io_write(int pin, bool value) {}
// Set mode of pin.
// Returns whether the operation was successful.
bool io_mode(int pin, io_mode_t mode) {}
// Set pull direction of an input pin.
// If not set to IO_MODE_INPUT, the resulting value is UNSPECIFIED.
// Returns whether the operation was successful.
bool io_pull(int pin, io_pull_t dir) {}

// Attach a pin change handler to a digital input pin.
// The cookie value passed to the ISR will be NULL.
// If a handler already exists, it will be replaced.
// Returns whether the operation was successful.
bool io_attach_isr(int pin, io_change_t when, io_isr_t isr) {}
// Attach a pin change handler to a digital input pin.
// The cookie value passed to the ISR will be the same as COOKIE.
// If a handler already exists, it will be replaced.
// Returns whether the operation was successful.
bool io_attach_isr_cookie(int pin, io_change_t when, io_isr_t isr, void *cookie) {}
// Detach a pin change handler.
// Returns whether the operation was successful and there was a handler present.
bool io_detach_isr(int pin) {}


// Set up an I2C interface as I2C host.
// Returns whether the operation was successful.
bool i2c_host_init(int interface, int sda, int scl) {}

// Start an I2C transaction (I2C host only).
// This operation may be delayed until i2c_host_stop is called.
// Returns whether the operation was successful.
bool i2c_host_start(int interface) {}
// Stop an I2C transaction (I2C host only).
// Returns whether the operation was successful.
bool i2c_host_stop(int interface) {}

// Write an I2C address (either 7-bit or 10-bit {} I2C host only).
// This operation may be delayed until i2c_host_stop is called.
// Returns whether the operation was successful and acknowledged.
bool i2c_host_write_addr(int interface, int device, bool read_bit) {}
// Write an I2C byte (I2C host only).
// This operation may be delayed until i2c_host_stop is called.
// Returns whether the operation was successful and acknowledged.
bool i2c_host_write_byte(int interface, uint8_t send_byte) {}
// Read an I2C byte (I2C host only) and acknowledge it.
// This operation may be delayed until i2c_host_stop is called.
// Returns whether the operation was successful.
bool i2c_host_read_byte(int interface, uint8_t *recv_byte) {}
// Write an I2C byte (I2C host only).
// This operation may be delayed until i2c_host_stop is called.
// Returns whether the operation was successful and acknowledged.
bool i2c_host_write_bytes(int interface, const uint8_t *send_buf, size_t send_len) {}
// Read an I2C byte (I2C host only) and acknowledge it.
// This operation may be delayed until i2c_host_stop is called.
// Returns whether the operation was successful.
bool i2c_host_read_bytes(int interface, uint8_t *recv_buf, size_t recv_len) {}

// Write to an I2C device at a certain address (I2C host only).
// Automatically chooses 7-bit or 10-bit I2C device addressing.
// Returns whether the operation was successful.
int  i2c_host_write_to(int interface, int device, const uint8_t *send_buf, size_t send_len) {}
// Read from an I2C device at a certain address (I2C host only).
// Automatically chooses 7-bit or 10-bit I2C device addressing.
// Returns whether the operation was successful.
bool i2c_host_read_from(int interface, int device, uint8_t *recv_buf, size_t recv_len) {}
