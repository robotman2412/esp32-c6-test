PORT ?= /dev/ttyUSB0
BUILDDIR ?= build
IDF_PATH ?= $(shell pwd)/esp-idf
IDF_EXPORT_QUIET ?= 0
SHELL := /usr/bin/env bash
BADGESDK_PATH ?= $(HOME)/.badgeteam/badgesdk

.PHONY: prepare checktools clean app-clean build app-build flash monitor menuconfig

all: flash

checktools:
	@riscv64-linux-gnu-g++ --version 1>/dev/null 2>/dev/null || echo "[31mPlease install g++-riscv64-linux-gnu[0m"
	@riscv64-linux-gnu-gcc --version 1>/dev/null 2>/dev/null || echo "[31mPlease install gcc-riscv64-linux-gnu[0m"
	@riscv64-linux-gnu-gcc --version 1>/dev/null 2>/dev/null && riscv64-linux-gnu-g++ --version 1>/dev/null 2>/dev/null

$(BADGESDK_PATH): checktools
	echo "[32mInstalling BadgeSDK...[0m"
	git clone https://github.com/robotman2412/badgesdk-src /tmp/badgesdk-src
	$(MAKE) -s -C /tmp/badgesdk-src
	rm -rf /tmp/badgesdk-src

prepare: checktools $(BADGESDK_PATH)
	git submodule update --init --recursive --depth 1
	cd esp-idf; bash install.sh

clean: app-clean
	rm -rf "$(BUILDDIR)"

build: app-build
	@source "$(IDF_PATH)/export.sh" >/dev/null && idf.py build

app-clean:
	@$(MAKE) -s -C lib/pax-graphics clean
	@$(MAKE) -s -C app/test6 clean

app-build:
	@$(MAKE) -s -C lib/pax-graphics
	@$(MAKE) -s -C app/test6

flash: build
	@source "$(IDF_PATH)/export.sh" >/dev/null && idf.py flash

monitor:
	@source "$(IDF_PATH)/export.sh" >/dev/null && idf.py monitor -p $(PORT)

menuconfig:
	@source "$(IDF_PATH)/export.sh" >/dev/null && idf.py menuconfig
