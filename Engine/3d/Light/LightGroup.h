#pragma once
#include "Vector3.h"
#include <wrl.h>
#include <d3d12.h>
#include "DirectionalLight.h"

class LightGroup {
private: // エイリアス
		 //省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public://定数
	static const int DirLightNum = 3;

public://サブクラス
	   //定数バッファ用データ構造体
	struct ConstBufferData {
		//環境光の色
		Vector3 ambientColor_;
		float pad1;
		//平行光源用
		DirectionalLight::ConstBufferData dirLights[DirLightNum];
	};

public://静的メンバ関数
	   //静的初期化
	static void StaticInitialize(ID3D12Device* device);

	//生成
	static LightGroup* Create();

public://メンバ関数
	void Initialize();
	void Update();
	void Draw(ID3D12GraphicsCommandList* cmdList, UINT rootParameterIndex);

	//定数バッファ生成
	void CreateConstBuffer();
	//定数バッファへのデータ転送
	void TransferConstBuffer();

	//標準のライト設定
	void DefaultSetting();


	//環境光のライト色をセット
	void SetAmbientColor(const Vector3& color) {
		ambientColor_ = color;
		dirty_ = true;
	}

	//平行光源の有効フラグをセット
	void SetDirLightActive(int index, bool active);

	//ライト方向をセット
	void SetDirLightDir(int index, const Vector3& lightdir);

	//ライト色をセット
	void SetDirLightColor(int index, const Vector3& lightcolor);

private://静的メンバ変数
	//デバイス
	static ComPtr<ID3D12Device> device_;

private://メンバ変数
	//定数バッファ
	ComPtr<ID3D12Resource> constBuff_;
	//環境光の色
	Vector3 ambientColor_ = { 1,1,1 };
	//平行光源の配列
	DirectionalLight dirLights_[DirLightNum];
	//ダーティフラグ
	bool dirty_ = false;
};