#pragma once
#include "Vector2.h"
#include "Matrix4.h"

#include "Object3d.h"
#include "Camera.h"

class Corsor {
public: //アクセッサ
	//3D照準位置取得
	Vector3 Get3DRethiclePosition(Camera* camera);

	//距離のセット
	void SetDistance(float distance) {
		distance_ = distance;
	}

private: //固有関数
	//ビューポート行列の逆行列生成
	void CreateMatrixInverseViewPort();
	//レイの方向を確認
	void CheckRayDirection();

private: //メンバ変数
	//カメラ
	static Camera* camera_;
	//ビュープロジェクションビューポートの逆行列
	Matrix4 matInverseVPV_ = Matrix4Identity();

	//ニア位置
	Vector3 posNear_ = {};
	//ファー位置
	Vector3 posFar_ = {};
	//レイの方向
	Vector3 rayDirection_ = {};

	//カメラからの距離
	float distance_ = 50.0f;
};