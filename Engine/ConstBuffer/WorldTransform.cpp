#include "WorldTransform.h"
#include "DirectXBasis.h"
#include <cassert>

void WorldTransform::Initialize() {
	constMap_ = nullptr;
	scale_ = { 1, 1, 1 };
	rotation_ = { 0, 0, 0 };
	position_ = { 0, 0, 0 };
	matWorld_ = {};
	parent_ = nullptr;

	CreateConstBuffer();
	Maping();
	UpdateMatrix();
}

void WorldTransform::CreateConstBuffer() {
	HRESULT result;

	// ヒーププロパティ
	D3D12_HEAP_PROPERTIES heapProps{};
	heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

	// リソース設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = (sizeof(ConstBufferDataWorldTransform) + 0xff) & ~0xff;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// 定数バッファの生成
	result = DirectXBasis::GetInstance()->GetDevice()->
		CreateCommittedResource(
		&heapProps, // アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff_));
	assert(SUCCEEDED(result));
}

void WorldTransform::Maping() {
	// 定数バッファとのデータリンク
	HRESULT result = constBuff_->Map(0, nullptr, (void**)&constMap_);
	assert(SUCCEEDED(result));
}

void WorldTransform::UpdateMatrix() {
	Matrix4 matScale, matRot, matTrans;

	// スケール、回転、平行移動行列の計算
	matScale = Matrix4Scale(scale_);
	matRot = Matrix4Identity();
	matRot = Matrix4Rotation(rotation_);
	matTrans = Matrix4Translation(position_);

	// ワールド行列の合成
	matWorld_ = Matrix4Identity(); // 変形をリセット
	matWorld_ *= matScale;          // ワールド行列にスケーリングを反映
	matWorld_ *= matRot;            // ワールド行列に回転を反映
	matWorld_ *= matTrans;          // ワールド行列に平行移動を反映

	// 親行列の指定がある場合は、掛け算する
	if (parent_) {
		matWorld_ *= parent_->matWorld_;
	}
}