
IDF_PATH ?= $(shell pwd)/../esp-idf
SHELL := /usr/bin/env bash

.PHONY: all clean build flash

all: build

build:
	@mkdir -p build && cmake -B build
	@make -s -C build

clean:
	rm -rf build

flash:
	@source "$(IDF_PATH)/export.sh" >/dev/null && esptool.py -b 921600 \
		write_flash --flash_mode dio --flash_freq 80m --flash_size 2MB \
		0x0 bin/bootloader.bin \
		0x10000 main.bin \
		0x8000 bin/partition-table.bin
