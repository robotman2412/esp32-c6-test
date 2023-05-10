
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <stdio.h>
#include <esp_log.h>

#include <badgert.h>

extern const char elf4_start[] asm("_binary_main5_o_start");
extern const char elf4_end[] asm("_binary_main5_o_end");

extern "C" void app_main() {
	// esp_log_level_set("elfloader", ESP_LOG_DEBUG);
	// esp_log_level_set("badgert", ESP_LOG_DEBUG);
	// esp_log_level_set("badgeloader", ESP_LOG_DEBUG);
	// esp_log_level_set("badgeabi", ESP_LOG_DEBUG);
	
	// Load the ELF thingylizer.
	FILE *elf_fd = fmemopen((void*) elf4_start, elf4_end-elf4_start, "r");
	badgert_start_fd("main5.o", elf_fd);
}
