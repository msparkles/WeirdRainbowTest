#ifndef _test_mask
#define _test_mask

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include <cmath>
#include <functional>
#include <iostream>

size_t powerOfTwo(int n) {
	return (size_t)ceil(log2(n));
}

template <typename T>
class Mask {
private:
	typedef typename std::make_unsigned<T>::type uT;

	const size_t byteSize = sizeof(T);
	const size_t bitSize  = byteSize * CHAR_BIT;

	const size_t lastBitPos	 = bitSize - 1;
	const size_t firstBitPos = 0;

	const uT rightmostBit = uT(1);
	const uT leftmostBit  = rightmostBit << lastBitPos;

	const uT empty	= uT(0);
	const uT filled = ~empty;

private:
	size_t len(int n) {
		if (n == 0) return 0;
		n = abs(n);

		return ((n - 1) >> (powerOfTwo(bitSize))) + 1;
	}

	size_t atPos(int n) {
		if (n == 0) return 0;
		n = abs(n);

		return (n >> (powerOfTwo(bitSize)));
	}

	bool validRange(int x, int y) {
		return (x >= 0 && y >= 0) && (x < width && y < height);
	}

	bool invalidRange(int x, int y) {
		return !validRange(x, y);
	}

	bool validPointRange(int x, int y) {
		return (x >= 0 && y >= 0) && (x < bitsWidth && y < bitsHeight);
	}

	bool invalidPointRange(int x, int y) {
		return !validPointRange(x, y);
	}

	void printBinary(T n) {
		uT mask = leftmostBit;

		while (mask) {
			printf("%c", (n & mask) ? '1' : '0');
			mask >>= 1;
		}
	}

	uT bitAt(int i) {
		return (leftmostBit >> (i % bitSize));
	}

	// a and b are bits count
	// e.g. from bit 0 to bit 15 -> range(0, 15)
	uT range(int a, int b) {
		if (a > b) {
			size_t t = a;
			a		 = b;
			b		 = t;
		}

		a = fmin(a, lastBitPos);
		b = fmin(b, lastBitPos);

		size_t n   = leftmostBit >> a;
		size_t end = leftmostBit >> b;

		// while n and end don't touch
		while (!(n & end)) {
			n |= n >> 1;
		}

		return n;
	}

private:
	T** mask;

	size_t width;
	size_t height;

	size_t bitsWidth;
	size_t bitsHeight;

public:
	Mask(int w, int h) {
		width  = len(w);
		height = abs(h);

		bitsWidth  = abs(w);
		bitsHeight = height;

		mask = new T*[height];

		for (int i = 0; i < height; i++) {
			mask[i] = new T[width]{0};
		}
	}

	~Mask() {
		for (int i = 0; i < height; i++) {
			delete mask[i];
		}
		delete mask;
	}

public:
	T* at(int x, int y) {
		if (invalidRange(x, y)) return NULL;

		return mask[y] + x;
	}

	T* atPoint(int x, int y) {
		if (invalidPointRange(x, y)) return NULL;

		return at(atPos(x), y);
	}

	bool isOn(int x, int y) {
		T* it = atPoint(x, y);
		if (!it) return false;

		return *it & bitAt(x);
	}

	void forEach(std::function<void(int, int, T*)> consumer) {
		for (size_t i = 0; i < height; i++) {
			T* a = mask[i];
			for (size_t j = 0; j < width; j++) {
				consumer(j, i, a + j);
			}
		}
	}

	void allOn() {
		forEach([this](int x, int y, T* it) {
			*it = filled;
		});
	}

	void allOff() {
		forEach([this](int x, int y, T* it) {
			*it = empty;
		});
	}

	void allToggle() {
		forEach([this](int x, int y, T* it) {
			*it ^= filled;
		});
	}

	void on(int x, int y) {
		T* it = atPoint(x, y);
		if (!it) return;

		*it |= bitAt(x);
	}

	void off(int x, int y) {
		T* it = atPoint(x, y);
		if (!it) return;

		*it &= ~bitAt(x);
	}

