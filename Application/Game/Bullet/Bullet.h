/*プレイヤー弾*/

#pragma once
#include "Model.h"
#include "Object3d.h"
#include "Sprite.h"
#include "SpriteBasis.h"

class CollisionManager;
class GamePlayScene;

//プレイヤー弾
class Bullet :
	public Object3d{
public: //静的メンバ関数
	//オブジェクト生成
	static Bullet* Create(Model* model = nullptr);

public://定数

public://メンバ関数
	virtual bool Initialize() override;
	virtual void Update() override;
	virtual void Draw();
	virtual void Finalize();

	//衝突時コールバック関数
	void OnCollision(const CollisionInfo & info) override;

public: //アクセッサ
	//ゲームシーンのセット
	void SetGameScene(GamePlayScene* gameScene) {
		gameScene_ = gameScene;
	}

	//位置の取得
	const Vector3& GetPosition() const {
		return worldTransform_.position_; }
	//位置の半径
	float GetRadius() const {
		return radius_; }

	//速度の取得
	const Vector3& GetVelocity() const {
		return velocity_;
	}

	//速度のセット
	void SetVelocity(Vector3 velocity) {
		velocity_ = velocity;
	}

	//ダメージ量の取得
	float GetDamage() const {
		return damage_;
	}

	//ダメージ量のセット
	void SetDamage(float damage) {
		damage_ = damage;
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
	//ゲームシーン
	GamePlayScene* gameScene_ = nullptr;

	//半径
	float radius_ = 1.0f;

	//速度
	Vector3 velocity_ = {};

	//自壊タイマー
	int32_t deathTimer_ = kLifeTime_;

	//自壊フラグ
	bool isDead_ = false;

	//弾ダメージ量
	float damage_ = 0.0f;

public:
	Bullet() = default;
	~Bullet() = default;
};
