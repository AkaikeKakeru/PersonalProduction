/*追従カメラ*/

#pragma once
#include "Camera.h"
#include "WorldTransform.h"
#include "ViewProjection.h"

#include "Input.h"
#include <vector>

class GamePlayScene;

//追従カメラ
class FollowCamera
	: public Camera {
public://メンバ関数
	void Initialize();
	void Update();
	void Finalize();
	void DrawImGui();

public://定数
	//初期座標
	const Vector3 startPos_{ 0,0,0 };
	//初期回転
	const Vector3 startRota_{ 0,0,0 };
	//オフセットのデフォルト値
	const Vector3 defaultOffSetPos_{ 0.0f,5.0f,-50.0f };

public://静的メンバ変数
	static Input* input_;

public://アクセッサ
	//追従対象のセット
	void SetTargetWorldTransform(const WorldTransform* target) {
		target_ = target;
	}

	//ワールド変換の取得
	WorldTransform* GetWorldTransform() {
		return &worldTransform_;
	}

private://メンバ変数
	//追従対象
	const WorldTransform* target_ = nullptr;

private://メンバ変数
	//ワールド変換
	WorldTransform worldTransform_;

	//回転速度
	Vector3 rotSpeed_;

	//オフセット
	Vector3 offset_;

private: //ImGui用
	//Vector3の要素数
	static const int kVector3Count_ = 3;

	//Pos範囲
	const float PosRange_ = 30.0f;

	//Dir範囲
	const float DirRange_ = ConvertToRadian(360.0f);

	//ImGui用レールカメラPos
	float debugPos_[kVector3Count_] = {};

	//ImGui用レールカメラDir
	float debugDir_[kVector3Count_] = {};
};
