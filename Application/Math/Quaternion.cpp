#include "Quaternion.h"
#include <cmath>
#include "MyMath.h"

Quaternion& operator+(Quaternion q0, Quaternion q1) {
	q0.x += q1.x;
	q0.y += q1.y;
	q0.z += q1.z;
	q0.w += q1.w;

	return q0;
}

Quaternion& operator*(Quaternion q, float s) {
	q.x *= s;
	q.y *= s;
	q.z *= s;
	q.w *= s;

	return q;
}

Quaternion& operator*=(Quaternion q, float s) {
	q.x *= s;
	q.y *= s;
	q.z *= s;
	q.w *= s;

	return q;
}

Quaternion& operator*(float s, Quaternion q) {
	q.x *= s;
	q.y *= s;
	q.z *= s;
	q.w *= s;

	return q;
}
Quaternion& operator*=(float s, Quaternion q) {
	q.x *= s;
	q.y *= s;
	q.z *= s;
	q.w *= s;

	return q;
}

Quaternion& operator/(Quaternion q, float s) {
	q.x /= s;
	q.y /= s;
	q.z /= s;
	q.w /= s;

	return q;
};

Quaternion& operator/=(Quaternion q, float s) {
	q.x /= s;
	q.y /= s;
	q.z /= s;
	q.w /= s;

	return q;
};

Quaternion QuaternionMultiplication(const Quaternion& q, const Quaternion& r) {
	Quaternion qr = {
		q.w * r.x - q.z * r.y + q.y * r.z + q.x * r.w,
		q.z * r.x + q.w * r.y - q.x * r.z + q.y * r.w,
		-q.y * r.x + q.x * r.y + q.w * r.z + q.z * r.w,
		-q.x * r.x - q.y * r.y - q.z * r.z + q.w * r.w
	};

	return qr;
}

Quaternion QuaternionIdentity() {
	Quaternion identity = {
		0.0f,
		0.0f,
		0.0f,
		1.0f
	};

	return identity;
}

Quaternion QuaternionConjugation(const Quaternion& quaternion) {
	Quaternion conjugation = {
		-quaternion.x,
		-quaternion.y,
		-quaternion.z,
		quaternion.w
	};

	return conjugation;
}

float QuaternionNorm(const Quaternion& quaternion) {
	float norm =
		sqrt((quaternion.w * quaternion.w)
			+ (quaternion.x * quaternion.x)
			+ (quaternion.y * quaternion.y)
			+ (quaternion.z * quaternion.z)
		);

	return norm;
}

Quaternion QuaternionNormalize(const Quaternion& quaternion) {
	float len = QuaternionNorm(quaternion);

	if (len != 0.0f) {
		return quaternion /= len;
	}

	return quaternion;
}

Quaternion QuaternionInverse(const Quaternion& quaternion) {
	Quaternion conj = QuaternionConjugation(quaternion);
	float norm = QuaternionNorm(quaternion);

	Quaternion inverse = conj / (norm * norm);

	return inverse;
}

Quaternion MakeAxisAngle(const Vector3& axis, float angle) {
	float c = cosf(angle / 2.0f);
	float s = sinf(angle / 2.0f);

	Vector3 axisNormal = Vector3Normalize(axis);

	Vector3 v = s * axisNormal;

	Quaternion quaternion = {
		v.x,v.y,v.z,c
	};

	return quaternion;
}

Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion) {
	Vector3 rotated = {};

	Quaternion r = { vector.x,vector.y,vector.z, 0.0f };

	Quaternion a = QuaternionMultiplication(quaternion, r);

	Quaternion b = QuaternionConjugation(quaternion);

	r = QuaternionMultiplication(a, b);

	rotated = { r.x,r.y,r.z };

	return rotated;
}

Vector3 CreateRotationVector(Vector3 axisAngle, float angleRadian) {
	Quaternion rotation = MakeAxisAngle(axisAngle, ConvertToRadian(1.0f));
	Vector3 point = axisAngle * angleRadian;

	return RotateVector(point, rotation);
}

Matrix4 MakeRotateMatrix(const Quaternion& quaternion) {
	Matrix4 R = {};

	float x = quaternion.x;
	float y = quaternion.y;
	float z = quaternion.z;
	float w = quaternion.w;

	R.m[0][0] = (w * w) + (x * x) - (y * y) - (z * z);
	R.m[0][1] = 2.0f * ((x * y) + (w * z));
	R.m[0][2] = 2.0f * ((x * z) - (w * y));
	R.m[0][3] = 0.0f;

	R.m[1][0] = 2.0f * ((x * y) - (w * z));
	R.m[1][1] = (w * w) - (x * x) + (y * y) - (z * z);
	R.m[1][2] = 2.0f * ((y * z) + (w * x));
	R.m[1][3] = 0.0f;

	R.m[2][0] = 2.0f * ((x * z) + (w * y));
	R.m[2][1] = 2.0f * ((y * z) - (w * x));
	R.m[2][2] = (w * w) - (x * x) - (y * y) + (z * z);
	R.m[2][3] = 0.0f;

	R.m[3][0] = 0.0f;
	R.m[3][1] = 0.0f;
	R.m[3][2] = 0.0f;
	R.m[3][3] = 1.0f;

	return R;
}

const float QuaternionDot(const Quaternion& q0, const Quaternion& q1) {
	return q0.x * q1.x + q0.y * q1.y + q0.z * q1.z + q0.w * q1.w;
}

Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t) {
	const float EPSILON = 0.0005f;

	//q0とq1の内積
	float dot = QuaternionDot(q0, q1);

	Quaternion qA = q0;

	//内積が +か -かを確認
	if (dot < 0.0f) {
		qA = qA * -1.0f;//もう片方の回転を利用
		dot = -dot;//内積も反転
	}

	if (dot >= 1.0f - EPSILON) {
		return (1.0f - t) * q0 + t * q1;
	}

	//なす角を求める
	float theta = std::acos(dot);

	//補間係数を求める
	float scale0 = sinf((1.0f - t) * theta) / sinf(theta);
	//補間係数を求める
	float scale1 = sinf(t * theta) / sinf(theta);

	//補間係数を用いて、補間後のQuaternionを返す
	return scale0 * q0 + scale1 * q1;
}

Quaternion DirectionToDirection(const Vector3& u, const Vector3& v) {
	//uとvを正規化して内積を求める。
	float dot = Vector3Dot(
		Vector3Normalize(u),
		Vector3Normalize(v));

	//uとvの外積
	Vector3 cross = Vector3Cross(u, v);

	//軸は正規化必須
	Vector3 axis = Vector3Normalize(cross);

	//単位ベクトルで内積を取っているのでacosで角度を求める
	float theta = std::acos(dot);

	//任意軸回転を作る
	return Quaternion({
		axis.x * sinf(theta / 2.0f),
		axis.y * sinf(theta / 2.0f),
		axis.z * sinf(theta / 2.0f),
		cosf(theta / 2.0f)
		});
}
