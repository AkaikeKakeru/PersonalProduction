/*プレイヤー*/

#pragma once

#include "Character.h"
#include "RailCamera.h"
#include "Shake.h"

class GamePlayScene;
class PlayerBullet;
class Enemy;
class CollisionManager;

//プレイヤー
class Player
	: public Character {
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

	//調整項目の適用
	void ApplyAdjustmentVariables();

	//衝突時コールバック関数
	void OnCollision(const CollisionInfo& info) override;

	//照準更新
	void UpdateReticle(Enemy* enemy);

	//発射攻撃
	void Attack() override;

	//開始時移動
	void StartMove() override;

	//脱落時移動
	void OverMove() override;

	void PhaseChange();

public: //定数
	//調整変数グループ名
	const char* groupName_ = "Player";

	//自機のデフォルト体力
	const float kDefaultLife_ = 10.0f;

	//HP用イージング最大時間
	const float kMaxTimeHP_ = 30.0f;

	//銃ダメージ量
	const float kGunDamage_ = 3.0f;

	//弾数の上限
	const int kBulletRimit_ = 30;

	//デフォルトクールタイム
	const int kDefaultBulletCooltime_ = 1;

	//デフォルトX座標
	const float kDefaultPosX_ = 0.0f;
	//デフォルトY座標
	const float kDefaultPosY_ = -5.0f;
	//デフォルトZ座標
	const float kDefaultPosZ_ = 30.0f;

public: //アクセッサ
	//追従カメラのビュープロジェクション
	void SetViewProjection(const ViewProjection* viewProjection) {
		viewProjection_ = viewProjection;
	}

	//レールカメラのワールド変換取得
	void SetWorldTransformRailCamera(WorldTransform* worldTransformRailCamera) {
		worldTransform_.parent_ = worldTransformRailCamera;
	}

	//隠れフラグの取得
	bool IsHide() {
		return isHide_;
	}

	//隠れフラグのセット
	void SetIsHide(bool isHide) {
		isHide_ = isHide;
	}

	//ポージングモデルのセット
	void SetModelPauses(Model* active, Model* hyde) {
		modelActive_ = active;
		modelHide_ = hyde;
	};

	//フェーズ進行フラグのセット
	void SetPhaseAdvance(bool isPhaseAdvance) {
		isPhaseAdvance_ = isPhaseAdvance;
	}

private: //静的メンバ変数
	//衝突マネージャー
	static CollisionManager* collisionManager_;
	//入力
	static Input* input_;
	//スプライト基盤
	static SpriteBasis* spriteBas_;

private: //メンバ変数
	//カメラのビュープロジェクション
	const ViewProjection* viewProjection_ = nullptr;

private: //メンバ変数
	//移動イーズ
	Ease moveEase{};
	//回転イーズ
	Ease rotaEase{};

	//テキスト
	std::unique_ptr<Text> textEmpty_ = nullptr;

	//カーソル
	//std::unique_ptr<Cursor> cursor{};
	Cursor cursor_;

	//3dレティクルのワールド変換
	WorldTransform worldTransform3dReticle_;

	//隠れフラグ
	bool isHide_ = false;

	//残弾数
	int remainBulletCount_ = kBulletRimit_;

	//弾のクールタイム
	int32_t bulletCooltime_ = kDefaultBulletCooltime_;

	//レティクル用スプライト
	std::unique_ptr<Sprite> spriteReticle_ = nullptr;

	//HPゲージのセットアップ位置
	Ease setupHpGaugePos_ = {};

	/// <summary>
	/// 残弾
	/// </summary>
	std::unique_ptr<Gauge> bulletGauge_ = {};

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

	//オーバー位置用イーズを、セットアップしたかのフラグ
	bool isSetupOverPositionEase_ = false;

	Model* modelActive_ = nullptr;
	Model* modelHide_ = nullptr;

	float cartPosYLock_ = {};
	bool isHideOld_ = false;

	//フェーズ進行フラグ
	bool isPhaseAdvance_ = false;

	//カーソル
	Vector3 cursorPos_{};
	//今受け取っているターゲットのワールド座標
	Vector3 targetWorldPos_{};
	
public:
	Player() = default;
	~Player() = default;
};
