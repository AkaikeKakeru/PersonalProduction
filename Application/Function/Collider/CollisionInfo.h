﻿/*衝突時の相手コライダー情報*/

#pragma once
#include "Vector3.h"

class Object3d;
class BaseCollider;

/*衝突時の相手コライダー情報*/
struct CollisionInfo {
public:
	/// <summary>
	/// コライダー情報
	/// </summary>
	/// <param name="object">オブジェクト</param>
	/// <param name="collider">コライダ－</param>
	/// <param name="inter">衝突点</param>
	CollisionInfo(Object3d* object, BaseCollider* collider,
		const Vector3& inter) {
		object_ = object;
		collider_ = collider;
		inter_ = inter;
	}

	//衝突相手のオブジェクト
	Object3d* object_ = nullptr;
	//衝突相手のコライダー
	BaseCollider* collider_ = nullptr;
	//衝突点
	Vector3 inter_;
};