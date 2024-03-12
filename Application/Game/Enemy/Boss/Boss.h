/*ボス*/

#pragma once
#include "Character.h"
#include "WeakPoint.h"

#include <vector>
#include <List>
#include <memory>

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
	void StartMove() override;

	//脱落時移動
	void OverMove() override;

	void Attack() override;

	void ResetWeak();

public://定数
	//発射間隔
	static const int kFireInterval = 60 * 10;

	//待機間隔
	static const int kWaitInterval = 60 * 5;

	//弱点数
	static const int kWeakPointCount_ = 4;

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

	//攻撃までのタイマー
	int32_t attackTimer_;

	//待機タイマー
	int32_t waitTimer_;

	//弱点
	std::list<std::unique_ptr<WeakPoint>> weakPoint_;

	//攻撃フラグ
	bool isAttack_ = false;

	//ブレイクフラグ
	bool isBreak_ = false;

	//活動フラグ
	bool isActive_ = false;

public:
	Boss() = default;
	~Boss() = default;
};
