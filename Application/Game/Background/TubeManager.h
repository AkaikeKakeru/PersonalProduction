/*背景筒管理*/

#pragma once
#include "Tube.h"

#include <memory>
#include <List>

//背景管理
class TubeManager {
public: //クラス内定数
	const float kTubePopPosZ_ = 1200.0f;
	const float kTubeDeadPosZ_ = -200.0f;
	const int32_t kRePopTime_ = 10;

public: //静的メンバ関数

public://メンバ関数
	void Initialize();
	void Update();
	void Draw();
	void DrawUI();
	void DrawImgui();
	void Finalize();

	//筒を追加
	void AddTube(
		const Vector3 pos,
		const Vector3 rota,
		const Vector3 scale
	);

	//カメラのセット
	void SetCamera(Camera* camera) {
		camera_ = camera;
	}

	//筒モデルのセット
	void SetTubeModel(Model* model) {
		tubeModel_ = model;
	}

	//スピードセット
	void SetSpeed(float speed) {
		speed_ = speed;
	}

	void SetScale(const Vector3& scale) {
		scale_ = scale;
	}

private: //メンバ変数

	//背景筒用モデル
	Model* tubeModel_;

	//背景筒
	std::list<std::unique_ptr<TubeBG>> tubes_;

	//カメラ
	Camera* camera_ = nullptr;

	//スピード
	float speed_ = 0.0f;

	//再配置タイマー
	int32_t timerRePop_ = 0;

	//スケール
	Vector3 scale_ = { 0,0,0 };

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
};
