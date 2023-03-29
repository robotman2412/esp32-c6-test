PORT ?= /dev/ttyUSB0
BUILDDIR ?= build
IDF_PATH ?= $(shell pwd)/esp-idf
IDF_EXPORT_QUIET ?= 0
SHELL := /usr/bin/env bash

.PHONY: prepare clean build flash monitor menuconfig

all: prepare build install

prepare:
	git submodule update --init --recursive --depth 1
	cd esp-idf; bash install.sh

clean:
	rm -rf "$(BUILDDIR)"

build:
	source "$(IDF_PATH)/export.sh" && idf.py build

flash: build
	source "$(IDF_PATH)/export.sh" && idf.py flash

monitor:
	source "$(IDF_PATH)/export.sh" && idf.py monitor -p $(PORT)

menuconfig:
	source "$(IDF_PATH)/export.sh" && idf.py menuconfig
