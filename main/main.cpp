
#include <stdio.h>
#include <string.h>

#include <esp_log.h>
static const char *TAG = "main";

#include <badgert.h>
#include <badgeabi.h>

#include <managed_i2c.h>
#include <driver_ssd1306.h>

#include <mpu.hpp>
#include <kernel.hpp>

// extern const char elf_start[] asm("_binary_main7_o_start");
// extern const char elf_end[] asm("_binary_main7_o_end");

extern const char elf_start[] asm("_binary_main6_o_start");
extern const char elf_end[] asm("_binary_main6_o_end");

extern const char elflib_start[] asm("_binary_libpax_so_start");
extern const char elflib_end[] asm("_binary_libpax_so_end");

uint8_t framebuffer[128*64/8];

bool flush_my_disp(const void *buf, size_t buf_len, int x, int y, int width, int height, void *cookie) {
	// if (x == 0 && y == 0 && width == 128 && height == 64) {
		driver_ssd1306_write((const uint8_t *) buf);
	// } else {
	// 	return !driver_ssd1306_write_part((const uint8_t *) buf, x, y, x+width-1, y+height-1);
	// }
	return false;
}

extern "C" void app_main() {
	// mpu::appendRegion({
	// 	0, 0x100000000,
	// 	0,
	// 	1,1,1,
	// 	1
	// });
	// kernel::init();
	
	// esp_log_level_set("elfloader", ESP_LOG_DEBUG);
	// esp_log_level_set("badgert", ESP_LOG_DEBUG);
	// esp_log_level_set("badgeloader", ESP_LOG_DEBUG);
	// esp_log_level_set("badgeabi", ESP_LOG_DEBUG);
	
	// Start up display.
	esp_err_t res = i2c_init(0, 5, 4, 800000, 1, 1);
	if (res) {
		ESP_LOGE(TAG, "I2C init failed: %s", esp_err_to_name(res));
		return;
	}
	driver_ssd1306_init();
	driver_ssd1306_write(framebuffer);
	
	// Register display.
	display_add(flush_my_disp, nullptr, 128, 64);
	
	// Register LIBRARY.
	badgert_register_buf("libpax.so", (void*) elflib_start, elflib_end-elflib_start);
	
	// Load the ELF thingylizer.
	FILE *elf_fd = fmemopen((void*) elf_start, elf_end-elf_start, "r");
	badgert_start_fd("main6.o", elf_fd);
}
