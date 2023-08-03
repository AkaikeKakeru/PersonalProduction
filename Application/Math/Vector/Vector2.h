#pragma once

class Vector2 {
public:
	float x;
	float y;

public:
	Vector2();
	Vector2(float x, float y);

	Vector2 operator+() const;
	Vector2 operator-() const;

	Vector2& operator+=(const Vector2& v);
	Vector2& operator-=(const Vector2& v);
	Vector2& operator*=(float s);
	Vector2& operator/=(float s);
};
//長さ
const float Vector2Length(Vector2 v);
//正規化
const Vector2& Vector2Normalize(Vector2 v);
//内積
const float Vector2Dot(Vector2& v1, Vector2& v2);
//外積
const float Vector2Cross(Vector2& v1, Vector2& v2);

const Vector2 operator+(const Vector2& v1, const Vector2& v2);
const Vector2 operator-(const Vector2& v1, const Vector2& v2);
const Vector2 operator*(const Vector2& v, float s);
const Vector2 operator*(float s, const Vector2& v);
const Vector2 operator/(const Vector2& v, float s);