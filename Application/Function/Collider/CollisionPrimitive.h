#pragma once
#include "Vector3.h"

//球
struct Sphere {
	//中心
	Vector3 center_ = { 0,0,0 };

	//半径
	float radius_ = 1.0f;
};

//平面
struct Plane {
	//法線
	Vector3 normal_ = { 0,1,0 };

	//原点からの距離
	float distance_ = 0.0f;
};

//法線付き三角形(時計回りが表面)
class Triangle{
public:
	//法線の算出
	void ComputeNormal();

public:
	//頂点座標3つ
	Vector3 p0_;
	Vector3 p1_;
	Vector3 p2_;
	//法線ベクトル
	Vector3 normal_;
};

//レイ(半直線)
class Ray{
public:
	//始点座標
	Vector3 start_ = {0,0,0};
	//方向
	Vector3 dir_ = {1,0,0};
};
