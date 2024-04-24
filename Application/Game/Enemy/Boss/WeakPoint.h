/*ボスの弱点*/

#pragma once
#include "Object3d.h"
#include "Vector3.h"

class WeakPoint :
	public Object3d {
public:
	static WeakPoint* Create(Model* model);

public:
	virtual bool Initialize() override;
	virtual void Update() override;
	virtual void Draw();

	//衝突時コールバック関数
	virtual void OnCollision(const CollisionInfo& info) override;

	void SetOffset(const Vector3& offset) {
		Vector3 v = offset;
		offsetPos_ = v;
	}

public:
	bool IsBreak() {
		return isBreak_;
	}

	void SetIsBreak(bool is) {
		isBreak_ = is;
	}

private:
	//半径
	float radiusCollider_ = 1.0f;

	//ブレイクフラグ
	bool isBreak_;

	//位置のオフセット
	Vector3 offsetPos_{};
};
