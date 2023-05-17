PORT ?= /dev/ttyUSB0
BUILDDIR ?= build
IDF_PATH ?= $(shell pwd)/esp-idf
IDF_EXPORT_QUIET ?= 0
SHELL := /usr/bin/env bash

.PHONY: prepare clean app-clean build app-build flash monitor menuconfig

all: flash

checktools:
	@riscv64-linux-gnu-g++ --version 1>/dev/null 2>/dev/null || echo "[31mPlease install g++-riscv64-linux-gnu[0m"
	@riscv64-linux-gnu-gcc --version 1>/dev/null 2>/dev/null || echo "[31mPlease install gcc-riscv64-linux-gnu[0m"
	@riscv64-linux-gnu-gcc --version 1>/dev/null 2>/dev/null && riscv64-linux-gnu-g++ --version 1>/dev/null 2>/dev/null

checksdk: $(HOME)/.badgeteam/badgesdk

$(HOME)/.badgeteam/badgesdk: checktools
	echo "[32mInstalling BadgeSDK...[0m"
	mkdir -p ~/.badgesdk
	git clone https://github.com/robotman2412/badgesdk-src /tmp/badgesdk-src
	$(MAKE) -s -C /tmp/badgesdk-src
	rm -rf /tmp/badgesdk-src

prepare: checktools checksdk
	git submodule update --init --recursive --depth 1
	@g++-riscv64-linux-gnu --version || echo -e \033[31mPlease install g++-riscv64-linux-gnu\033[0m
	@gcc-riscv64-linux-gnu --version || echo -e \033[31mPlease install gcc-riscv64-linux-gnu\033[0m
	cd esp-idf; bash install.sh

clean: app-clean
	rm -rf "$(BUILDDIR)"

build: app-build
	source "$(IDF_PATH)/export.sh" && idf.py build

app-clean:
	$(MAKE) -C lib/libcstub clean
	$(MAKE) -C lib/libbadge clean
	$(MAKE) -C lib/badgert clean
	$(MAKE) -C lib/pax-graphics clean
	$(MAKE) -C lib/test3 clean
	$(MAKE) -C app/test5 clean
	$(MAKE) -C app/test6 clean

app-build:
	$(MAKE) -C lib/libcstub
	$(MAKE) -C lib/libbadge
	$(MAKE) -C lib/badgert
	$(MAKE) -C lib/pax-graphics
	$(MAKE) -C lib/test3
	$(MAKE) -C app/test5
	$(MAKE) -C app/test6

flash: build
	source "$(IDF_PATH)/export.sh" && idf.py flash

monitor:
	source "$(IDF_PATH)/export.sh" && idf.py monitor -p $(PORT)

menuconfig:
	source "$(IDF_PATH)/export.sh" && idf.py menuconfig
