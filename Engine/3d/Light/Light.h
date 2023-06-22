#pragma once
#include "Vector3.h"
#include <wrl.h>
#include <d3d12.h>

class Light {
private: // エイリアス
	//省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public://サブクラス
	struct ConstBufferDataLight {
		Vector3 lightv_; //ライトへの方向ベクトル
		Vector3 lightcolor_;//ライトの色
	};

public://静的メンバ関数
	//静的初期化
	static void StaticInitialize(ID3D12Device* device);

	//生成
	static Light* Create();

public://メンバ関数
	void Initialize();
	void Update();
	void Draw(ID3D12GraphicsCommandList* cmdList,UINT rootParameterIndex);

	//定数バッファ生成
	void CreateConstBuffer();
	//定数バッファへのデータ転送
	void TransferConstBuffer();

	//ライト方向をセット
	void SetLightDir(const Vector3& lightdir) {
		//正規化してセット
		lightdir_ = Vector3Normalize(lightdir);
		dirty_ = true;
	}

	//ライト色をセット
	void SetLightColor(const Vector3& lightcolor) {
		lightcolor_ = lightcolor;
		dirty_ = true;
	}

private://静的メンバ変数
	//デバイス
	static ComPtr<ID3D12Device> device_;

private://メンバ変数
	//定数バッファ
	ComPtr<ID3D12Resource> constBuff_;
	//ライト光線方向(単位ベクトル)
	Vector3 lightdir_ = { 1,0,0 };
	//ライト色
	Vector3 lightcolor_ = { 1,1,1 };
	//ダーティフラグ
	bool dirty_ = false;
};