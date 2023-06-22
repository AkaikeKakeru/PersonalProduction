#pragma once

#include "Matrix4.h"
#include "Vector3.h"
#include "ViewProjection.h"

/// <summary>
/// カメラ基本機能
/// </summary>
class Camera {
protected:

public: // メンバ関数
	Camera();

	virtual ~Camera();

	virtual void Update();

	// ビュー行列の取得
	inline const Matrix4& GetViewMatrix() { return viewProjection_.matView_; }

	// 射影行列の取得
	const Matrix4& GetProjectionMatrix() { return viewProjection_.matProjection_; }

	// ビュー射影行列の取得
	const Matrix4& GetViewProjectionMatrix() { return matViewProjection_; }

	// ビルボード行列の取得
	const Matrix4& GetBillboardMatrix() { return viewProjection_.matBillboard_; }

	// 視点座標の取得
	const Vector3& GetEye() { return viewProjection_.eye_; }

	// 視点座標の設定
	void SetEye(Vector3 eye) {
		viewProjection_.eye_ = eye;

		viewDirty_ = true;
	}

	// 注視点座標の取得 
	const Vector3& GetTarget() { return viewProjection_.target_; }

	// 注視点座標の設定
	inline void SetTarget(Vector3 target) {
		viewProjection_.target_ = target;

		viewDirty_ = true;
	}

	// 上方向ベクトルの取得
	const Vector3& GetUp() { return viewProjection_.up_; }

	// 上方向ベクトルの設定
	void SetUp(Vector3 up) {
		viewProjection_.up_ = up;
		viewDirty_ = true;
	}

	// ベクトルによる移動
	void MoveVector(const Vector3& move);

	// ベクトルによる視点移動
	void MoveEyeVector(const Vector3& move);

protected: // メンバ変数
	// ビュー射影行列
	Matrix4 matViewProjection_ = Matrix4Identity();
	// ビュー行列ダーティフラグ
	bool viewDirty_ = false;
	// 射影行列ダーティフラグ
	bool projectionDirty_ = false;

	//ビュープロジェクション
	ViewProjection viewProjection_;
};
