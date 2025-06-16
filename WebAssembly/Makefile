directory := wasm32-unknown-unknown
toolchain := /opt/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake

build-WebAssembly:
	cmake -S .. -B ../build/$(directory) \
		-DCMAKE_BUILD_TYPE=Release \
		-DCMAKE_TOOLCHAIN_FILE=$(toolchain)
	cmake --build ../build/$(directory) -j 7
	cmake --install ../build/$(directory) --prefix ../dist/$(directory)

	cmake -S . -B ../build/WebAssembly \
		-DCMAKE_BUILD_TYPE=Release \
		-DCMAKE_TOOLCHAIN_FILE=$(toolchain) \
		-DDIST=$(PWD)/../dist/$(directory)
	cmake --build ../build/WebAssembly

	npx webpack

	cp src/index.html dist/
	cp ../build/WebAssembly/WebNative.js dist/
	cp ../build/WebAssembly/WebNative.worker.js dist/
	cp ../build/WebAssembly/WebNative.wasm dist/