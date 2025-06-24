directory := aarch64-linux-android
toolchain := /opt/android-sdk/ndk/28.0.13004108/build/cmake/android.toolchain.cmake

build-Android:
	cmake -S .. -B ../build/$(directory) \
		-DCMAKE_BUILD_TYPE=Release \
		-DANDROID_ABI=arm64-v8a \
		-GNinja \
		-DCMAKE_TOOLCHAIN_FILE=$(toolchain)
	cmake --build ../build/$(directory) -j 7
	cmake --install ../build/$(directory) --prefix ../dist/$(directory)


	mkdir -p app/src/main/jniLibs/arm64-v8a;
	
	cmake -S app/src/main/cpp -B ../build/Android/$(directory) \
		-DCMAKE_BUILD_TYPE=Release \
		-DANDROID_ABI=arm64-v8a \
		-GNinja \
		-DDIST_DIR=$(PWD)/../dist/$(directory) \
		-DCMAKE_TOOLCHAIN_FILE=$(toolchain)
	cmake --build ../build/Android/$(directory) -j 7
	cmake --install ../build/Android/$(directory) --prefix ../dist/$(directory)

	find ../dist/$(directory) -name "*.so"|while read item; do cp $${item} app/src/main/jniLibs/arm64-v8a; done 


javah:
	javac -h  app/src/main/cpp/  app/src/main/java/com/example/mesen/demo/MesenAPI.java 