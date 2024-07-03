/*カーソルやレティクルの座標を求める*/
/*カーソルやレティクルの座標を求める*/

#pragma once
#include "Vector2.h"
#include "Matrix4.h"

#include "Object3d.h"
#include "Camera.h"
#include "Ease.h"

/*カーソルやレティクルの座標を求める*/
class Cursor {
public://定数
	//ロックオンの範囲
	const float kLockOnRange_ = 50.0f;
	
	//ロックオン時、イージングタイマーの最大値
	const int32_t kEaseTimerLockOn_ = 6;
	//リリース時、イージングタイマーの最大値
	const int32_t kEaseTimerRelease_ = 3;

public: //アクセッサ

	void Initialize();


	/// <summary>
	/// 3Dレティクル位置取得
	/// </summary>
	/// <param name="camera">カメラ</param>
	/// <param name="targetWorldPos">標的のワールド座標(MatWorld.m[3][0]～m.[3][2])</param>
	/// <returns>レティクルのワールド座標</returns>
	Vector3& Get3DReticlePosition(Camera* camera,const Vector3 targetWorldPos);

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

	//ロックオン中かどうか
	bool IsLockOn() {
		return isLockOn_;
	}

	void SetCamera(const Camera& camera) {
		Camera c = camera;

		camera_ = &c;

		CreateMatrixInverseVPV();
	}

private: //固有関数
	//ビュープロジェクションビューポート行列の逆行列生成
	void CreateMatrixInverseVPV();
	//レイの方向を確認
	void CheckRayDirection();

	/// <summary>
	///ロックオン
	/// </summary>
	/// <param name="targetWorldPos">標的のワールド座標(MatWorld.m[3][0]～m.[3][2])</param>
	void LockOn(const Vector3& targetWorldPos);

	/// <summary>
	///ロックオン
	/// </summary>
	/// <param name="targetWorldPos">標的のワールド座標(MatWorld.m[3][0]～m.[3][2])</param>
	void LockOnBef(const Vector3& targetWorldPos);

	/// <summary>
	/// イージングで位置を移動させる
	/// </summary>
	void EasePosition();

private: //メンバ変数
	//カメラ
	Camera* camera_;

	//ビュープロジェクションビューポート行列
	Matrix4 matVPV_ = Matrix4Identity();

	//ビュープロジェクションビューポートの逆行列
	Matrix4 matInverseVPV_ = Matrix4Identity();

	//レティクル位置
	Vector3 reticlePos_ = {};

	//レティクルの移動先保管用
	Vector3 reticleMove_ = {};

	//カメラからの距離
	float distance_ = 50.0f;

	//イージング
	Ease ease_;

	//イージング開始地点
	Vector3 easeStartPos_{};
	//イージング終了地点
	Vector3 easeEndPos_{};

	//ロックオンフラグ
	bool isLockOn_ = false;

	//イージング待機中かどうか
	bool isStand_ = true;
};
