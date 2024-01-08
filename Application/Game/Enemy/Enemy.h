/*エネミー*/

#pragma once
#include "Character.h"

class GamePlayScene;
class Player;
class CollisionManager;

//エネミー
class Enemy
	: public Character {
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
	void Draw();
	void DrawUI();
	void DrawImgui();
	void Finalize();

	//衝突時コールバック関数
	void OnCollision(const CollisionInfo& info) override;

	//発射
	void Fire();

	//落下
	void Fall();

	//イージング位置リセット
	void ReSetEasePos();

public://定数
	//発射間隔
	static const int kFireInterval = 60 * 5;

	//デスボーダー
	const float kDeadBorder_ = -30.0f;

//public: //定数
	//調整変数グループ名
	const char* groupName_ = "Enemy";

	//自機のデフォルト体力
	const float kDefaultLife_ = 10.0f;

	//HP用イージング最大時間
	const float kMaxTimeHP_ = 30.0f;

	//銃ダメージ量
	const float kGunDamage_ = 3.0f;

	//デフォルトクールタイム
	const int kDefaultBulletCooltime_ = 1;

	//デフォルトX座標
	const float kDefaultPosX_ = 0.0f;
	//デフォルトY座標
	const float kDefaultPosY_ = 0.0f;
	//デフォルトZ座標
	const float kDefaultPosZ_ = 0.0f;

public: //アクセッサ
	const Vector3& GetPosition() const {
		return worldTransform_.position_;
	}

	//プレイヤーのセット
	void SetPlayer(Player* player) {
		player_ = player;
	}

	//発射タイマーのセット(フレーム単位)
	void SetFireTimer(int32_t timerFrame) {
		fireTimer_ = timerFrame * 60;
	}

	//弾種のセット
	void SetBulletType(int bulletType) {
		bulletType_ = bulletType;
	}

private: //静的メンバ変数
	//衝突マネージャー
	static CollisionManager* collisionManager_;
	//入力
	static Input* input_;
	//スプライト基盤
	static SpriteBasis* spriteBas_;

private: //メンバ変数
	//プレイヤー
	Player* player_ = nullptr;

	//3dレティクルのワールド変換
	WorldTransform worldTransform3dReticle_;

	//レティクル用スプライト
	Sprite* spriteReticle_ = nullptr;

	//落下フラグ
	bool isFall_ = false;

	//落下の速さ
	float speedFall_ = 0.2f;

	//発射タイマー
	int32_t fireTimer_ = 0;

	//弾種
	int bulletType_ = Gun_BulletType;

	///// <summary>
	///// HP
	///// </summary>
	//Gauge* hpGauge_ = {};

	////HPゲージの長さ
	//float lengthHPGauge_ = 4.0f;

	////HPゲージの位置(左上角)
	//Vector2 positionHPGauge_ = {
	//	lengthHPGauge_ * 2,
	//	lengthHPGauge_ * 2
	//};

	////HPゲージ位置のオフセット
	//Vector2 positionHPGaugeOffset_ = {
	//	lengthHPGauge_ / 2,
	//	lengthHPGauge_ / 2 
	//};

	////HP用イージング最大時間
	//float maxTimeHP_ = 30.0f;

public:
	Enemy() = default;
	~Enemy() = default;
};
