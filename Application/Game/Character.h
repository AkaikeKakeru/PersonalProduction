/*キャラクター*/

#pragma once
#include "Model.h"
#include "Object3d.h"
#include "Sprite.h"
#include "SpriteBasis.h"
#include <Input.h>

#include "Cursor.h"
#include "Ease.h"
#include "Gauge.h"
#include "Text.h"

#include <List>
#include <memory>
#include <Cart.h>

class GamePlayScene;
class CollisionManager;

//キャラクター
class Character
	: public Object3d {
public: //静的メンバ関数
	//キャラクター生成
	static Character* Create();

public://メンバ関数
	virtual bool Initialize() override;
	virtual void Update() override;
	virtual void Draw();
	virtual void DrawUI();
	virtual void DrawImgui();
	virtual void Finalize();

	//調整項目の適用
	virtual void ApplyAdjustmentVariables();

	//衝突時コールバック関数
	virtual void OnCollision(const CollisionInfo& info) override;

	//開始時移動
	virtual void StartMove();

	//脱落時移動
	virtual void OverMove();

public: //アクセッサ
	//ゲームシーンのセット
	void SetGamePlayScene(GamePlayScene* gamePlayScene) {
		gamePlayScene_ = gamePlayScene;
	}

	//半径の取得
	float GetRadius() const {
		return radiusCollider_;
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

public:
	//ゲームプレイシーンの取得
	GamePlayScene* GetGamePlayScene() {
		return gamePlayScene_;
	}
	//弾モデルの取得
	Model* GetBulletModel() {
		return bulletModel_;
	}
	//移動後位置のセット
	void SetMovePos(const Vector3& movePos) {
		movePos_ = movePos;
	}
	//移動後回転のセット
	void SetMoveRota(const Vector3& moveRota) {
		moveRota_ = moveRota;
	}
	//シェイク後位置のセット
	void SetShakePos(const Vector3& shakePos) {
		shakePos_ = shakePos;
	}

	//スピードの取得
	float GetSpeed() {
		return speed_;
	}

	//タイマー最大値の取得
	int32_t GetTimerMax() {
		return timerMax_;
	}

	//開始イーズ始点の取得
	Ease& GetStartEase1() {
		return startEase1_;
	}
	//開始イーズ終点点の取得
	Ease& GetStartEase2() {
		return startEase2_;
	}
	//終了イーズ始点の取得
	Ease& GetEndEase1() {
		return endEase1_;
	}
	//終了イーズ終点の取得
	Ease& GetEndEase2() {
		return endEase2_;
	}
	//グループ名の取得
	void SetGroupName(const char* groupName) {
		groupName_ = groupName;
	}

	void SetHpGauge(Gauge* hpGauge) {
		hpGauge_ = hpGauge;
	}

	void SetDefaultLife(float defaultLife) {
		defaultLife_ = defaultLife;
	}

	void SetDefaultPosition(const Vector3& defaultPos) {
		defaultPos_ = defaultPos;
	}
	const Vector3& GetDefaultPosition() {
		return defaultPos_;
	}

private:
	//調整変数グループ名
	const char* groupName_ = "Character";

	//移動後位置
	Vector3 movePos_{};
	//移動後回転
	Vector3 moveRota_{};
	//シェイク後位置
	Vector3 shakePos_{};

	//タイマー最大値
	int32_t timerMax_ = 60;

	//デフォルト体力
	float defaultLife_ = 10.0f;

	//デフォルトY座標
	Vector3 defaultPos_ = {};

private: //メンバ変数
	//ゲームシーン
	GamePlayScene* gamePlayScene_ = nullptr;

	//半径
	float radiusCollider_ = 1.0f;

	//弾モデル
	Model* bulletModel_ = nullptr;

	//体力
	float life_ = defaultLife_;

	//デスフラグ
	bool isDead_ = false;

	//ダメージフラグ
	bool isDamage_ = false;

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

	//スタートフラグ
	bool isStart_ = false;
	//ゲームオーバーフラグ
	bool isOver_ = false;

	//タイマー現在値
	int32_t timerNow_ = 0;

	//開始イーズ始点
	Ease startEase1_;
	//開始イーズ終了
	Ease startEase2_;
	//終了イーズ始点
	Ease endEase1_;
	//終了イーズ終了
	Ease endEase2_;

	//スピード
	float speed_ = 0.5f;

	//カート
	Cart* cart_ = nullptr;

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
	Character() = default;
	virtual ~Character() = default;
};
