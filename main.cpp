#include <stdio.h>

#include "imageLoader.h"
#include "test.h"

int main() {
	Test demo(readBMP("background.bmp"));

	if (demo.Construct())
		demo.Start();

	return 0;
}