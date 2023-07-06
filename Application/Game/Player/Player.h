#pragma once
#include "Model.h"
#include "Object3d.h"
#include "RailCamera.h"
#include "Sprite.h"
#include "DrawBasis.h"
#include "PlayerBullet.h"
#include <Input.h>

#include <list>
#include <memory>

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

private: //静的メンバ変数
	//衝突マネージャー
	static CollisionManager* collisionManager_;
	//入力
	static Input* input_;
	//スプライト基盤
	static DrawBasis* drawBas_;

private: //メンバ変数
	//半径
	float radius_ = 1.0f;

	//3dレティクルのワールド変換
	WorldTransform worldTransform3dReticle_;

	//レティクル用スプライト
	Sprite* spriteReticle_ = nullptr;

	//弾
	std::list<std::unique_ptr<PlayerBullet>> bullets_;

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