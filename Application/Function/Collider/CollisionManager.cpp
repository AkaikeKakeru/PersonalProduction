#include "CollisionManager.h"
#include "BaseCollider.h"
#include "Collision.h"

CollisionManager* CollisionManager::GetInstance() {
	static CollisionManager instance;
	return &instance;
}

void CollisionManager::CheckAllCollisions() {
	std::forward_list<BaseCollider*>::iterator itA;
	std::forward_list<BaseCollider*>::iterator itB;

	//全ての組み合わせについて総当たり
	itA = colliders_.begin();
	for (; itA != colliders_.end(); ++itA) {
		itB = itA;
		++itB;
		for (; itB != colliders_.end(); ++itB) {
			BaseCollider* colA = *itA;
			BaseCollider* colB = *itB;

			//属性が違わなければスキップ
			if ((colA->attribute_ & colB->attribute_)) {
				continue;
			}

			if (colA->GetShapeType() == COLLISIONSHAPE_SPHERE
				&& colB->GetShapeType() == COLLISIONSHAPE_SPHERE) {
				//ともに球
				CheckSphere2Sphere(colA, colB);
			}
		}
	}
}

void CollisionManager::CheckSphere2Sphere(BaseCollider* colA, BaseCollider* colB) {
	Sphere* SphereA = dynamic_cast<Sphere*>(colA);
	Sphere* SphereB = dynamic_cast<Sphere*>(colB);
	Vector3 inter;

	if (Collision::CheckSphere2Sphere(*SphereA, *SphereB, &inter)) {
		colA->OnCollision(CollisionInfo(colB->GetObject3d(), colB, inter));
		colB->OnCollision(CollisionInfo(colA->GetObject3d(), colA, inter));
	}
}

bool CollisionManager::Raycast(const Ray& ray, RaycastHit* hitInfo,
	float maxDistance) {
	return Raycast(ray,0xffff,hitInfo,maxDistance);
}

bool CollisionManager::Raycast(const Ray& ray, unsigned short attribute, RaycastHit* hitInfo, float maxDistance) {
	bool result = false;

	//走査用イテレータ
	std::forward_list<BaseCollider*>::iterator it;
	//今までで最も近いコライダーを記録するためのイテレータ
	std::forward_list<BaseCollider*>::iterator it_hit;
	//今までで最も近いコライダーの距離を記録する変数
	float distance = maxDistance;
	//今までで最も近いコライダーとの交点を記録するための変数
	Vector3 inter;

	//全てのコライダーと総当たりチェック
	it = colliders_.begin();
	for (; it != colliders_.end(); ++it) {
		BaseCollider* colA = *it;

		//属性が合わなければスキップ
		if (!(colA->attribute_ & attribute)) {
			continue;
		}

		//球の場合
		if (colA->GetShapeType() == COLLISIONSHAPE_SPHERE) {
			Sphere* sphere = dynamic_cast<Sphere*>(colA);
			float tempDistance;
			Vector3 tempInter;

			//当たらなければ除外
			if (!Collision::CheckRay2Sphere(ray, *sphere, &tempDistance, &tempInter))continue;

			//距離が最小でなければ除外
			if (tempDistance >= distance)continue;

			//今までで最も近いので記録を取る
			result = true;
			distance = tempDistance;
			inter = tempInter;
			it_hit = it;
		}
	}

	//最終的に何かに当たっていたら結果を書き込む
	if (result && hitInfo) {
		hitInfo->distance_ = distance;
		hitInfo->inter_ = inter;
		hitInfo->collider_ = *it_hit;
		hitInfo->object_ = hitInfo->collider_->GetObject3d();
	}

	return result;
}
