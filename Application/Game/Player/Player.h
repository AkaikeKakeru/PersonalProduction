#pragma once
#include "Model.h"
#include "Object3d.h"
#include "RailCamera.h"
#include "Sprite.h"
#include "SpriteBasis.h"
#include <Input.h>

#include <list>
#include <memory>

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
	//ダメージ量
	const float kDamage_ = 3.0f;

public: //アクセッサ
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

private: //静的メンバ変数
	//衝突マネージャー
	static CollisionManager* collisionManager_;
	//入力
	static Input* input_;
	//スプライト基盤
	static SpriteBasis* spriteBas_;

private: //メンバ変数
	//半径
	float radius_ = 1.0f;

	//3dレティクルのワールド変換
	WorldTransform worldTransform3dReticle_;

	//レティクル用スプライト
	Sprite* spriteReticle_ = nullptr;

	//弾
	std::list<std::unique_ptr<PlayerBullet>> bullets_;

	//弾モデル
	Model* bulletModel_ = nullptr;

	//体力
	float life_ = kDefaultPlayerLife_;

	//デスフラグ
	bool isDead_ = false;

	//ダメージフラグ
	bool isDamage_ = false; 

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