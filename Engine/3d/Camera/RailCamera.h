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

	void Finalize();

	void DrawImGui();

	Vector3 SplinePosition(
		const std::vector<Vector3>& points,
		size_t startIndex,
		float t);

	void SightNextPhasePosition();

public://定数
	//ファイナルフェーズ番号
	const size_t kFinalPhaseIndex_ = 3;

public://静的メンバ変数
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

	void SetSplinePoint(
		std::vector<Vector3> splineList,
		size_t phaseIndex) {
		points_ = {
			splineList[phaseIndex - 1],
			splineList[phaseIndex - 1],
			splineList[phaseIndex],
			splineList[phaseIndex + 1],
			splineList[phaseIndex + 2],
			splineList[phaseIndex + 2]
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

	//ゲームシーン
	GamePlayScene* gameScene_ = nullptr;;

	//次の座標
	Vector3 nextPos_;

	//スプラインで移動する座標ベクトルコンテナ
	std::vector<Vector3> splinePositions_;

	//スプラインで回転する方向ベクトルコンテナ
	std::vector<Vector3> splineDirections_;

	const float kTotalTime_ = 60.0f * 2.0f;

	float nowTime_ = 0.0f;
	float endTime_ = kTotalTime_;

	//フェーズ番号
	size_t phaseIndex_ = 1;

	//レール番号
	size_t railIndex_ = 1;

	//先頭と最後に、制御点を1個ずつ追加しておく
	std::vector<Vector3> points_;

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