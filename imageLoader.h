#ifndef _test_image_loader
#define _test_image_loader

#include <stdlib.h>

#include <fstream>
#include <iostream>

#include "color.h"
#include "image.cpp"
#include "olcPixelGameEngine.h"

Image* readBMP(const char* filename) {
	std::ifstream file(filename);

	if (!file) return NULL;

	unsigned char* pixels;  // Pixels

	BITMAPFILEHEADER* bmpHeader = new BITMAPFILEHEADER;	 // Header
	BITMAPINFOHEADER* bmpInfo	= new BITMAPINFOHEADER;	 // Info

	file.read((char*)bmpHeader, sizeof(BITMAPFILEHEADER));
	file.read((char*)bmpInfo, sizeof(BITMAPINFOHEADER));

	if (bmpHeader->bfType != 0x4D42) {
		std::cout << "File " << filename << " isn't a bitmap file\n";
		return NULL;
	}

	int width  = bmpInfo->biWidth;
	int height = bmpInfo->biHeight;
	int size   = width * height;

	unsigned long rawSize  = bmpInfo->biSizeImage;
	unsigned char pixelSize = rawSize / size;
	unsigned char* rawData  = new unsigned char[rawSize];

	file.seekg(bmpHeader->bfOffBits);
	file.read((char*)rawData, rawSize);

	delete bmpHeader;
	delete bmpInfo;

	auto image = new ImageImpl(width, height);

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			// the axis is flipped
			int realY = (height - 1) - y;

			int p = image->flattenIndex(x, y) * pixelSize;

			unsigned char rawPixel[pixelSize];
			for (int a = 0; a < pixelSize; a++) {
				rawPixel[a] = rawData[p + a];
			}
			unsigned short r = rawPixel[2];
			unsigned short g = rawPixel[1];
			unsigned short b = rawPixel[0];
			
			if (pixelSize >= 4) {
				unsigned char a = rawPixel[3];
				r = (r * a) / 255;
				g = (g * a) / 255;
				b = (b * a) / 255;
			}

			auto pixel = RGBColor(r, g, b);

			image->set(x, realY, pixel);
		}
	}

	delete rawData;

	return image;
}

#endif