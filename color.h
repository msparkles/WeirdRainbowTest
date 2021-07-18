#ifndef _test_color
#define _test_color

unsigned char calcRegion(short int h) {
	if (h > 300) return 5;
	if (h > 240) return 4;
	if (h > 180) return 3;
	if (h > 120) return 2;
	if (h > 60) return 1;
	return 0;
}

class RGBColor {
public:
	unsigned char r;
	unsigned char g;
	unsigned char b;

	RGBColor() {}

	RGBColor(unsigned char r, unsigned char g, unsigned char b) {
		this->r = r;
		this->g = g;
		this->b = b;
	}
};

class HSVColor {
public:
	short int h;
	unsigned char s;
	unsigned char v;

	HSVColor() {}

	HSVColor(short int h, unsigned char s, unsigned char v) {
		this->h = h;
		this->s = s;
		this->v = v;
	}
};

unsigned int sqr(int n) {
	return n * n;
}

int mod(int a, int b) {
	if (b < 0)
		return -mod(-a, -b);

	int ret = a % b;
	if (ret < 0)
		ret += b;
	return ret;
}

short int roundShort(short int n) {
	if (n == 0) return 0;

	unsigned char d = abs(n % 10);
	if (n > 0) {
		return (n / 10) + ((d >= 5) ? 1 : 0);
	} else {
		return (n / 10) - ((d >= 5) ? 1 : 0);
	}
}

short int preciseHue(short int n, short int delta, short int offset) {
	return roundShort(10 * offset + ((10 * 60 * n) / delta));
}

unsigned int squareDistance(RGBColor a, RGBColor b) {
	return sqr(a.r - b.r) + sqr(a.g - b.g) + sqr(a.b - b.b);
}

unsigned short int mulBy255(unsigned short int n) {
	return (n << 8) - n;
}

unsigned short int divBy255(unsigned short int n) {
	return (n >> 8) + 1;
}

unsigned short int divBy60(unsigned short int n) {
	return (n >> 6) + (n >> 10) + 1;
}

unsigned char max(unsigned char r, unsigned char g, unsigned char b) {
	return std::max(r, std::max(g, b));
}

unsigned char min(unsigned char r, unsigned char g, unsigned char b) {
	return std::min(r, std::min(g, b));
}

HSVColor RGBtoHSV(RGBColor rgb) {
	unsigned char r = rgb.r;
	unsigned char g = rgb.g;
	unsigned char b = rgb.b;

	unsigned char Xmax = max(r, g, b);

	if (Xmax == 0) {
		return HSVColor(0.0, 0, 0);
	}

	short int delta = Xmax - min(r, g, b);

	if (delta == 0) {
		return HSVColor(0.0, 0, Xmax);
	}

	unsigned char v = Xmax;
	unsigned char s = roundShort((10 * mulBy255(delta)) / Xmax);

	short int h;
	if (v == r) {
		short int n = g - b;

		h = preciseHue(n, delta, 0);
	} else if (v == g) {
		short int n = b - r;

		h = preciseHue(n, delta, 120);
	} else {
		short int n = r - g;

		h = preciseHue(n, delta, 240);
	}

	return HSVColor(h, s, v);
}

RGBColor HSVtoRGB(HSVColor hsv) {
	unsigned char s = hsv.s;
	unsigned char v = hsv.v;

	if (s == 0) {
		return RGBColor(v, v, v);
	}

	// special mod function for actual modulus operation.
	short int h = mod(hsv.h, 360);

	// c in [0, v]
	unsigned char c = divBy255((short)v * (short)s);

	// rem in [0, 119]
	unsigned char rem = h % 120;

	// x in [0, 255]
	unsigned char x;

	if (rem == 0) {
		x = 0;
	} else if (rem == 60) {
		x = c;
	} else {
		// a in [0, 60]
		unsigned short a = (60 - abs(rem - 60));

		x = (c * a) / 60;
	}

	// Cm in [0, 255]
	unsigned char Cm = v;

	// m in [v, 0]
	unsigned char _m = v - c;

	// Xm in ??? (I don't know at this point)
	short int Xm = x + _m;

	// region in [0, 5]
	unsigned char region = calcRegion(h);
	switch (region) {
		case 0: return RGBColor(Cm, Xm, _m);
		case 1: return RGBColor(Xm, Cm, _m);
		case 2: return RGBColor(_m, Cm, Xm);
		case 3: return RGBColor(_m, Xm, Cm);
		case 4: return RGBColor(Xm, _m, Cm);
		case 5: return RGBColor(Cm, _m, Xm);
		default: return RGBColor(_m, _m, _m);
	}
}

#endif