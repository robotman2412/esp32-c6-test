PORT ?= /dev/ttyUSB0
BUILDDIR ?= build
IDF_PATH ?= $(shell pwd)/esp-idf
IDF_EXPORT_QUIET ?= 0
SHELL := /usr/bin/env bash

.PHONY: prepare clean app-clean build app-build flash monitor menuconfig

all: flash

prepare:
	git submodule update --init --recursive --depth 1
	sudo apt install g++-riscv64-linux-gnu gcc-riscv64-linux-gnu
	cd esp-idf; bash install.sh

clean: app-clean
	rm -rf "$(BUILDDIR)"

build: app-build
	source "$(IDF_PATH)/export.sh" && idf.py build

app-clean:
	$(MAKE) -C lib/libcstub clean
	$(MAKE) -C lib/libbadge clean
	$(MAKE) -C lib/badgert clean
	$(MAKE) -C app/test5 clean

app-build:
	$(MAKE) -C lib/libcstub
	$(MAKE) -C lib/libbadge
	$(MAKE) -C lib/badgert
	$(MAKE) -C app/test5

flash: build
	source "$(IDF_PATH)/export.sh" && idf.py flash

monitor:
	source "$(IDF_PATH)/export.sh" && idf.py monitor -p $(PORT)

menuconfig:
	source "$(IDF_PATH)/export.sh" && idf.py menuconfig
