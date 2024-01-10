/*背景筒*/

#pragma once
#include "Model.h"
#include "Object3d.h"

//背景筒
class TubeBG
	: public Object3d {
public: //静的メンバ関数
	//オブジェクト生成
	static TubeBG* Create(Model* model = nullptr);

public://メンバ関数
	bool Initialize() override;
	void Update() override;
	void Draw();
	void DrawUI();
	void DrawImgui();
	void Finalize();

	//衝突時コールバック関数
	void OnCollision(const CollisionInfo& info) override;

public: //定数

public: //アクセッサ
	const Vector3& GetPosition() const {
		return worldTransform_.position_;
	}

	//デスフラグの取得
	bool IsDead() {
		return isDead_;
	}

	//デスフラグのセット
	void SetIsDead(bool isDead) {
		isDead_ = isDead;
	}

	//スピードセット
	void SetSpeed(float speed) {
		speed_ = speed;
	}

private: //静的メンバ変数

private: //メンバ変数
	//デスフラグ
	bool isDead_ = false;

	//スピード
	float speed_ = 0.0f;

	bool isStart_ = false;

	int32_t timer_ = 10;

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
	TubeBG() = default;
	~TubeBG() = default;
};
