#include "Vector2.h"
#include <cmath> //sqrt

Vector2::Vector2()
	: x(0), y(0) {
}

Vector2::Vector2(float x, float y)
	: x(x), y(y) {
}

const float Vector2Length(Vector2 v) {
	return sqrt((v.x * v.x) + (v.y * v.y));
}

const Vector2& Vector2Normalize(Vector2 v) {
	Vector2 temp(v);
	float len = Vector2Length(v);
	if (len != 0) {
		return temp /= len;
	}
	return temp /= 1.0f;
}

const float Vector2Dot(Vector2& v1, Vector2& v2) {
	return v1.x * v2.x + v1.y * v2.y;
}

const float Vector2Cross(Vector2& v1, Vector2& v2) {
	return v1.x * v2.y - v1.y * v2.x;
}

Vector2 Vector2::operator+() const {
	return *this;
}

Vector2 Vector2::operator-() const {
	return Vector2(-x, -y);
}

Vector2& Vector2::operator+=(const Vector2& v) {
	this->x += v.x;
	this->y += v.y;
	return *this;
}

Vector2& Vector2::operator-=(const Vector2& v) {
	this->x -= v.x;
	this->y -= v.y;
	return *this;
}

Vector2& Vector2::operator*=(float s) {
	x *= s;
	y *= s;
	return *this;
}

Vector2& Vector2::operator/=(float s) {
	x /= s;
	y /= s;
	return *this;
}

//Vector2 �N���X�ɑ����Ȃ��֐��Q
//�񍀉��Z�q
const Vector2 operator+(const Vector2& v1, const Vector2& v2) {
	Vector2 temp(v1);
	return temp += v2;
}

const Vector2 operator-(const Vector2& v1, const Vector2& v2) {
	Vector2 temp(v1);
	return temp -= v2;
}

const Vector2 operator*(const Vector2& v, float s) {
	Vector2 temp(v);

	float x = temp.x * s;

	float y = temp.y * s;

	return Vector2(x,y);
}

const Vector2 operator*(float s, const Vector2& v) {
	return v * s;
}

const Vector2 operator/(const Vector2& v, float s) {
	Vector2 temp(v);

	float x = temp.x / s;

	float y = temp.y / s;

	return Vector2(x,y);
}