#include "Vector3.h"
#include <cmath>	//sqrt

Vector3::Vector3()
	: x(0.0f), y(0.0f), z(0.0f) {
}

Vector3::Vector3(float x, float y, float z)
	: x(x), y(y), z(z) {
}

Vector3 Vector3::operator+() const {
	return *this;
}

Vector3 Vector3::operator-() const {
	return Vector3(-x, -y, -z);
}

Vector3& Vector3::operator+=(const Vector3& v) {
	this->x += v.x;
	this->y += v.y;
	this->z += v.z;
	return *this;
}

Vector3& Vector3::operator-=(const Vector3& v) {
	this->x -= v.x;
	this->y -= v.y;
	this->z -= v.z;
	return *this;
}

Vector3& Vector3::operator*=(float s) {
	x *= s;
	y *= s;
	z *= s;
	return *this;
}

Vector3& Vector3::operator/=(float s) {
	x /= s;
	y /= s;
	z /= s;
	return *this;
}

const float Vector3Length(Vector3 v) {
	return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

Vector3 Vector3Normalize(Vector3 v) {
	float len = Vector3Length(v);
	if (len > 0.0f) {
		len = 1 / len;
	}

	v = {
		v.x * len,
		v.y * len,
		v.z * len,};

	return v;
}

const float Vector3Dot(const Vector3& v1, const Vector3& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

const Vector3 Vector3Cross(const Vector3& v1, const Vector3& v2) {
	return Vector3(v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x);
}

const Vector3 operator+(const Vector3& v1, const Vector3& v2) {
	Vector3 temp(v1);
	return temp += v2;
}

const Vector3 operator-(const Vector3& v1, const Vector3& v2) {
	Vector3 temp(v1);
	return temp -= v2;
}

const Vector3 operator*(const Vector3& v, float s) {
	Vector3 temp(v);
	return temp *= s;
}

const Vector3 operator*(float s, const Vector3& v) {
	Vector3 temp(v);
	return temp *= s;
}

const Vector3 operator/(const Vector3& v, float s) {
	Vector3 temp(v);
	return temp /= s;
}

const Vector3 lerp(
	const Vector3& start,
	const Vector3& end,
	const float t) {
	return start * (1.0f - t) + end * t;
}

const Vector3 EaseIn(
	const Vector3& start,
	const Vector3& end,
	const float t) {
	//ease-in
	float y = t * t;
	return start * (1.0f - y) + end * y;
}

const Vector3 EaseOut(
	const Vector3& start,
	const Vector3& end,
	const float t) {
	//ease-out
	float y = t * (2.0f - t);
	return start * (1.0f - y) + end * y;
}

const Vector3 EaseInOut(
	const Vector3& start,
	const Vector3& end,
	const float t) {
	float a = t * t;
	float b = 3.0f - (2.0f * t);

	//ease-in,ease-out
	float y = a * b;
	return start * (1.0f - y) + end * y;
}

const Vector3 CatmullRomSpline(
	Vector3 P0,
	Vector3 P1,
	Vector3 P2,
	Vector3 P3,
	float t) {
	Vector3 a = (2.0f * P1) + ((-P0 + P2) * t);
	Vector3 b = ((2.0f * P0) - (5.0f * P1) + (4.0f * P2) - P3) * t * t;
	Vector3 c = (-P0 + (3.0f * P1) - (3.0f * P2) + P3) * t * t * t;

	return 0.5f * (a + b + c);
}