#pragma once

#include <d3d12.h>
#include <string>
#include <wrl.h>
#include "Vector3.h"
//using namespace DirectX;

class Material {
private: // エイリアス
	//省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;


public: //サブクラス
		//マテリアル
	struct ConstBufferDataMaterial {
		Vector3 ambient; //アンビエント影響度
		float pad1;        // パディング
		Vector3 diffuse; //ディフューズ影響度
		float pad2;        // パディング
		Vector3 specular; //スペキュラー影響度
		float alpha; //アルファ
		};

public: //静的メンバ関数
	//静的初期化
	static void StaticInitialize(ID3D12Device* device);
	//生成
	static Material* Create();

private: // 静的メンバ変数
		 // デバイス
	static ComPtr<ID3D12Device> device_;

public:
	std::string name_;            // マテリアル名
	Vector3 ambient_;            // アンビエント影響度
	Vector3 diffuse_;            // ディフューズ影響度
	Vector3 specular_;           // スペキュラー影響度
	float alpha_;                 // アルファ
	std::string filename_; // テクスチャファイル名

public:
	void Update();

	// 定数バッファの取得
	ID3D12Resource* GetConstantBuffer() { return constBuff_.Get(); }

	//テクスチャ読み込み
	void LoadTexture(const std::string& directoryPath,
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle);

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetCpuHandle() { return cpuDescHandleSRV_; }
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGpuHandle() { return gpuDescHandleSRV_; }

private:
	// テクスチャバッファ
	ComPtr<ID3D12Resource> texbuff_;
	// 定数バッファマテリアル
	ComPtr<ID3D12Resource> constBuff_;
	// シェーダリソースビューのハンドル(CPU)
	D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV_;
	// シェーダリソースビューのハンドル(GPU)
	D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV_;
	//定数バッファマップ
	ConstBufferDataMaterial* constMap_ = nullptr;

private:
	// コンストラクタ
	Material() {
		ambient_ = {0.3f, 0.3f, 0.3f};
		diffuse_ = {0.0f, 0.0f, 0.0f};
		specular_ = {0.0f, 0.0f, 0.0f};
		alpha_ = 1.0f;
	}

	void Initialize();
	// 定数バッファの生成
	void CreateConstBuffer();
};
