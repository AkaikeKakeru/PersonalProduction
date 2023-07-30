#pragma once
#include "Model.h"
#include "Object3d.h"
#include "Sprite.h"
#include "SpriteBasis.h"

class CollisionManager;

class EnemyBullet :
	public Object3d{
public: //静的メンバ関数
	//オブジェクト生成
	static EnemyBullet* Create(Model* model = nullptr);

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

	//速度のセット
	void SetVelocity(Vector3 velocity) {
		velocity_ = velocity;
	}

	//自壊したかを取得
	bool IsDead() const {
		return isDead_;
	}

private: //静的メンバ変数
	//衝突マネージャー
	static CollisionManager* collisionManager_;

public://メンバ定数
	//寿命<フレーム単位>
	static const int32_t kLifeTime_ = 60 * 5;

private: //メンバ変数
	//半径
	float radius_ = 1.0f;

	//速度
	Vector3 velocity_ = {};

	//自壊タイマー
	int32_t deathTimer_ = kLifeTime_;

	//自壊フラグ
	bool isDead_ = false;

public:
	EnemyBullet() = default;
	~EnemyBullet() = default;
};