MAKEFILE_PATH := $(lastword $(MAKEFILE_LIST))
CUR_DIR := $(dir $(abspath $(MAKEFILE_PATH)))

# ifeq ($(strip $(triple)),)
#     $(error triple 变量为空, 请设置为大约这样的值 x86_64-PC-Linux)
# endif

triple := x86_64-PC-Linux

build-linux:
	cmake -S .. -B ../build/$(triple)/MesenRT \
		-DCMAKE_BUILD_TYPE=Release
	cmake --build ../build/$(triple)/MesenRT -j 11
	cmake --install ../build/$(triple)/MesenRT --prefix ../dist/$(triple)

	cmake -S . -DCMAKE_BUILD_TYPE=Release \
		-B ../build/$(triple)/DesktopApp \
		-DDIST=$(CUR_DIR)/../dist/$(triple)
	cmake --build ../build/$(triple)/DesktopApp -j 11
	cmake --install ../build/$(triple)/DesktopApp --prefix ../dist/$(triple)
