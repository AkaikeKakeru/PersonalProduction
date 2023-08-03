#pragma once
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"
#include <d3d12.h>
#include <wrl.h>
#include "MyMath.h"

//定数バッファデータ構造体(ビュープロジェクション)
struct ConstBufferDataViewProjection {
	//ビュー
	Matrix4 view_;
	//プロジェクション
	Matrix4 projection_;
	//カメラ位置
	Vector3 cameraPos_;
};

struct ViewProjection {
private: // エイリアス
		 // Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public:
	//初期化
	void Initialize();

	//定数バッファ生成
	void CreateConstBuffer();

	//マッピング
	void Maping();

	//行列の更新
	void UpdateMatrix();
	//プロジェクション行列更新
	void UpdateProjectionMatrix();
	//ビュー行列更新
	void UpdateViewMatrix();

	//転送
	void TransferMatrix();

	// ビュー行列
	Matrix4 matView_ = Matrix4Identity();
	// 射影行列
	Matrix4 matProjection_ = Matrix4Identity();

	//ビュプロジェクション行列
	Matrix4 matViewProjection_ = Matrix4Identity();

	// ビルボード行列
	Matrix4 matBillboard_ = Matrix4Identity();
	// Y軸回りビルボード行列
	Matrix4 matBillboardY_ = Matrix4Identity();

	//垂直視野角
	float angle_ = 0.0f;
	//アスペクト比
	float aspect_ = 0.0f;
	//ニアクリップ
	float nearClip_ = 0.0f;
	//ファークリップ
	float farClip_ = 0.0f;

	// 視点座標
	Vector3 eye_;
	// 注視点座標
	Vector3 target_;
	// 上方向ベクトル
	Vector3 up_;

	// 定数バッファ
	ComPtr<ID3D12Resource> constBuff_;
	// マッピング済みアドレス
	ConstBufferDataViewProjection* constMap_ = nullptr;
};