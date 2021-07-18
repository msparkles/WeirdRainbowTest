#include <stdio.h>

#include "imageLoader.hpp"
#include "test.hpp"

int main() {
	Test demo(readBMP("background.bmp"));

	if (demo.Construct())
		demo.Start();

	return 0;
}