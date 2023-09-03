#pragma once
#include "Model.h"
#include "Object3d.h"
#include "Sprite.h"
#include "SpriteBasis.h"
#include <Input.h>

#include "Gauge.h"

#include <list>
#include <memory>

class GamePlayScene;
class Player;
class CollisionManager;

//エネミー
class Enemy
	: public Object3d {
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

public://定数
	//発射間隔
	static const int kFireInterval = 60 * 5;

	//敵機のデフォルト体力
	const float kDefaultEnemyLife_ = 10.0f;

public: //アクセッサ
	const Vector3& GetPosition() const {
		return worldTransform_.position_;
	}
	float GetRadius() const {
		return radius_;
	}

	//弾モデルのセット
	void SetBulletModel(Model* bulletModel) {
		bulletModel_ = bulletModel;
	}

	//体力取得
	float GetLife() {
		return life_;
	}

	//体力のセット
	void SetLife(float life) {
		life_ = life;
	}

	//ダメージフラグの取得
	bool IsDamage() {
		return isDamage_;
	}

	//ダメージフラグのセット
	void SetIsDamage(bool isDamage) {
		isDamage_ = isDamage;
	}

	//デスフラグの取得
	bool IsDead() {
		return isDead_;
	}
	//デスフラグのセット
	void SetIsDead(bool isDead) {
		isDead_ = isDead;
	}

	//ゲームシーンのセット
	void SetGameScene(GamePlayScene* gameScene) {
		gameScene_ = gameScene;
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

	//HPゲージの取得
	Gauge* GetHPGauge() const {
		return hpGauge_;
	}

private: //静的メンバ変数
	//衝突マネージャー
	static CollisionManager* collisionManager_;
	//入力
	static Input* input_;
	//スプライト基盤
	static SpriteBasis* spriteBas_;

private: //メンバ変数
	//ゲームシーン
	GamePlayScene* gameScene_ = nullptr;

	//プレイヤー
	Player* player_ = nullptr;

	//半径
	float radius_ = 1.0f;

	//3dレティクルのワールド変換
	WorldTransform worldTransform3dReticle_;

	//レティクル用スプライト
	Sprite* spriteReticle_ = nullptr;

	//弾モデル
	Model* bulletModel_ = nullptr;

	//体力
	float life_ = kDefaultEnemyLife_;

	//ダメージフラグ
	bool isDamage_ = false; 
	//デスフラグ
	bool isDead_ = false;

	//発射タイマー
	int32_t fireTimer_ = 0;

	//弾種
	int bulletType_ = Gun_BulletType;

	/// <summary>
	/// HP
	/// </summary>
	Gauge* hpGauge_ = {};

	//HPゲージの長さ
	float lengthHPGauge_ = 4.0f;

	//HPゲージの位置(左上角)
	Vector2 positionHPGauge_ = {
		lengthHPGauge_ * 2,
		lengthHPGauge_ * 2
	};

	//HPゲージ位置のオフセット
	Vector2 positionHPGaugeOffset_ = {
		lengthHPGauge_ / 2,
		lengthHPGauge_ / 2 
	};

	//HP用イージング最大時間
	float maxTimeHP_ = 30.0f;

private: //ImGui用
	//Vector3の要素数
	static const int kVector3Count_ = 3;

	//Pos範囲
	const float PosRange_ = 30.0f;

	//Dir範囲
	const float DirRange_ = ConvertToRadian(360.0f);

	//ImGui用敵Pos
	float debugPos_[kVector3Count_] = {};

	//ImGui用敵Dir
	float debugDir_[kVector3Count_] = {};

public:
	Enemy() = default;
	~Enemy() = default;
};