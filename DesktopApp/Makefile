build:
	cmake -S .. -B ../build/MesenRT \
		-GNinja \
		-DCMAKE_BUILD_TYPE=Release
	cmake --build ../build/MesenRT -j 2
	cmake --install ../build/MesenRT --prefix ../dist

	cmake -S . -DCMAKE_BUILD_TYPE=Release \
		-B ../build/DesktopApp \
		-GNinja \
		-DDIST=$(PWD)/../dist
	cmake --build ../build/DesktopApp -j 2
	cmake --install ../build/DesktopApp --prefix ../dist
	cp NotoSansSC-Regular.ttf ../dist/bin/
