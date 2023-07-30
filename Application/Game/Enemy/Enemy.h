#pragma once
#include "Model.h"
#include "Object3d.h"
#include "Sprite.h"
#include "SpriteBasis.h"
#include <Input.h>

#include <list>
#include <memory>

class GamePlayScene;
class CollisionManager;

//エネミー
class Enemy
	: public Object3d {
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
	static const int kFireInterval = 60;

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

	//半径
	float radius_ = 1.0f;

	//3dレティクルのワールド変換
	WorldTransform worldTransform3dReticle_;

	//レティクル用スプライト
	Sprite* spriteReticle_ = nullptr;

	//弾モデル
	Model* bulletModel_ = nullptr;

	//デスフラグ
	bool isDead_ = false;

	//発射タイマー
	int32_t fireTimer_ = 0;

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