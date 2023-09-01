#pragma once
#include "Model.h"
#include "Object3d.h"
#include "RailCamera.h"
#include "Sprite.h"
#include "SpriteBasis.h"
#include <Input.h>

#include "Gauge.h"
#include "Text.h"

#include <list>
#include <memory>

class GamePlayScene;
class PlayerBullet;
class CollisionManager;

//プレイヤー
class Player
	: public Object3d {
public: //静的メンバ関数
	//オブジェクト生成
	static Player* Create(Model* model = nullptr);

public://メンバ関数
	bool Initialize() override;
	void Update() override;
	void Draw();
	void DrawUI();
	void DrawImgui();
	void Finalize();

	//衝突時コールバック関数
	void OnCollision(const CollisionInfo& info) override;

	//照準
	void Reticle();

	//発射攻撃
	void Attack();

public: //定数
	//自機のデフォルト体力
	const float kDefaultPlayerLife_ = 10.0f;

	//銃ダメージ量
	const float kGunDamage_ = 3.0f;

	//弾数の上限
	const int kBulletRimit_ = 30;

public: //アクセッサ
	//ゲームシーンのセット
	void SetGameScene(GamePlayScene* gameScene) {
		gameScene_ = gameScene;
	}

	const Vector3& GetPosition() const {
		return worldTransform_.position_;
	}
	float GetRadius() const {
		return radius_;
	}

	void SetWorldTransformRailCamera(WorldTransform* worldTransformRailCamera) {
		worldTransform_.parent_ = worldTransformRailCamera;
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

	//デスフラグの取得
	bool IsDead() {
		return isDead_;
	}

	//デスフラグのセット
	void SetIsDead(bool isDead) {
		isDead_ = isDead;
	}

	//ダメージフラグの取得
	bool IsDamage() {
		return isDamage_;
	}

	//ダメージフラグのセット
	void SetIsDamage(bool isDamage) {
		isDamage_ = isDamage;
	}

	//隠れフラグの取得
	bool IsHide() {
		return isHide_;
	}

	//隠れフラグのセット
	void SetIsHide(bool isHide) {
		isHide_ = isHide;
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
	//テキスト
	Text* text_ = nullptr;

	//ゲームシーン
	GamePlayScene* gameScene_ = nullptr;

	//半径
	float radius_ = 1.0f;

	//3dレティクルのワールド変換
	WorldTransform worldTransform3dReticle_;

	//弾モデル
	Model* bulletModel_ = nullptr;

	//体力
	float life_ = kDefaultPlayerLife_;

	//デスフラグ
	bool isDead_ = false;

	//ダメージフラグ
	bool isDamage_ = false;

	//隠れフラグ
	bool isHide_ = false;

	//発射した弾数
	int firedCount_ = 0;

	/// <summary>
	/// HP
	/// </summary>
	Gauge* hpGauge_ = {};

	//HPゲージ1メモリ当たりの長さ
	float lengthHPGauge_ = 16.0f;
	//HPゲージの位置(左上角)
	Vector2 positionHPGauge_ = {
		lengthHPGauge_ * 2,
		lengthHPGauge_ * 2};

	//HPゲージ位置のオフセット
	Vector2 positionHPGaugeOffset_ =// { (64.0f * 4),(64.0f * 4) };
	{ lengthHPGauge_ / 2,lengthHPGauge_ / 2 };

	//レティクル用スプライト
	Sprite* spriteReticle_ = nullptr;

	//HP用位置イージング始発点
	Vector3 startEaseHPPosition_{};
	//HP用位置イージング終着点
	Vector3 endEaseHPPosition_{};

	//HP用サイズイージング始発点
	Vector3 startEaseHPSize_{};
	//HP用サイズイージング終着点
	Vector3 endEaseHPSize_{};

	//HP用イージング最大時間
	float maxTimeHP_ = 30.0f;

private: //ImGui用
	//Vector3の要素数
	static const int kVector3Count_ = 3;

	//Pos範囲
	const float PosRange_ = 30.0f;

	//Dir範囲
	const float DirRange_ = ConvertToRadian(360.0f);

	//ImGui用自機Pos
	float debugPos_[kVector3Count_] = {};

	//ImGui用自機Dir
	float debugDir_[kVector3Count_] = {};

public:
	Player() = default;
	~Player() = default;
};