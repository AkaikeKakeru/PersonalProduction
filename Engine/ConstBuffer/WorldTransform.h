#pragma once
#include "Vector3.h"
#include "Matrix4.h"
#include <d3d12.h>
#include <wrl.h>

// 定数バッファデータワールド変換
struct ConstBufferDataWorldTransform{
	Matrix4 viewproj_;//ビュープロジェクション行列
	Matrix4 world_;//ワールド行列
	Vector3 cameraPos_;//カメラ座標(ワールド座標)
};

struct WorldTransform {
private: // 省略
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public:
	// 定数バッファ
	ComPtr<ID3D12Resource> constBuff_;
	// マッピング済みアドレス
	ConstBufferDataWorldTransform* constMap_ = nullptr;
	// スケール
	Vector3 scale_ = {1, 1, 1};
	// 回転角
	Vector3 rotation_ = {0, 0, 0};
	// 座標
	Vector3 position_ = {0, 0, 0};
	// ワールド変換行列
	Matrix4 matWorld_ = {};
	// 親へのポインタ
	WorldTransform* parent_ = nullptr;

	// 初期化
	void Initialize();
	// 定数バッファ生成
	void CreateConstBuffer();
	// マッピング
	void Maping();
	// 行列を更新
	void UpdateMatrix();
};
