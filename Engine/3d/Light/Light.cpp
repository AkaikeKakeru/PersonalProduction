#include "Light.h"
#include <cassert>
#include "DirectXBasis.h"
//省略
template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

ComPtr<ID3D12Device> Light::device_ = nullptr;

void Light::StaticInitialize(ID3D12Device* device) {
	//再初期化チェック
	assert(!Light::device_);
	//nullptrチェック
	assert(device);

	//静的メンバ変数セット
	Light::device_ = device;
}

Light* Light::Create() {
	Light* light = new Light();

	light->Initialize();

	return light;
}

void Light::Initialize() {
	CreateConstBuffer();
	TransferConstBuffer();
}

void Light::Update() {
	if (dirty_) {
		TransferConstBuffer();
		dirty_ = false;
	}
}

void Light::Draw(ID3D12GraphicsCommandList* cmdList, UINT rootParameterIndex) {
	cmdList->SetGraphicsRootConstantBufferView(
		rootParameterIndex, constBuff_->GetGPUVirtualAddress());
}


void Light::CreateConstBuffer() {
	HRESULT result;

	// ヒーププロパティ
	D3D12_HEAP_PROPERTIES heapProps{};
	heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

	// リソース設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = (sizeof(ConstBufferDataLight) + 0xff) & ~0xff;
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

void Light::TransferConstBuffer() {
	HRESULT result;

	ConstBufferDataLight* constMap = nullptr;
	result = constBuff_->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result)) {
		constMap->lightv_ = -lightdir_;
		constMap->lightcolor_ = lightcolor_;
		constBuff_->Unmap(0, nullptr);
	}
}
