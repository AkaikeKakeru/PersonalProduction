#pragma once
#include "Character.h"

class CollisionManager;

class Boss
	: public Character {
public: //静的メンバ関数
	//オブジェクト生成
	static Boss* Create(Model* model = nullptr);

public://メンバ関数
	bool Initialize() override;
	void Update() override;
	void Draw();
	void DrawUI();
	void DrawImgui();
	void Finalize();

	//衝突時コールバック関数
	void OnCollision(const CollisionInfo& info) override;

	//開始時移動
	void StartMove();

	//脱落時移動
	void OverMove();

public: //定数
	//調整変数グループ名
	const char* groupName_ = "Boss";

	//デフォルト体力
	const float kDefaultLife_ = 10.0f;

	//HP用イージング最大時間
	const float kMaxTimeHP_ = 30.0f;

	//銃ダメージ量
	const float kGunDamage_ = 3.0f;

	//デフォルトクールタイム
	const int kDefaultBulletCoolTime_ = 1;

	//デフォルトX座標
	const float kDefaultPosX_ = 0.0f;
	//デフォルトY座標
	const float kDefaultPosY_ = 0.0f;
	//デフォルトZ座標
	const float kDefaultPosZ_ = 0.0f;

private:
	//オーバーフラグ
	bool isOver_ = false;

public:
	Boss() = default;
	~Boss() = default;
};
