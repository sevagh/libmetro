EXTRA_CMAKE_FLAGS:=
UBSAN_FLAGS:=-DENABLE_UBSAN=ON
CLANG_TIDY_FLAGS:=-DENABLE_CLANG_TIDY=ON

all: help

clean:
	rm -rf build/
	rm -rf lib/
	rm -rf bin/

_pre:
	mkdir -p build
	(cd build; cmake .. -G Ninja $(EXTRA_CMAKE_FLAGS))

help:
	@printf "libmetro targets:\n"
	@printf "\tclean\n"
	@printf "\tbuild\n"
	@printf "\tbuild-ubsan\t(needs a clean)\n"
	@printf "\tbuild-clang-tidy\t(needs a clean)\n"
	@printf "\ttest\n"
	@printf "\tinstall\n"
	@printf "\tcpp-clean\n"
	@printf "\tclang-analyze\n"
	@printf "\tclang-format\n"

build: _pre
	ninja -C build

build-ubsan: EXTRA_CMAKE_FLAGS=$(UBSAN_FLAGS)
build-ubsan: clean
build-ubsan: build

build-clang-tidy: EXTRA_CMAKE_FLAGS=$(CLANG_TIDY_FLAGS)
build-clang-tidy: clean
build-clang-tidy: build

test: build
	ninja -C build test

install: build
	sudo ninja -C build install

cpp-clean: _pre
	ninja -C build cpp-clean

clang-analyze: clean
	mkdir -p build
	(cd build; scan-build cmake .. -G Ninja)
	scan-build ninja -C build

clang-format: _pre
	ninja -C build clang-format

.PHONY:
	clean _pre build build-ubsan build-clang-tidy test install clang-analyze cpp-clean clang-format