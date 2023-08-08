#include "Degree.h"

float ConvertToRadian(float degree) {
	const float PI = 3.14159265f;

	float radian = 0;
	radian = degree * (PI / 180);

	return radian;
}