#include "Particle.h"
#include <DirectXTex.h>
#include <cassert>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <Windows.h>
#include <d3dx12.h>

using namespace DirectX;
using namespace std;
template <class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

//静的メンバ変数の実体
// デバイス
ComPtr<ID3D12Device> Particle::device_ = nullptr;
//デフォルトディレクトリ
std::string Particle::defaultDirectoryPath_ = "Resource/particle/";
//インクリメントサイズ
UINT Particle::incrementSize_;
//デスクリプタヒープ
ComPtr<ID3D12DescriptorHeap> Particle::srvDescHeap_;

Particle* Particle::LoadFromObjModel(const std::string& fileName) {
	Particle* particle = new Particle();

	//デスクリプタヒープ生成
	particle->InitializeDescriptorHeap();

	//OBJファイルからのデータ読み込み
	particle->LoadTexture(fileName);

	//読み込んだデータを元に各種バッファ生成
	particle->CreateBuffers();

	return particle;
}

void Particle::InitializeDescriptorHeap() {
	HRESULT result = S_FALSE;

	// デスクリプタヒープを生成	
	D3D12_DESCRIPTOR_HEAP_DESC srvDescHeapDesc = {};
	srvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//シェーダから見えるように
	srvDescHeapDesc.NumDescriptors = 1; // シェーダーリソースビュー1つ
	result = device_->CreateDescriptorHeap(
		&srvDescHeapDesc,
		IID_PPV_ARGS(&srvDescHeap_)
	);//生成
	if (FAILED(result)) {
		assert(0);
	}

	// デスクリプタサイズを取得
	incrementSize_ =
		device_->GetDescriptorHandleIncrementSize(
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void Particle::LoadTexture(const std::string& fileName) {
	HRESULT result;

	//ディレクトリパスとファイル名を連結してフルパスを得る
	std::string fullPath =
		defaultDirectoryPath_ + fileName;

	//ワイド文字列に変換した際の文字列バッファサイズを計算
	int filePathBufferSize =
		MultiByteToWideChar(
			CP_ACP, 0,
			fullPath.c_str(),
			-1, nullptr, 0);

	//ワイド文字列に変換
	std::vector<wchar_t> wfilePath(filePathBufferSize);
	MultiByteToWideChar(
		CP_ACP, 0,
		fullPath.c_str(), -1, wfilePath.data(),
		filePathBufferSize);

	//画像ファイルの用意
	TexMetadata metadata{};
	ScratchImage scratchImg{};

	//WICテクスチャのロード
	result = LoadFromWICFile(
		wfilePath.data(),
		WIC_FLAGS_NONE,
		&metadata,
		scratchImg);

	assert(SUCCEEDED(result));

	ScratchImage mipChain{};
	//ミップマップ生成
	result = GenerateMipMaps(
		scratchImg.GetImages(),
		scratchImg.GetImageCount(),
		scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT,
		0,
		mipChain);

	if (SUCCEEDED(result)) {
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}
	//読み込んだディフューズテクスチャをSRGBとして扱う
	metadata.format = MakeSRGB(metadata.format);

	// ヒープ設定
	D3D12_HEAP_PROPERTIES texHeapProp{};
	texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	texHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	//リソース設定
	D3D12_RESOURCE_DESC texResDesc{};
	texResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texResDesc.Format = metadata.format;
	texResDesc.Width = metadata.width;							//幅
	texResDesc.Height = (UINT)metadata.height;				//高さ
	texResDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
	texResDesc.MipLevels = (UINT16)metadata.mipLevels;
	texResDesc.SampleDesc.Count = 1;

	//テクスチャバッファの生成
	result = device_->CreateCommittedResource(
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&texResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuff_));
	assert(SUCCEEDED(result));

	for (size_t i = 0; i < metadata.mipLevels; i++) {
		//ミップマップレベルを指定してイメージを取得
		const Image* img =
			scratchImg.GetImage(i, 0, 0);
		//テクスチャバッファにデータ転送
		result = texBuff_->WriteToSubresource(
			(UINT)i,
			nullptr,			 //全領域へコピー
			img->pixels,		 //元データアドレス
			(UINT)img->rowPitch, //1ラインサイズ
			(UINT)img->slicePitch//1枚サイズ
		);
		assert(SUCCEEDED(result));
	}

	//SRVヒープのハンドルを取得
	srvCpuDescHandle_ =
		srvDescHeap_->GetCPUDescriptorHandleForHeapStart();
	//srvCpuDescHandle_.ptr += incrementSize_;

	srvGpuDescHandle_ =
		srvDescHeap_->GetGPUDescriptorHandleForHeapStart();
	//srvGpuDescHandle_.ptr += incrementSize_;

	//シェーダーリソースビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};	//設定構造体
	srvDesc.Format = texResDesc.Format;
	srvDesc.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = texResDesc.MipLevels;

	//ハンドルの指す位置にシェーダーリソースビュー作成
	device_->CreateShaderResourceView(
		texBuff_.Get(),
		&srvDesc,
		srvCpuDescHandle_);
}

void Particle::CreateBuffers() {
	HRESULT result = S_FALSE;

#pragma region VerticesData
	//頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	UINT sizeVB = static_cast<UINT>(sizeof(vertices_[0]) * _countof(vertices_));
#pragma endregion

#pragma region VertexBafferSetting
	//頂点バッファの設定
	D3D12_HEAP_PROPERTIES vbHeapProp{};
	vbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	//リソース設定
	D3D12_RESOURCE_DESC vbResDesc{};
	vbResDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vbResDesc.Width = sizeVB;
	vbResDesc.Height = 1;
	vbResDesc.DepthOrArraySize = 1;
	vbResDesc.MipLevels = 1;
	vbResDesc.SampleDesc.Count = 1;
	vbResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
#pragma endregion

#pragma region VertexBafferCreate
	//頂点バッファの生成
	result = device_->CreateCommittedResource(
		&vbHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&vbResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff_));
	assert(SUCCEEDED(result));
#pragma endregion

#pragma region VertexBafferTransfer
	// 頂点バッファへのデータ転送

	//GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
	result = vertBuff_->Map(0, nullptr, (void**)&vertMap_);
	assert(SUCCEEDED(result));
	//全頂点に対して
	//座標をコピー
	std::copy(std::begin(vertices_), std::end(vertices_), vertMap_);

	//繋がりを解除
	vertBuff_->Unmap(0, nullptr);
#pragma endregion

#pragma region VertexBafferViewCreate
	//頂点バッファビューの作成

	//GPU仮想アドレス
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	//頂点バッファのサイズ
	vbView_.SizeInBytes = sizeVB;
	//頂点1つ分のデータサイズ
	vbView_.StrideInBytes = sizeof(vertices_[0]);
#pragma endregion
}

