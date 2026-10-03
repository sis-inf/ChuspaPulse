.PHONY: all build clean test

all: build

build:
	@cmake -B build -S .
	@cmake --build build

clean:
	@rm -rf build

test: build
	@ctest --test-dir build --output-on-failure