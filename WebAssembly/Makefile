directory := wasm32-unknown-unknown
toolchain := /opt/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake

CORES := $(shell nproc)
JOBS := $(shell expr $(CORES) - 1)

build-WebAssembly:
	cmake -S .. -B ../build/$(directory) \
		-DCMAKE_BUILD_TYPE=Release \
		-GNinja \
		-DCMAKE_TOOLCHAIN_FILE=$(toolchain)
	cmake --build ../build/$(directory) -j $(JOBS)
	cmake --install ../build/$(directory) --prefix ../dist/$(directory)

	cmake -S . -B ../build/WebAssembly \
		-DCMAKE_BUILD_TYPE=Release \
		-GNinja \
		-DCMAKE_TOOLCHAIN_FILE=$(toolchain) \
		-DDIST=$(PWD)/../dist/$(directory)
	cmake --build ../build/WebAssembly -j $(JOBS)


	cp ../build/WebAssembly/WebNative.js src/
	cp ../build/WebAssembly/WebNative.worker.js src/
	cp ../build/WebAssembly/WebNative.wasm src/
	rm -rf dist/*
	npx webpack
