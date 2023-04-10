PORT ?= /dev/ttyUSB0
BUILDDIR ?= build
IDF_PATH ?= $(shell pwd)/esp-idf
IDF_EXPORT_QUIET ?= 0
SHELL := /usr/bin/env bash

.PHONY: prepare clean build flash monitor menuconfig

all: flash

prepare:
	git submodule update --init --recursive --depth 1
	sudo apt install g++-riscv64-linux-gnu gcc-riscv64-linux-gnu
	cd esp-idf; bash install.sh

clean:
	rm -rf "$(BUILDDIR)"

build:
	$(MAKE) -C lib/libcstub
	$(MAKE) -C lib/test1
	$(MAKE) -C app/test2
	$(MAKE) -C app/test3
	source "$(IDF_PATH)/export.sh" && idf.py build

flash: build
	source "$(IDF_PATH)/export.sh" && idf.py flash

monitor:
	source "$(IDF_PATH)/export.sh" && idf.py monitor -p $(PORT)

menuconfig:
	source "$(IDF_PATH)/export.sh" && idf.py menuconfig
