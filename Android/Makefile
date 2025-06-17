directory := aarch64-linux-android
toolchain := /opt/android-sdk/ndk/28.0.13004108/build/cmake/android.toolchain.cmake

build-Android:
	cmake -S .. -B ../build/$(directory) \
		-DCMAKE_BUILD_TYPE=Release \
		-DANDROID_ABI=arm64-v8a \
		-DCMAKE_TOOLCHAIN_FILE=$(toolchain)
	cmake --build ../build/$(directory) -j 7
	cmake --install ../build/$(directory) --prefix ../dist/$(directory)


