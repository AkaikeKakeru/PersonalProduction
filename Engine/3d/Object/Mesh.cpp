#include "Mesh.h"
#include <cassert>
//省略
template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

//実体
ComPtr<ID3D12Device> Mesh::device_ = nullptr;

void Mesh::StaticInitialize(ID3D12Device* device) {
	Mesh::device_ = device;

	// マテリアルの静的初期化
	Material::StaticInitialize(device);
}

void Mesh::CreateBuffers() {
	HRESULT result = S_FALSE;

	UINT sizeVB = static_cast<UINT>(sizeof(VertexPosNormalUv) * vertices_.size());

	// ヒーププロパティ
	D3D12_HEAP_PROPERTIES heapProps{};
	heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

	// リソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// 頂点バッファ生成
	result = device_->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&vertBuff_));
	assert(SUCCEEDED(result));

	// 頂点バッファへのデータ転送
	VertexPosNormalUv* vertMap = nullptr;
	result = vertBuff_->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		std::copy(vertices_.begin(), vertices_.end(), vertMap);
		vertBuff_->Unmap(0, nullptr);
	}

	// 頂点バッファビューの作成
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	vbView_.SizeInBytes = sizeVB;
	vbView_.StrideInBytes = sizeof(vertices_[0]);

	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * indices_.size());

	// リソース設定
	resDesc.Width = sizeIB;

	// インデックスバッファ生成
	result = device_->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&indexBuff_));

	// インデックスバッファへのデータ転送
	unsigned short* indexMap = nullptr;
	result = indexBuff_->Map(0, nullptr, (void**)&indexMap);
	if (SUCCEEDED(result)) {
		std::copy(indices_.begin(), indices_.end(), indexMap);
		indexBuff_->Unmap(0, nullptr);
	}

	// インデックスバッファビューの作成
	ibView_.BufferLocation = indexBuff_->GetGPUVirtualAddress();
	ibView_.Format = DXGI_FORMAT_R16_UINT;
	ibView_.SizeInBytes = sizeIB;
}

void Mesh::CalculateSmoothVertexNormals() {
	auto itr = smoothData.begin();

	for (; itr != smoothData.end(); itr++) {
		//各面用の共通頂点コレクション
		std::vector<unsigned short>& v = itr->second;
		//全頂点の法線を平均する
		Vector3 normal = {};
		for (unsigned short index : v) {
			normal += vertices_[index].normal;
		}

		normal = Vector3Normalize(normal / (float)v.size());
		//共通法線を使用するすべての頂点データに書き込む
		for (unsigned short index : v) {
			vertices_[index].normal = normal;
		}
	}
}

void Mesh::Draw(ID3D12GraphicsCommandList* cmdList) {
	// 頂点バッファの設定
	cmdList->IASetVertexBuffers(0, 1, &vbView_);
	// インデックスバッファの設定
	cmdList->IASetIndexBuffer(&ibView_);

	// シェーダリソースビューをセット
	cmdList->SetGraphicsRootDescriptorTable(2, material_->GetGpuHandle());
	if (true) {

	}
	// 定数バッファビューマテリアルをセット
	ID3D12Resource* constBuff = material_->GetConstantBuffer();

	cmdList->SetGraphicsRootConstantBufferView(1,
		constBuff->GetGPUVirtualAddress());

	// 描画コマンド
	cmdList->DrawIndexedInstanced((UINT)indices_.size(), 1, 0, 0, 0);
}
