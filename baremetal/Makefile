
IDF_PATH ?= $(shell pwd)/../esp-idf
SHELL := /usr/bin/env bash

.PHONY: all tools clean-tools clean build flash

all: tools build

tools:
	@mkdir -p tools
	@make -s -C elftool
	@cp elftool/build/elftool tools/elftool

clean-tools:
	rm -rf tools
	@make -s -C elftool clean

build:
	@mkdir -p build && cmake -B build
	@make -s -C build
	@./packimage.py

clean:
	rm -rf build

flash:
	@source "$(IDF_PATH)/export.sh" >/dev/null && esptool.py -b 921600 \
		write_flash --flash_mode dio --flash_freq 80m --flash_size 2MB \
		0x0 bin/bootloader.bin \
		0x10000 build/main.bin \
		0x8000 bin/partition-table.bin
