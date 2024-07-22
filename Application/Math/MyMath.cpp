/*自作の数学関数まとめ*/

#include "MyMath.h"
#include <cstdint>
#include <cassert>

//円周率
const float PI = 3.1415926535f;

float ConvertToRadian(float degree) {
	float radian = 0.0f;
	radian = degree * (PI / 180.0f);

	return radian;
}

float AbsoluteValue(float f) {
	f = (f * f) / f;
	return f;
}

float SquareRoot(float f) {
	double fD = (double)f;

	const float EPSILON = 0.00001f;

	double x = 1.0f, y = 0.0f;

	bool notEnd = true;

	//ニュートン法を用いる
	while (notEnd) {
		x = x - (x * x - fD) / (2.0f * x);
		y = x * x - fD;
		if ((y <= EPSILON) && (y >= -EPSILON)){
			notEnd = false;
		}
	}

	return (float)x;
}

float Pow(float f, size_t n) {
	float result = 1.0f;

	for (size_t k = 0; k < n; k++) {
		result *= f;
	}

	return result;
}

size_t Factorial(size_t n) {
	int result = 1;
	while (n > 1) {
		result *= (int)n--;
	}
		 
	return result;
}

float Sin(float f, size_t n) {
	float result = 0.0f;

	for (size_t k = 0; k < n; k++) {
		int32_t sign = (k % 2 == 0) ? 1 : -1;
		size_t k2p1 = 2 * k + 1;
		result += sign * Pow(f, k2p1) / float(Factorial(k2p1));
	}

	return result;
}

float Cos(float f, size_t n) {
	float result = 0.0f;

	for (size_t k = 0; k < n; k++) {
		int32_t sign = (k % 2 == 0) ? 1 : -1;
		size_t k2 = 2 * k;
		result += sign * Pow(f, k2) / float(Factorial(k2));
	}

	return result;
}

float Tan(float f, size_t n) {
	float sin = Sin(f, n);
	float cos = Cos(f, n);

	return sin / cos;
}
