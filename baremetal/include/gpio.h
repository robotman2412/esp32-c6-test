
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <badge_err.h>



typedef enum {
	// high-impedance; no digital or analog functions
	IO_MODE_HIGH_Z,
	// digital output pin; io_write
	IO_MODE_OUTPUT,
	// digital input pin; optionally with pull resistors; io_read
	IO_MODE_INPUT,
	// PWM-based output pin; io_pwm_write
	IO_MODE_PWM,
	// DAC-based output pin; io_dac_write
	IO_MODE_DAC,
	// ADC-based input pin; optionally with pull resistors; io_adc_read
	IO_MODE_ADC,
} io_mode_t;

typedef enum {
	// no internal pull resistors; floating
	IO_PULL_NONE,
	// internal pull-up resistor; disconnected pins will read VCC
	IO_PULL_UP,
	// internal pull-down resistor; disconnected pins will read 0v
	IO_PULL_DOWN,
} io_pull_t;



// Returns the amount of GPIO pins present.
// Cannot produce an error.
#define io_count() (31)
// Sets the mode of GPIO pin `pin` to `mode`.
// Produces an error if permission is denied, the pin is in use by I²C/SPI/etc. or if the pin is incapable.
void io_mode(badge_err_t *ec, int pin, io_mode_t mode);
// Sets the pull resistor behaviour of GPIO pin `pin` to `dir`.
// Produces an error if permission is denied, the pin is not in an input mode or if the the pin is incapable.
void io_pull(badge_err_t *ec, int pin, io_pull_t dir);
// Writes level to GPIO pin pin.
// Produces an error if permission is denied, the pin is not in output mode or if the pin is incapable.
void io_write(badge_err_t *ec, int pin, bool level);
// Reads logic level value from GPIO pin `pin`.
// Produces an error if permission is denied or if the pin is incapable.
// Returns false on error.
bool io_read(badge_err_t *ec, int pin);
