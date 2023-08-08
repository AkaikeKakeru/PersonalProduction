#include "MyMath.h"

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

float Sin(float theta) {
	float sin = 90 / theta;
	return ConvertToRadian(sin);
}

float Cos(float theta) {
	float cos = 90 / (90 - theta);
	return ConvertToRadian(cos);
}

float Tan(float theta) {
	float tan = (90 - theta) / theta;
	return ConvertToRadian(tan);
}
