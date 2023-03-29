
#pragma once

#define ELFLOADER_LINENUMBERS

#ifdef ELFLOADER_LINENUMBERS
#define LOG_LNFMT "%s:%d: "
#define LOG_LNARG , __FILE__, __LINE__
#else
#define LOG_LNFMT ""
#define LOG_LNARG
#endif

#ifdef ESP_PLATFORM
#include <esp_log.h>
static const char *TAG = "elfloader";
#define LOGE(fmt, ...) ESP_LOGE(TAG, LOG_LNFMT fmt LOG_LNARG __VA_OPT__(,) __VA_ARGS__)
#define LOGI(fmt, ...) ESP_LOGI(TAG, LOG_LNFMT fmt LOG_LNARG __VA_OPT__(,) __VA_ARGS__)
#define LOGW(fmt, ...) ESP_LOGW(TAG, LOG_LNFMT fmt LOG_LNARG __VA_OPT__(,) __VA_ARGS__)
#define LOGD(fmt, ...) ESP_LOGD(TAG, LOG_LNFMT fmt LOG_LNARG __VA_OPT__(,) __VA_ARGS__)
#else
static void _elf_log_helper(const char *prefix, const char *fmt, ...) {
	fputs(prefix, stdout);
	va_list ls;
	va_start(ls, fmt);
	vprintf(fmt, ls);
	va_end(ls);
	fputs("\033[0m", stdout);
}
#define LOGE(fmt, ...) _elf_log_helper("\033[31mErr  elfloader: ", LOG_LNFMT fmt LOG_LNARG __VA_OPT__(,) __VA_ARGS__)
#define LOGI(fmt, ...) _elf_log_helper( "\033[0mInfo elfloader: ", LOG_LNFMT fmt LOG_LNARG __VA_OPT__(,) __VA_ARGS__)
#define LOGW(fmt, ...) _elf_log_helper("\033[33mWarn elfloader: ", LOG_LNFMT fmt LOG_LNARG __VA_OPT__(,) __VA_ARGS__)
#define LOGD(fmt, ...) _elf_log_helper("\033[34mDeb  elfloader: ", LOG_LNFMT fmt LOG_LNARG __VA_OPT__(,) __VA_ARGS__)
#endif
