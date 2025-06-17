directory := wasm32-unknown-unknown
toolchain := /opt/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake

build-WebAssembly:
	cmake -S .. -B ../build/$(directory) \
		-DCMAKE_BUILD_TYPE=Release \
		-GNinja \
		-DCMAKE_TOOLCHAIN_FILE=$(toolchain)
	cmake --build ../build/$(directory) -j 2
	cmake --install ../build/$(directory) --prefix ../dist/$(directory)

	cmake -S . -B ../build/WebAssembly \
		-DCMAKE_BUILD_TYPE=Release \
		-GNinja \
		-DCMAKE_TOOLCHAIN_FILE=$(toolchain) \
		-DDIST=$(PWD)/../dist/$(directory)
	cmake --build ../build/WebAssembly


	cp ../build/WebAssembly/WebNative.js src/
	cp ../build/WebAssembly/WebNative.worker.js src/
	cp ../build/WebAssembly/WebNative.wasm src/
	mkdir -p dist
	rm -rf dist/*.wasm
	rm -rf dist/*.js
	rm -rf dist/*.txt
	npx webpack
