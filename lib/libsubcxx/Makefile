
.PHONY: all

all:
	@mkdir -p build
	@cd build && cmake ..
	@cd build && make -j$(shell nproc)

clean:
	rm -rf build
