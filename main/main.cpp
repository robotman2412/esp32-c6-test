
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <stdio.h>
#include <esp_log.h>

#include <badgert.h>

extern const char elf_start[] asm("_binary_main6_o_start");
extern const char elf_end[] asm("_binary_main6_o_end");

extern const char elflib_start[] asm("_binary_libtest3_so_start");
extern const char elflib_end[] asm("_binary_libtest3_so_end");

extern "C" void app_main() {
	// esp_log_level_set("elfloader", ESP_LOG_DEBUG);
	// esp_log_level_set("badgert", ESP_LOG_DEBUG);
	// esp_log_level_set("badgeloader", ESP_LOG_DEBUG);
	// esp_log_level_set("badgeabi", ESP_LOG_DEBUG);
	
	// Register LIBRARY.
	badgert_register_buf("libtest3.so", (void*) elflib_start, elflib_end-elflib_start);
	
	// Load the ELF thingylizer.
	FILE *elf_fd = fmemopen((void*) elf_start, elf_end-elf_start, "r");
	badgert_start_fd("main6.o", elf_fd);
}
