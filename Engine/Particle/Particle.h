#pragma once

#include "Vector4.h"
#include "Vector3.h"
#include "Vector2.h"

#include <wrl.h>
#include <d3d12.h>

#include <forward_list>
#include <string>

//パーティクルクラス
class Particle {
private://省略
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public://サブクラス
	//頂点の位置とUV
	typedef struct VertexPosUv {
		Vector3 position_; // xyz座標
		float uv_;  //uv
	}Vertex;

	//上下左右
	typedef struct TopBottomLeftRight {
		float left;//左
		float right;//右
		float top;//上
		float bottom;//下
	}Direction;

	//頂点部位
	typedef enum VerticesParts {
		LeftBottom,//左下
		LeftTop,//左上
		RightBottom,//右下
		RightTop,//右上
	}VerticesParts;

	//パーティクル一粒
	struct OneParticle {
		//座標
		Vector3 position_ = {};
		//速度
		Vector3 velocity_ = {};
		//加速度
		Vector3 accel_ = {};
		//現在frame
		int frameNow_ = 0;
		//終了frame
		int frameEnd_ = 0;

		//スケール
		float scale_ = 1.0f;
		//初期値
		float scaleStart_ = 1.0f;
		//最終値
		float scaleEnd_ = 0.0f;
	};

private://定数
	//頂点数
	static const int kVerticesNum = 4;

public://メンバ関数
	//OBJモデルを読み込み
	static Particle* LoadFromObjModel(const std::string& fileName);
	// デスクリプタヒープの初期化
	void InitializeDescriptorHeap();
	// テクスチャ読み込み
	void LoadTexture(const std::string& fileName);

	//各種バッファ生成
	void CreateBuffers();
	//更新
	void Update();
	// 描画
	void Draw(ID3D12GraphicsCommandList* cmdList);

	//パーティクルの追加
	void Add(int life, Vector3 position, Vector3 velocity, Vector3 accel,
		float start_scale, float end_scale);

public://アクセッサ
	//デバイスのセット
	static void SetDevice(ID3D12Device* device) {
		Particle::device_ = device;
	}

public://静的メンバ変数
	// デバイス
	static ComPtr<ID3D12Device> device_;
	//デフォルトディレクトリ
	static std::string defaultDirectoryPath_;

	//インクリメントサイズ
	static UINT incrementSize_;
	//デスクリプタヒープ
	static ComPtr<ID3D12DescriptorHeap> srvDescHeap_;

private://メンバ変数
	//上下左右
	Direction dir_;
	//色
	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };
	//表示サイズ
	Vector2 size_ = { 100.0f,100.0f };
	//アンカーポイント
	Vector2 anchorPoint_ = { 0.5f,0.5f };

	// テクスチャバッファ
	ComPtr<ID3D12Resource> texBuff_;
	// シェーダリソースビューのハンドル(CPU)
	D3D12_CPU_DESCRIPTOR_HANDLE srvCpuDescHandle_;
	// シェーダリソースビューのハンドル(CPU)
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuDescHandle_;

	/// <summary>
	/// 頂点バッファ
	/// </summary>

	//頂点データ
	Vertex vertices_[kVerticesNum]{};
	//頂点データマップ
	Vertex* vertMap_ = nullptr;
	//頂点バッファ
	ComPtr<ID3D12Resource> vertBuff_;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_{};

	//パーティクルのコンテナリスト
	std::forward_list<OneParticle> particles_;
};