void Particle::Update() {
	HRESULT result;
	//寿命が尽きたパーティクルを全削除
	particles_.remove_if([](OneParticle& x) {
		return x.frameNow_ >= x.frameEnd_;
		});
	//全パーティクル更新
	for (std::forward_list<OneParticle>::iterator it =
		particles_.begin();
		it != particles_.end(); it++) {
		//経過フレーム数をカウント
		it->frameNow_++;
		//速度に加速度を加算
		it->velocity_ = it->velocity_ + it->accel_;
		//速度による移動
		it->position_ = it->position_ + it->velocity_;

		//進行度を0~1の範囲に換算
		float f = (float)it->frameNow_ / it->frameEnd_;
		//スケールの線形補間
		it->scale_ = (it->scaleEnd_ - it->scaleStart_) * f;
		it->scale_ += it->scaleStart_;
	}
	//頂点バッファへデータ転送
	result = vertBuff_->Map(0, nullptr, (void**)&vertMap_);
	if (SUCCEEDED(result)) {
		//パーティクルの情報を1つずつ反映
		for (std::forward_list<OneParticle>::iterator it =
			particles_.begin();
			it != particles_.end(); it++) {
			//座標
			vertMap_->position_ = it->position_;
			//UVのX
			vertMap_->uv_ = it->scale_;
			//UVのY
			//vertMap_->uv_.y = it->scale_;
			//次の頂点へ
			vertMap_++;
		}
		vertBuff_->Unmap(0, nullptr);
	}
}

void Particle::Draw(ID3D12GraphicsCommandList* cmdList) {
	// nullptrチェック
	assert(device_);
	assert(cmdList);

	// 頂点バッファの設定
	cmdList->IASetVertexBuffers(0, 1, &vbView_);

	// デスクリプタヒープの配列
	ID3D12DescriptorHeap* ppHeaps[] = { srvDescHeap_.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	// シェーダリソースビューをセット
	cmdList->SetGraphicsRootDescriptorTable(1, srvGpuDescHandle_);

	// 描画コマンド
	cmdList->DrawInstanced(
		(UINT)std::distance(
			particles_.begin(),
			particles_.end()),
		1, 0, 0);
}

void Particle::Add(
	int life,
	Vector3 position,
	Vector3 velocity,
	Vector3 accel,
	float start_scale,
	float end_scale) {
	//リストに要素を追加
	particles_.emplace_front();
	//追加した要素の参照
	OneParticle& p = particles_.front();
	//値セット
	p.position_ = position;
	p.velocity_ = velocity;
	p.accel_ = accel;
	p.frameEnd_ = life;

	p.scaleStart_ = start_scale;
	p.scaleEnd_ = end_scale;
	p.scale_ = start_scale;
}