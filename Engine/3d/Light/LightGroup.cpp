#include "LightGroup.h"
#include <cassert>
#include "DirectXBasis.h"
//省略
template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

ComPtr<ID3D12Device> LightGroup::device_ = nullptr;

void LightGroup::StaticInitialize(ID3D12Device* device) {
	//再初期化チェック
	assert(!LightGroup::device_);
	//nullptrチェック
	assert(device);

	//静的メンバ変数セット
	LightGroup::device_ = device;
}

LightGroup* LightGroup::Create() {
	LightGroup* light = new LightGroup();

	light->Initialize();

	return light;
}

void LightGroup::Initialize() {
	//標準のライトの設定
	DefaultSetting();
	//定数バッファ生成
	CreateConstBuffer();
	//定数バッファへ転送
	TransferConstBuffer();
}

void LightGroup::Update() {
	//値の変更があったら定数バッファへ転送
	if (dirty_) {
		TransferConstBuffer();
		dirty_ = false;
	}
}

void LightGroup::Draw(ID3D12GraphicsCommandList* cmdList, UINT rootParameterIndex) {
	cmdList->SetGraphicsRootConstantBufferView(
		rootParameterIndex, constBuff_->GetGPUVirtualAddress());
}


void LightGroup::CreateConstBuffer() {
	HRESULT result;

	// ヒーププロパティ
	D3D12_HEAP_PROPERTIES heapProps{};
	heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

	// リソース設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = (sizeof(ConstBufferData) + 0xff) & ~0xff;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// 定数バッファの生成
	result = DirectXBasis::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProps, // アップロード可能
		D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&constBuff_));
	assert(SUCCEEDED(result));
}

void LightGroup::TransferConstBuffer() {
	HRESULT result;

	ConstBufferData* constMap = nullptr;
	result = constBuff_->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result)) {
		constMap->ambientColor_ = ambientColor_;

		for (int i = 0; i < DirLightNum; i++) {
			//ライトが有効なら設定を転送
			if (dirLights_[i].IsActive()) {
				constMap->dirLights[i].active_ = 1;
				constMap->dirLights[i].lightv_ = -dirLights_[i].GetLightDir();
				constMap->dirLights[i].lightcolor_ = dirLights_[i].GetLightColor();
			}
			//ライトが無効なら転送しない
			else {
				constMap->dirLights[i].active_ = 0;
			}
		}
	}
}

void LightGroup::DefaultSetting() {
	dirLights_[0].SetActive(true);
	dirLights_[0].SetLightColor({ 1.0f, 1.0f, 1.0f });
	dirLights_[0].SetLightDir({ 0.0f,-1.0f,0.0f });

	dirLights_[1].SetActive(true);
	dirLights_[1].SetLightColor({ 1.0f, 1.0f, 1.0f });
	dirLights_[1].SetLightDir({ +0.5f,+0.1f,+0.2f });

	dirLights_[2].SetActive(true);
	dirLights_[2].SetLightColor({ 1.0f, 1.0f, 1.0f });
	dirLights_[2].SetLightDir({ -0.5f,+0.1f,-0.2f });
}

void LightGroup::SetDirLightActive(int index, bool active) {
	assert(0 <= index && index < DirLightNum);
	dirLights_[index].SetActive(active);
}

void LightGroup::SetDirLightDir(int index, const Vector3& lightdir) {
	assert(0 <= index && index < DirLightNum);
	dirLights_[index].SetLightDir(lightdir);
	dirty_ = true;
}

void LightGroup::SetDirLightColor(int index, const Vector3& lightcolor) {
	assert(0 <= index && index < DirLightNum);
	dirLights_[index].SetLightColor(lightcolor);
	dirty_ = true;
}
