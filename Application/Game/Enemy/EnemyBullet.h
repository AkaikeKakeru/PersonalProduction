/*エネミー弾*/

#pragma once
//#include "Model.h"
//#include "Object3d.h"
//#include "Sprite.h"
//#include "SpriteBasis.h"
//
//class CollisionManager;
//class GamePlayScene;

#include "Bullet.h"

//エネミー弾
class EnemyBullet :
	public Bullet{
public: //サブ構造体
	enum BulletType{
		Gun_BulletType,
		Axe_BulletType,
	};

public://定数
	//斧の落下ペース
	const float kFallAxe_ = 0.098f;

	//銃型のダメージ量
	const float kGunDamage_ = 3.0f;
	//斧型のダメージ量
	const float kAxeDamage_ = 1.0f;

public: //静的メンバ関数
	//オブジェクト生成
	static EnemyBullet* Create(Model* model = nullptr);

public://メンバ関数
	bool Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;

	//衝突時コールバック関数
	void OnCollision(const CollisionInfo & info) override;

public: //アクセッサ
	//弾種のセット
	void SetBulletType(int bulletType) {
		bulletType_ = bulletType;
	}

private: //静的メンバ変数

public://メンバ定数
	//斧高さのデフォルト
	const float kDefaultHeightAxe_ = Cos(80.0f,1);

	//高さ変動タイマー<フレーム単位>
	static const int32_t kHeightTime_ = 18;
	
	//重力
	const float kGravity_ = 9.8f;

private: //メンバ変数
	//弾種
	int bulletType_ = Gun_BulletType;

	//斧高さ
	float heightAxe_ = 0.0f;

	//高さ変動までのタイマー
	float heightTimer_ = kHeightTime_;

public:
	EnemyBullet() = default;
	~EnemyBullet() = default;
};
