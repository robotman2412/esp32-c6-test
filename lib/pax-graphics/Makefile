
.PHONY: all

all:
	@mkdir -p build
	@cd build && cmake ..
	@cd build && make -j$(shell nproc)
	riscv64-linux-gnu-gcc -shared \
		-march=rv32imac -mabi=ilp32 \
		-nostdlib -nodefaultlibs \
		-fPIE \
		-Wl,--whole-archive \
		-o build/libpax.so \
		build/libpax_graphics.a

clean:
	rm -rf build
