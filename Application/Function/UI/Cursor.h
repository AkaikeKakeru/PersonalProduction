/*カーソルやレティクルの座標を求める*/

#pragma once
#include "Vector2.h"
#include "Matrix4.h"

#include "Object3d.h"
#include "Camera.h"

/*カーソルやレティクルの座標を求める*/
class Corsor {
public://定数
	//ロックオンの範囲
	const float kLockOnRange_ = 80.0f;
	//イージングタイマーの最大値
	const int32_t kEaseTimer_ = 60 / 2;

public: //アクセッサ
	/// <summary>
	/// 3Dレティクル位置取得
	/// </summary>
	/// <param name="camera">カメラ</param>
	/// <param name="targetWorldPos">標的のワールド座標(MatWorld.m[3][0]～m.[3][2])</param>
	/// <returns>レティクルのワールド座標</returns>
	Vector3& Get3DReticlePosition(Camera* camera, const Vector3 targetWorldPos);

	/// <summary>
	///スクリーン座標から、ワールド座標へ変換 
	/// </summary>
	/// <param name="screenPos">スクリーン座標</param>
	/// <returns>変換後のワールド座標</returns>
	Vector3& TransFromScreenToWorld(const Vector2& screenPos);

	/// <summary>
	/// ワールド座標から、スクリーン座標へ変換
	/// </summary>
	/// <param name="worldPos">ワールド座標</param>
	/// <returns>変換後のスクリーン座標</returns>
	Vector2& TransFromWorldToScreen(const Vector3& worldPos);

	/// <summary>
	/// 距離のセット
	/// </summary>
	/// <param name="distance">距離</param>
	void SetDistance(float distance) {
		distance_ = distance;
	}

private: //固有関数
	//ビュープロジェクションビューポート行列生成
	void CreateMatrixVPV();
	//ビュープロジェクションビューポート行列の逆行列生成
	void CreateMatrixInverseVPV();
	//レイの方向を確認
	void CheckRayDirection();

	/// <summary>
	///ロックオン
	/// </summary>
	/// <param name="targetWorldPos">標的のワールド座標(MatWorld.m[3][0]～m.[3][2])</param>
	void LockOn(const Vector3& targetWorldPos);

private: //メンバ変数
	//カメラ
	static Camera* camera_;

	//ビュープロジェクションビューポート行列
	Matrix4 matVPV_ = Matrix4Identity();
	//ビュープロジェクションビューポートの逆行列
	Matrix4 matInverseVPV_ = Matrix4Identity();

	//レティクル位置
	Vector3 reticlePos_ = {};

	//カメラからの距離
	float distance_ = 50.0f;

};
