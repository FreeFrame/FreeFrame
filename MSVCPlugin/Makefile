# Makefile for Linux Open Video Plugins

all-am: FreeFrame.so


FreeFrame.so: FreeFrame.cpp
	g++ -shared -O3 -DLINUX -mmmx FreeFrame.cpp -o FreeFrame.so
clean:
	rm -f *.so
