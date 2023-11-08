/*球形コライダーの座標更新*/

#pragma once

#include "BaseCollider.h"
#include "CollisionPrimitive.h"

/*球形コライダー*/
class SphereCollider
	: public BaseCollider, public Sphere {
public: //メンバ関数
	/// <summary>
	/// 球形コライダー
	/// </summary>
	/// <param name="offset">オフセット</param>
	/// <param name="radius">半径</param>
	SphereCollider(Vector3 offset = { 0,0,0 },
		float radius = 1.0f) :
		offset_(offset),
		radius_(radius) {
		//球形状をセット
		shapeType_ = COLLISIONSHAPE_SPHERE;
	}

	//更新
	void Update() override;

public://アクセッサ
	//半径取得
	inline float GetRadius() {
		return radius_;
	}

	//オフセット取得
	inline const Vector3& GetOffset() {
		return offset_; 
	}

	//半径セット
	inline void SetRadius(float radius) {
		radius_ = radius;
	}

	//オフセットセット
	inline void SetOffset(const Vector3& offset) {
		offset_ = offset;
	}

private: //メンバ変数
	//オブジェクト中心からのオフセット
	Vector3 offset_;
	//半径
	float radius_;
};