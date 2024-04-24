/*ボス*/

#pragma once
#include "Character.h"
#include "WeakPoint.h"

#include <vector>
#include <List>
#include <memory>

class CollisionManager;
class Player;

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

	void ResetNextEase();

	void ResetWeak();

	void SetTargetPos(const Vector3& targetPos) {
		targetPos_ = targetPos;
	}

private:
	void SortingSign(float& f);

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
	const float kDefaultPosZ_ = -50.0f;

	//横移動エーズ
	const float kMoveEaseRangeX_ = 10.0f;
	const float kMoveEaseRangeY_ = 10.0f;
	const float kMoveEaseRangeZ_ = 10.0f;

	//移動イーズのタイマー最大値
	const int kMoveEaseTimerMax_ = 80;

	//横移動待機タイマー
	const int kStayTimer_ = 120;
private:
	//オーバーフラグ
	bool isOver_ = false;

	//攻撃までのタイマー
	int32_t attackTimer_;

	//待機タイマー
	int32_t waitTimer_;

	//弱点
	std::list<std::unique_ptr<WeakPoint>> weakPoint_;

#pragma region 攻撃用
	//攻撃フラグ
	bool isAttack_ = false;
	//ブレイクフラグ
	bool isBreak_ = false;
	//活動フラグ
	bool isActive_ = false;
#pragma endregion

#pragma region 移動用
	Ease moveEase_;
	Ease rotaEase_;
	int32_t stayTimer_ = kStayTimer_;
	Vector3 moveEaseVec_ = {
		+1.0f,
		+1.0f,
		+1.0f
	};
	bool isMoveBoot_ = true;
#pragma endregion

	//半径
	float radiusCollider_ = 1.0f;

	//ターゲットの位置座標(主に回転など用)
	Vector3 targetPos_{};

public:
	Boss() = default;
	~Boss() = default;
};
