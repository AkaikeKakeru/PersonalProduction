#pragma once
#include "Vector3.h"
#include "Matrix4.h"

struct Quaternion {
	float x;
	float y;
	float z;
	float w;
};

Quaternion& operator+(Quaternion q0, Quaternion q1);

Quaternion& operator*(Quaternion q, float s);
Quaternion& operator*=(Quaternion q, float s);

Quaternion& operator*(float s, Quaternion q);
Quaternion& operator*=(float s, Quaternion q);

Quaternion& operator/(Quaternion q, float s);
Quaternion& operator/=(Quaternion q, float s);

//Quaternionの積
Quaternion QuaternionMultiplication(const Quaternion& q,const Quaternion& r);

//Quaternionの単位化
Quaternion QuaternionIdentity();

//Quaternionの共役
Quaternion QuaternionConjugation(const Quaternion& quaternion);

//Quaternionのノルム(長さ)
float QuaternionNorm(const Quaternion& quaternion);

//Quaternionの正規化
Quaternion QuaternionNormalize(const Quaternion& quaternion);

//逆Quaternion
Quaternion QuaternionInverse(const Quaternion& quaternion);


//任意軸回転を表すQuaternionの生成
Quaternion MakeAxisAngle(const Vector3& axis, float angle);

//ベクトルをQuaternionで回転させた結果のベクトル
Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion);

//回転ベクトルを生成
Vector3 CreateRotationVector(Vector3 axisAngle, float angleRadian);

//Quaternionから回転行列を求める
Matrix4 MakeRotateMatrix(const Quaternion& quaternion);


//Quaternion内積
const float QuaternionDot(const Quaternion& q0, const Quaternion &q1);

//球面線形補間
Quaternion Slerp(const Quaternion& q0, const Quaternion &q1, float t);

//uからtへの回転を生成
Quaternion DirectionToDirection(const Vector3& u, const Vector3& v);