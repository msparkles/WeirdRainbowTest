#ifndef _test_impl_image
#define _test_impl_image

#include "image.hpp"

class ImageImpl : public Image {
private:
	unsigned int size;

	HSVColor** data;

public:
	int getSize() {
		return size;
	}

	ImageImpl(int width, int height) {
		this->width	 = abs(width);
		this->height = abs(height);

		this->size = getWidth() * getHeight();
		this->data = new HSVColor* [getSize()] { NULL };
	}

	~ImageImpl() {
		forEachPixel([](int x, int y, HSVColor* pixel) { delete pixel; });
		delete this->data;
	}

public:
	HSVColor* get(int x, int y) override {
		if (invalidRange(x, y)) return NULL;

		return this->data[flattenIndex(x, y)];
	}

	void set(int x, int y, HSVColor& pixel) override {
		if (invalidRange(x, y)) return;

		auto old = get(x, y);

		if (old) {
			delete old;
		}

		this->data[flattenIndex(x, y)] = new HSVColor(pixel);
	}

	void set(int x, int y, RGBColor& pixel) override {
		if (invalidRange(x, y)) return;

		auto hsv = RGBtoHSV(pixel);

		return set(x, y, hsv);
	}

public:
	void shiftHue(int shift) override {
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				get(x, y)->h += shift;
			}
		}
	}
};

#endif