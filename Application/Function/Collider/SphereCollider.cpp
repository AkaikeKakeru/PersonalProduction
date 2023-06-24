#include "SphereCollider.h"

void SphereCollider::Update() {
	//ワールド行列から座標を抽出
	const Matrix4& matWorld =
		object_->GetMatWorld();

	//球のメンバ変数を更新
	Sphere::center_.x = matWorld.m[3][0]
		+ offset_.x;
	Sphere::center_.y = matWorld.m[3][1]
		+ offset_.y;
	Sphere::center_.z = matWorld.m[3][2]
		+ offset_.z;

	Sphere::radius_ = radius_;
}
