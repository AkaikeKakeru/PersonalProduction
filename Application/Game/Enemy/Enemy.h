/*エネミー*/

#pragma once
#include "Character.h"
#include "BaseEnemy.h"

class GamePlayScene;
class Player;
class CollisionManager;

//エネミー
class Enemy
	: public BaseEnemy{
public: //サブ構造体
	enum BulletType{
		Gun_BulletType,
		Axe_BulletType,
	};

public: //静的メンバ関数
	//オブジェクト生成
	static Enemy* Create(Model* model = nullptr);

public://メンバ関数
	bool Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawUI() override;
	void DrawImgui() override;
	void Finalize() override;

	//衝突時コールバック関数
	void OnCollision(const CollisionInfo& info) override;

	//発射
	void Attack() override;

	//開始時移動
	void StartMove() override;

	//脱落時移動
	void OverMove() override;

	//落下
	void Fall();

	//イージング位置リセット
	void ReSetEasePos();

//public: //定数
	//調整変数グループ名
	const char* groupName_ = "Enemy";

	//デフォルト体力
	const float kDefaultLife_ = 10.0f;

	//HP用イージング最大時間
	const float kMaxTimeHP_ = 30.0f;

	//銃ダメージ量
	const float kGunDamage_ = 3.0f;

	//デフォルトクールタイム
	const int kDefaultBulletCoolTime_ = 1;

public: //アクセッサ


	//弾種のセット
	void SetBulletType(int bulletType) {
		bulletType_ = bulletType;
	}

private: //メンバ変数

	//弾種
	int bulletType_ = Gun_BulletType;

public:
	Enemy() = default;
	~Enemy() = default;
};
