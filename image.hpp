#ifndef _test_image
#define _test_image

#include <functional>

#include "color.h"

class Image {
protected:
	bool validRange(int x, int y) {
		return (x >= 0 && y >= 0) && (x < width && y < height);
	}

	bool invalidRange(int x, int y) {
		return !validRange(x, y);
	}

public:
	int flattenIndex(int x, int y) {
		return x + (y * height);
	}

protected:
	unsigned int width;
	unsigned int height;

public:
	unsigned int getWidth() {
		return width;
	}

	unsigned int getHeight() {
		return height;
	}

public:
	virtual HSVColor* get(int x, int y) { return NULL; }

	virtual void set(int x, int y, HSVColor& pixel) {}

	virtual void set(int x, int y, RGBColor& pixel) {}

public:
	virtual void forEachPixel(std::function<void(int, int, HSVColor*)> consumer) {
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				consumer(x, y, get(x, y));
			}
		}
	}

	virtual void forEachPixel(std::function<void(int, int, HSVColor&)> consumer) {
		forEachPixel([&consumer](int x, int y, HSVColor* pixel) {
			consumer(x, y, *pixel);
		});
	}

	virtual void shiftHue(int shift) {}
};

#endif