
#include <gpio.h>
#include <hardware.h>

// GPIO output register (Access: R/W/SC/WTC)
#define GPIO_OUT_REG			(GPIOMTX_BASE + 0x0004)
// GPIO output set register (Access: WT)
#define GPIO_OUT_W1TS_REG		(GPIOMTX_BASE + 0x0008)
// GPIO output clear register (Access: WT)
#define GPIO_OUT_W1TC_REG		(GPIOMTX_BASE + 0x000C)
// GPIO output enable register (Access: R/W/WTC)
#define GPIO_ENABLE_REG			(GPIOMTX_BASE + 0x0020)
// GPIO output enable set register (Access: WT)
#define GPIO_ENABLE_W1TS_REG	(GPIOMTX_BASE + 0x0024)
// GPIO output enable clear register (Access: WT)
#define GPIO_ENABLE_W1TC_REG	(GPIOMTX_BASE + 0x0028)
// Strapping pin register (Access: RO)
#define GPIO_STRAP_REG			(GPIOMTX_BASE + 0x0038)
// GPIO input register (Access: RO)
#define GPIO_IN_REG				(GPIOMTX_BASE + 0x003C)
// GPIO interrupt status register (Access: R/W/WTC)
#define GPIO_STATUS_REG			(GPIOMTX_BASE + 0x0044)
// GPIO interrupt status set register (Access: WT)
#define GPIO_STATUS_W1TS_REG	(GPIOMTX_BASE + 0x0048)
// GPIO interrupt status clear register (Access: WT)
#define GPIO_STATUS_W1TC_REG	(GPIOMTX_BASE + 0x004C)
// GPIO CPU interrupt status register (Access: RO)
#define GPIO_PCPU_INT_REG		(GPIOMTX_BASE + 0x005C)
// GPIO CPU non-maskable interrupt status register (Access: RO)
#define GPIO_PCPU_NMI_INT_REG	(GPIOMTX_BASE + 0x0060)
// GPIO interrupt source register (Access: RO)
#define GPIO_STATUS_NEXT_REG	(GPIOMTX_BASE + 0x014C)

void io_mode(badge_err_t *err, int pin, io_mode_t mode) {
	// Check pin bounds.
	if (pin < 0 || pin > io_count()) {
		if (err) err->cause = ECAUSE_RANGE;
		if (err) err->location = ELOC_GPIO;
		return;
	}
	
	// Check pin capabilities.
	if (mode != IO_MODE_INPUT && mode != IO_MODE_OUTPUT) {
		if (err) err->cause = ECAUSE_UNSUPPORTED;
		if (err) err->location = ELOC_GPIO;
		return;
	}
	
	// Set output enable bit.
	uint32_t out = READ_REG(GPIO_OUT_REG);
	if (mode == IO_MODE_INPUT) {
		out &= ~(1 << pin);
	} else {
		out |= 1 << pin;
	}
	WRITE_REG(GPIO_OUT_REG, out);
	
	// Clear error status.
	if (err) err->cause = 0;
}

void io_pull(badge_err_t *err, int pin, io_pull_t pull) {
	
}

void io_write(badge_err_t *err, int pin, bool level) {
	
}

bool io_read(badge_err_t *err, int pin) {
	
}
