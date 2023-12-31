﻿/*二つのオブジェクトの衝突判定*/

#pragma once
#include "CollisionPrimitive.h"

/*二つのオブジェクトの衝突判定*/
class Collision {
public:
	/// <summary>
	///球と球の衝突判定
	/// </summary>
	/// <param name="sphereA">球A</param>
	/// <param name="sphereB">球B</param>
	/// <param name="inter">交点</param>
	/// <returns>衝突しているか</returns>
	static bool CheckSphere2Sphere(
		const Sphere& sphereA, const Sphere& sphereB, Vector3* inter = nullptr);

	/// <summary>
	//球と平面の交差判定
	/// </summary>
	/// <param name="sphere">球</param>
	/// <param name="plane">平面</param>
	/// <param name="inter">交点</param>
	/// <returns>交差しているか</returns>
	static bool CheckSphere2Plane(
		const Sphere& sphere, const Plane& plane, Vector3* inter = nullptr);

	/// <summary>
	//点と三角形の最近接点を求める
	/// </summary>
	/// <param name="point">点</param>
	/// <param name="triangle">三角形</param>
	/// <param name="closest">最近接点(出力用)</param>
	static void ClosestPtPoint2Triangle(
		const Vector3& point, const Triangle& triangle, Vector3* closest);

	/// <summary>
	//球と法線付き三角形の衝突判定
	/// </summary>
	/// <param name="sphere">球</param>
	/// <param name="triangle">三角形</param>
	/// <param name="inter">交点(三角形上の最近接点)</param>
	/// <returns>衝突しているか</returns>
	static bool CheckSphere2Triangle(
		const Sphere& sphere, const Triangle& triangle, Vector3* inter = nullptr);

	/// <summary>
	/// レイと平面の衝突判定
	/// </summary>
	/// <param name="ray">レイ</param>
	/// <param name="plane">平面</param>
	/// <param name="distance">距離(出力)</param>
	/// <param name="inter">交点(出力用)</param>
	/// <returns>衝突しているか</returns>
	static bool CheckRay2Plane(
		const Ray& ray, const Plane& plane,
		float* distance = nullptr, Vector3* inter = nullptr);

	/// <summary>
	/// レイと法線付き三角形の衝突判定
	/// </summary>
	/// <param name="ray">レイ</param>
	/// <param name="">三角形</param>
	/// <param name="distance">距離(出力)</param>
	/// <param name="inter">交点(出力用)</param>
	/// <returns>衝突しているか</returns>
	static bool CheckRay2Triangle(
		const Ray& ray, const Triangle& triangle,
		float* distance = nullptr, Vector3* inter = nullptr);

	/// <summary>
	/// レイと球の衝突判定
	/// </summary>
	/// <param name="ray">レイ</param>
	/// <param name="sphere">球</param>
	/// <param name="distance">距離(出力)</param>
	/// <param name="inter">交点(出力用)</param>
	/// <returns>衝突しているか</returns>
	static bool CheckRay2Sphere(
		const Ray& ray, const Sphere& sphere,
		float* distance = nullptr, Vector3* inter = nullptr);

private:
	//誤差吸収用の微小な値
	static const float EPSILON_;
};