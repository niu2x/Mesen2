MAKEFILE_PATH := $(lastword $(MAKEFILE_LIST))
CUR_DIR := $(dir $(abspath $(MAKEFILE_PATH)))

# ifeq ($(strip $(triple)),)
#     $(error triple 变量为空, 请设置为大约这样的值 x86_64-PC-Linux)
# endif
CORES := $(shell nproc)
JOBS := $(shell expr $(CORES) - 1)

triple := x86_64-PC-Linux

build_type := Release

build-linux:
	cmake -S .. -B ../build-$(build_type)/$(triple)/MesenRT \
		-DCMAKE_BUILD_TYPE=$(build_type)
	cmake --build ../build-$(build_type)/$(triple)/MesenRT -j $(JOBS)
	cmake --install ../build-$(build_type)/$(triple)/MesenRT --prefix ../dist/$(triple)

	cmake -S . -DCMAKE_BUILD_TYPE=$(build_type) \
		-B ../build-$(build_type)/$(triple)/DesktopApp \
		-DDIST=$(CUR_DIR)/../dist/$(triple)
	cmake --build ../build-$(build_type)/$(triple)/DesktopApp -j $(JOBS)
	cmake --install ../build-$(build_type)/$(triple)/DesktopApp --prefix ../dist/$(triple)
