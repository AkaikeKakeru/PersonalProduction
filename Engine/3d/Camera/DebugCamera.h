#pragma once
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Input.h"

//デバッグカメラ
class DebugCamera {
public://静的メンバ変数
	static Input* input_;

public:
	void Initialize(const Vector3 pos,const Vector3 rotaRadianAngle);
	void Update();

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

private:
	//ワールド変換
	WorldTransform worldTransform_;
	//ビュープロジェクション
	ViewProjection viewProjection_;

};