	void toggle(int x, int y) {
		T* it = atPoint(x, y);
		if (!it) return;

		*it ^= bitAt(x);
	}

private:
	void square(int x1, int y1, int x2, int y2,
				std::function<void(T*, T)> consumer) {
		const size_t w = bitsWidth - 1;
		const size_t h = bitsHeight - 1;

		x1 = fmax(0, fmin(x1, w));
		x2 = fmax(0, fmin(x2, w));
		y1 = fmax(0, fmin(y1, h));
		y2 = fmax(0, fmin(y2, h));

		if (x1 > x2) {
			int t = x1;
			x1	  = x2;
			x2	  = t;
		}
		if (y1 > y2) {
			int t = y1;
			y1	  = y2;
			y2	  = t;
		}

		size_t _x1 = atPos(x1);
		size_t _x2 = atPos(x2);

		if (_x1 == _x2) {
			uT filled = range(x1 % bitSize, x2 % bitSize);

			for (size_t i = y1; i <= y2; i++) {
				consumer(mask[i] + _x1, filled);
			}
		} else {
			uT head = range(x1 % bitSize, lastBitPos);
			uT foot = range(firstBitPos, x2 % bitSize);

			for (size_t i = y1; i <= y2; i++) {
				T* it = mask[i];

				consumer(it + _x1, head);

				for (size_t j = _x1 + 1; j < _x2; j++) {
					consumer(it + j, filled);
				}

				consumer(it + _x2, foot);
			}
		}
	}

	const static void _on(T* it, T other) {
		*it |= other;
	}

	const static void _off(T* it, T other) {
		*it &= ~other;
	}

	const static void _toggle(T* it, T other) {
		*it ^= other;
	}

public:
	void on(int x1, int y1, int x2, int y2) {
		return square(x1, y1, x2, y2, _on);
	}

	void off(int x1, int y1, int x2, int y2) {
		return square(x1, y1, x2, y2, _off);
	}

	void toggle(int x1, int y1, int x2, int y2) {
		return square(x1, y1, x2, y2, _toggle);
	}

private:
	void shift_LR(int amount,
				  std::function<void(T*, T*, size_t, size_t, size_t)> consumer) {
		if (amount <= 0) return;

		size_t shift = fmin(bitSize, amount);
		size_t flip	 = bitSize - shift;

		T* last = new T(empty);

		for (size_t i = 0; i < height; i++) {
			T* a = mask[i];

			consumer(last, a, shift, flip, width);

			*last = empty;
		}

		delete last;

		if (amount > bitSize) {
			return shift_LR(amount - bitSize, consumer);
		}
	}

	const static void _left(T* last, T* a, size_t shift, size_t flip, size_t width) {
		// reverse loop
		// this weird thing is used because
		// normal reverse loop would break with
		// size_t due to wrap around
		// and I don't wanna change size_t.

		for (size_t j = width; j-- > 0;) {
			T* it = a + j;

			T sht = *it << shift;
			T rem = *it >> flip;

			sht |= *last;
			*last = rem;

			*it = sht;
		}
	}

	const static void _right(T* last, T* a, size_t shift, size_t flip, size_t width) {
		for (size_t j = 0; j < width; j++) {
			T* it = a + j;

			T sht = *it >> shift;
			T rem = *it << flip;

			sht |= *last;
			*last = rem;

			*it = sht;
		}
	}

public:
	void shiftLeft(int amount) {
		if (amount < 0) return shiftRight(abs(amount));

		return shift_LR(amount, _left);
	}

	void shiftRight(int amount) {
		if (amount < 0) return shiftLeft(abs(amount));

		return shift_LR(amount, _right);
	}

public:
	void shiftDown(int amount) {
		if (amount == 0) return;
		if (amount < 0) return shiftUp(abs(amount));

		amount = fmin(height, amount);

		for (size_t i = (height - amount); i-- > 0;) {
			T* a = mask[i + amount];
			T* b = mask[i];

			for (size_t j = 0; j < width; j++) {
				a[j] = b[j];
				b[j] = empty;
			}
		}

		for (size_t i = 0; i < amount; i++) {
			T* it = mask[i];

			for (size_t j = 0; j < width; j++) {
				it[j] = empty;
			}
		}
	}

	void shiftUp(int amount) {
		if (amount == 0) return;
		if (amount < 0) return shiftDown(abs(amount));

		amount = fmin(height, amount);

		for (size_t i = amount; i < height; i++) {
			T* a = mask[i - amount];
			T* b = mask[i];

			for (size_t j = 0; j < width; j++) {
				a[j] = b[j];
				b[j] = empty;
			}
		}

		for (size_t i = (height - amount); i < height; i++) {
			T* it = mask[i];

			for (size_t j = 0; j < width; j++) {
				it[j] = empty;
			}
		}
	}

	void printBits() {
		forEach([this](int x, int y, T* it) {
			printBinary(*it);
			if (x == width - 1) printf("\n");
		});
	}
};

#endif