/*プレイヤー*/

#pragma once
#include "Model.h"
#include "Object3d.h"
#include "RailCamera.h"
#include "Sprite.h"
#include "SpriteBasis.h"
#include <Input.h>

#include "Ease.h"
#include "Gauge.h"
#include "Text.h"

#include "Shake.h"

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

	//開始時移動
	void StartMove();

	//脱落時移動
	void OverMove();

public: //定数
	//自機のデフォルト体力
	const float kDefaultPlayerLife_ = 10.0f;

	//銃ダメージ量
	const float kGunDamage_ = 3.0f;

	//弾数の上限
	const int kBulletRimit_ = 30;

	//デフォルトクールタイム
	const int kDefaultBulletCooltime_ = 1;

	//自機のデフォルトY座標
	const float kDefaultPosY_ = -5.0f;

	//自機のデフォルトZ座標
	const float kDefaultPosZ_ = 30.0f;

public: //アクセッサ
	//ゲームシーンのセット
	void SetGameScene(GamePlayScene* gameScene) {
		gameScene_ = gameScene;
	}

	//位置の取得
	const Vector3& GetPosition() const {
		return worldTransform_.position_;
	}
	//半径の取得
	float GetRadius() const {
		return radius_;
	}

	//レールカメラのワールド変換取得
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

	//スタートフラグの取得
	bool IsStart() {
		return isStart_;
	}

	//スタートフラグのセット
	void SetIsStart(bool isStart) {
		isStart_ = isStart;
	}

	//ゲームオーバーフラグの取得
	bool IsOver() {
		return isOver_;
	}

	//ゲームオーバーフラグのセット
	void SetIsOver(bool isOver) {
		isOver_ = isOver;
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

	//残弾数
	int remainBulletCount_ = kBulletRimit_;

	//弾のクールタイム
	int32_t bulletCooltime_ = kDefaultBulletCooltime_;

	//レティクル用スプライト
	Sprite* spriteReticle_ = nullptr;

	/// <summary>
	/// HP
	/// </summary>
	Gauge* hpGauge_ = {};

	//HPゲージの位置(左上角)
	Vector2 positionHPGauge_ = {
		0,
		0
	};

	//HPゲージ位置のオフセット
	Vector2 positionHPGaugeOffset_ = {
		0,
		0
	};

	//HP用イージング最大時間
	float maxTimeHP_ = 30.0f;

	/// <summary>
	/// 残弾
	/// </summary>
	Gauge* bulletGauge_ = {};

	//残弾ゲージの位置(左上角)
	Vector2 positionBulletGauge_ = {
		0,
		0
	};

	//残弾ゲージ位置のオフセット
	Vector2 positionBulletGaugeOffset_ = {
		0,
		0
	};

	//残弾用イージング最大時間
	float maxTimeBullet_ = 30.0f;

	//シェイク
	Shake shake_;

	//スタートフラグ
	bool isStart_ = false;
	//ゲームオーバーフラグ
	bool isOver_ = false;

	//タイマー最大値
	int timerMax_ = 60;
	//タイマー現在値
	int timerNow_ = 0;

	//イーズ
	Ease ease_;

	//イーズ2
	Ease ease_2;

	//イーズ3
	Ease ease_3;

	//イーズ4
	Ease ease_4;

	//スピード
	float speed_ = 0.5f;

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