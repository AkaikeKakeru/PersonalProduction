#pragma once
#include "WorldTransform.h"
#include "ViewProjection.h"

#include "Input.h"
#include <vector>

class GamePlayScene;

//レールカメラ
class RailCamera {
public://メンバ関数
	void Initialize(const Vector3 pos,
		const Vector3 rotaRadianAngle);
	void Update();

	void DrawImGui();

	Vector3 SplinePosition(
		const std::vector<Vector3>& points,
		size_t startIndex,
		float t);
public:
	static Input* input_;

public://アクセッサ
	// 視点座標の取得
	const Vector3& GetEye() {
		return viewProjection_.eye_; }
	// 注視点座標の取得 
	const Vector3& GetTarget() {
		return viewProjection_.target_; }
	// 上方向ベクトルの取得
	const Vector3& GetUp() {
		return viewProjection_.up_; }
	//ワールド変換の取得
	WorldTransform* GetWorldTransform() {
		return &worldTransform_;
	}

	// 視点座標の取得
	void SetEye(const Vector3& eye) {
		viewProjection_.eye_ = eye; }
	// 注視点座標の取得 
	void SetTarget(const Vector3& target) {
		viewProjection_.target_ = target; }
	// 上方向ベクトルの取得
	void SetUp(const Vector3& up) {
		viewProjection_.up_ = up; }

	//スプライン中間点のセット
	void SetSplinePoint(
		Vector3 p0,
		Vector3 p1,
		Vector3 p2,
		Vector3 p3 ) {
		points_ = {
			p0,p0,p1,p2,p3,p3
		};
	}

	//フェーズ進行フラグのセット
	void SetPhaseAdvance(bool isPhaseAdvance) {
		isPhaseAdvance_ = isPhaseAdvance;
	}

	//ゲームシーンのセット
	void SetGameScene(GamePlayScene* gameScene) {
		gameScene_ = gameScene;
	}

private://メンバ変数
	//ワールド変換
	WorldTransform worldTransform_;
	//ビュープロジェクション
	ViewProjection viewProjection_;

	GamePlayScene* gameScene_ = nullptr;;

	Vector3 splinePosStart_ = {};
	Vector3 splinePos1_ = {};
	Vector3 splinePos2_ = {};
	Vector3 splinePosEnd_ = {};

	Vector3 splineDirStart_ = {};
	Vector3 splineDir1_ = {};
	Vector3 splineDir2_ = {};
	Vector3 splineDirEnd_ = {};

	const float kTotalTime_ = 60.0f * 5;

	float nowTime_ = 0.0f;
	float endTime_ = kTotalTime_;

	//フェーズ番号
	size_t phaseIndex_ = 1;

	//先頭と最後に、制御点を1個ずつ追加しておく
	std::vector<Vector3> points_{
		splinePosStart_,
		splinePosStart_,
		splinePos1_,
		splinePos2_,
		splinePosEnd_,
		splinePosEnd_
	};

	//フェーズ進行フラグ
	bool isPhaseAdvance_ = false;

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