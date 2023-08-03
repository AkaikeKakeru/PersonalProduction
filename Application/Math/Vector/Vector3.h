#pragma once

class Vector3 {
public:
	float x;
	float y;
	float z;

public:
	Vector3();
	Vector3(float x, float y, float z);

	Vector3 operator+() const;
	Vector3 operator-() const;

	Vector3& operator+=(const Vector3& v);
	Vector3& operator-=(const Vector3& v);
	Vector3& operator*=(float s);
	Vector3& operator/=(float s);
};

//長さ
const float Vector3Length(Vector3 v);
//正規化
Vector3 Vector3Normalize(Vector3 v);
//内積
const float Vector3Dot(const Vector3& v1, const Vector3& v2);
//外積
const Vector3 Vector3Cross(const Vector3& v1, const Vector3& v2);

const Vector3 operator+(const Vector3& v1, const Vector3& v2);
const Vector3 operator-(const Vector3& v1, const Vector3& v2);
const Vector3 operator*(const Vector3& v, float s);
const Vector3 operator*(float s, const Vector3& v);
const Vector3 operator/(const Vector3& v, float s);

///線形補間
//基本補間
const Vector3 lerp(
	const Vector3& start,
	const Vector3& end,
	const float t);

//開始時早い補間
const Vector3 EaseIn(
	const Vector3& start,
	const Vector3& end,
	const float t);

//終了時早い補間
const Vector3 EaseOut(
	const Vector3& start,
	const Vector3& end,
	const float t);

//開始時と終了時早い補間
const Vector3 EaseInOut(
	const Vector3& start,
	const Vector3& end,
	const float t);

//スプライン曲線用
const Vector3 CatmullRomSpline(
	Vector3 P0,
	Vector3 P1,
	Vector3 P2,
	Vector3 P3,
	float t);