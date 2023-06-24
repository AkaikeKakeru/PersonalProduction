#include "MyMath.h"

float ConvertToRadian(float degree) {
	const float PI = 3.14159265f;

	float radian = 0.0f;
	radian = degree * (PI / 180.0f);

	return radian;
}

float AbsoluteValue(float f) {
	f = (f * f) / f;
	return f;
}

float SquareRoot(float f) {
	const float EPSILON = 0.00001f;

	float x = 1.0f, y = 0.0f;

	//ニュートン法を用いる
	while (true) {
		x = x - (x * x - f) / (2.0f * x);
		y = x * x - f;
		if ((y <= EPSILON) && (y >= -EPSILON)){
			break;
		}
	}

	return x;
}
