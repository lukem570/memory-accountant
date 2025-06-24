add: build
	g++ test.cpp -o build/main -Iinclude
	./build/main

build:
	mkdir build