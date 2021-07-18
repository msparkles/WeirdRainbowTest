#define OLC_PGE_APPLICATION

#include "image.hpp"
#include "mask.h"
#include "olcPixelGameEngine.h"

class Test : public olc::PixelGameEngine {
public:
	Image* image;
	int width;
	int height;
	Mask<unsigned long>* mask;

	Test(Image* image) {
		this->image	   = image;
		this->width	   = image->getWidth();
		this->height   = image->getHeight();
		this->mask	   = new Mask<unsigned long>(width, height);
		this->sAppName = "test";

		this->x = width / 2;
		this->y = height / 2;

		this->pixelSize = std::max(1, 512 / width);
	}

	~Test() {
		delete this->mask;
		delete this->image;
	}

private:
	int x;
	int y;
	int speed  = 2;
	int radius = 5;

	int pixelSize;

private:
	void shiftHue() {
		while (1) {
			image->shiftHue(2);
			std::this_thread::sleep_for(std::chrono::milliseconds(30));
		}
	}

	void input() {
		while (1) {
			if (GetKey(olc::Key::LEFT).bHeld) {
				mask->shiftRight(speed);
			}
			if (GetKey(olc::Key::RIGHT).bHeld) {
				mask->shiftLeft(speed);
			}
			if (GetKey(olc::Key::UP).bHeld) {
				mask->shiftDown(speed);
			}
			if (GetKey(olc::Key::DOWN).bHeld) {
				mask->shiftUp(speed);
			}
			mask->on(x - radius, y - radius, x + radius, y + radius);
			std::this_thread::sleep_for(std::chrono::milliseconds(30));
		}
	}

public:
	olc::rcode Construct() {
		return PixelGameEngine::Construct(width, height, pixelSize, pixelSize);
	}

	bool OnUserCreate() override {
		std::thread(&Test::shiftHue, this).detach();
		std::thread(&Test::input, this).detach();

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override {
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				if (mask->isOn(x, y)) {
					auto pixel = image->get(x, y);
					auto rgb   = HSVtoRGB(*pixel);
					Draw(x, y, olc::Pixel(rgb.r, rgb.g, rgb.b));
				} else {
					Draw(x, y, olc::BLACK);
				}
			}
		}

		return true;
	}
};