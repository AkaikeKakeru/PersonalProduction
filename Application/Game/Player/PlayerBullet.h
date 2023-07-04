#pragma once
#include "Model.h"
#include "Object3d.h"
#include "Sprite.h"
#include "DrawBasis.h"

class CollisionManager;

class PlayerBullet :
	public Object3d{
public: //静的メンバ関数
	//オブジェクト生成
	static PlayerBullet* Create(Model* model = nullptr);

public://メンバ関数
	bool Initialize() override;
	void Update() override;
	void Draw();
	void Finalize();

	//衝突時コールバック関数
	void OnCollision(const CollisionInfo & info) override;

public: //アクセッサ
	const Vector3& GetPosition() const {
		return worldTransform_.position_; }
	float GetRadius() const {
		return radius_; }

	void SetWorldTransformRailCamera(WorldTransform* worldTransformRailCamera) {
		worldTransform_.parent_ = worldTransformRailCamera;
	}

private: //静的メンバ変数
	//衝突マネージャー
	static CollisionManager* collisionManager_;

private: //メンバ変数
	//半径
	float radius_ = 1.0f;

public:
	PlayerBullet() = default;
	~PlayerBullet() = default;
};