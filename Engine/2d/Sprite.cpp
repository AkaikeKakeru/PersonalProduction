#include "Sprite.h"
#include "SafeDelete.h"
#include "MyMath.h"
#include "WinApp.h"
#include <cassert>

SpriteBasis* Sprite::spriteBas_ = SpriteBasis::GetInstance();

void Sprite::Initialize(uint32_t textureIndex) {
	assert(spriteBas_);
	device_ = spriteBas_->GetDevice();
	cmdList_ = spriteBas_->GetCommandList();

	//テクスチャサイズをイメージに合わせる
	if (textureIndex != UINT32_MAX) {
		textureIndex_ = textureIndex;
		AdjustTextureSize();
		//テクスチャサイズをスプライトのサイズに適用
		size_ = textureSize_;
	}

	worldTransform_.scale = { 1,1,1 };
	worldTransform_.rotation = ConvertToRadian(0.0f);
	worldTransform_.position = { 0,0 };
	worldTransform_.matWorld = Matrix4Identity();

	matOrtGrapricProjection_ = Matrix4Identity();
	//テクスチャの左上を、画面の左上角に合わせたい
	//ポリゴンの左上を、画面中央に合わせる
	matOrtGrapricProjection_.m[0][0] = 2.0f / WinApp::Win_Width;
	matOrtGrapricProjection_.m[1][1] = -2.0f / WinApp::Win_Height;
	//上の状態から、画面半分くらいの距離だけ、左上にずらす
	matOrtGrapricProjection_.m[3][0] = -1.0f;
	matOrtGrapricProjection_.m[3][1] = 1.0f;

	CreateVertexBufferView();
	GenerateConstBuffer();
}

void Sprite::Update() {
	//上下左右の数値の更新
	dir_.left = (0.0f - anchorPoint_.x) * size_.x;
	dir_.right = (1.0f - anchorPoint_.x) * size_.x;
	dir_.top = (0.0f - anchorPoint_.y) * size_.y;
	dir_.bottom = (1.0f - anchorPoint_.y) * size_.y;

	//左右反転
	if (isFlipX_) {
		dir_.left = -dir_.left;
		dir_.right = -dir_.right;
	}
	//上下反転
	if (isFlipY_) {
		dir_.top = -dir_.top;
		dir_.bottom = -dir_.bottom;
	}

	//頂点データを設定
	vertices_[LeftBottom].pos = Vector3(dir_.left, dir_.bottom, 0);
	vertices_[LeftTop].pos = Vector3(dir_.left, dir_.top, 0);
	vertices_[RightBottom].pos = Vector3(dir_.right, dir_.bottom, 0);
	vertices_[RightTop].pos = Vector3(dir_.right, dir_.top, 0);

	ID3D12Resource* textureBuffer = spriteBas_->GetTextureBuffer(textureIndex_);
	//指定番号の画像が読み込み済みなら
	if (textureBuffer) {
		//テクスチャ情報取得
		D3D12_RESOURCE_DESC texResDesc = textureBuffer->GetDesc();

		float tex_left = textureLeftTop_.x / texResDesc.Width;
		float tex_right = (textureLeftTop_.x + textureSize_.x) / texResDesc.Width;
		float tex_top = textureLeftTop_.y / texResDesc.Height;
		float tex_bottom = (textureLeftTop_.y + textureSize_.y) / texResDesc.Height;

		//頂点のUVに反映する
		vertices_[LeftBottom].uv = { tex_left,tex_bottom };//左下
		vertices_[LeftTop].uv = { tex_left,tex_top };//左上
		vertices_[RightBottom].uv = { tex_right,tex_bottom };//右下
		vertices_[RightTop].uv = { tex_right,tex_top };//右上
	}

	///値を書き込むと自動的に転送される

	//頂点データをGPUに転送
	std::copy(std::begin(vertices_), std::end(vertices_), vertMap_);

	//色情報をGPUに転送
	constMapMaterial_->color = color_;

	//ワールド行列を再計算
	ReCalcMatWorld();

	//ワールド座標情報をGPUに転送
	//ワールド変換行列と、平行投影変換行列を掛ける
	constMapTransform_->mat = worldTransform_.matWorld *= matOrtGrapricProjection_;
}

void Sprite::Draw() {
	//非表示
	if (isInvisible_) {
		return;
	}

	//描画用テクスチャコマンド
	spriteBas_->SetTextureCommand(textureIndex_);

	//頂点バッファビューの設定コマンド
	cmdList_->IASetVertexBuffers(0, 1, &vbView_);

	//定数バッファビュー(CBV)の設定コマンド
	cmdList_->SetGraphicsRootConstantBufferView(0, constBuffMaterial_->GetGPUVirtualAddress());

	//定数バッファビュー(CBV)の設定コマンド
	cmdList_->SetGraphicsRootConstantBufferView(2, constBuffTransform_->GetGPUVirtualAddress());

	//インスタンス描画
	cmdList_->DrawInstanced(kVerticesNum, 1, 0, 0);
}

void Sprite::CreateVertexBufferView() {
	HRESULT result;
#pragma region VerticesData
	//上下左右の数値の設定
	dir_.left = (0.0f - anchorPoint_.x) * size_.x;
	dir_.right = (1.0f - anchorPoint_.x) * size_.x;
	dir_.top = (0.0f - anchorPoint_.y) * size_.y;
	dir_.bottom = (1.0f - anchorPoint_.y) * size_.y;

	float leftUv = 0.0f;//左
	float rightUv = 1.0f;//右
	float topUv = 0.0f;//上
	float bottomUv = 1.0f;//下

	//左右反転
	if (isFlipX_) {
		dir_.left = -dir_.left;
		dir_.right = -dir_.right;
	}
	//上下反転
	if (isFlipY_) {
		dir_.top = -dir_.top;
		dir_.bottom = -dir_.bottom;
	}

	//頂点データを設定
	vertices_[LeftBottom].pos = Vector3(dir_.left, dir_.bottom, 0);
	vertices_[LeftTop].pos = Vector3(dir_.left, dir_.top, 0);
	vertices_[RightBottom].pos = Vector3(dir_.right, dir_.bottom, 0);
	vertices_[RightTop].pos = Vector3(dir_.right, dir_.top, 0);

	vertices_[LeftBottom].uv = Vector2(leftUv, bottomUv);
	vertices_[LeftTop].uv = Vector2(leftUv, topUv);
	vertices_[RightBottom].uv = Vector2(rightUv, bottomUv);
	vertices_[RightTop].uv = Vector2(rightUv, topUv);

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

void Sprite::GenerateConstBuffer() {
	GenerateConstMaterial();
	GenerateConstTransform();
}

void Sprite::GenerateConstMaterial() {
	HRESULT result;

	//定数バッファヒープ設定
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;//GPUへの転送用
	//定数バッファリソース設定
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff;//256バイトアライメント
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//定数バッファの生成
	result = device_->CreateCommittedResource(
		&cbHeapProp,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc,//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffMaterial_));
	assert(SUCCEEDED(result));

	//定数バッファのマッピング
	result = constBuffMaterial_->Map(0, nullptr, (void**)&constMapMaterial_);//マッピング
	assert(SUCCEEDED(result));

	//値を書き込むと自動的に転送される

	//色情報をGPUに転送
	constMapMaterial_->color = color_;
}

void Sprite::GenerateConstTransform() {
	HRESULT result;

	//定数バッファヒープ設定
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;//GPUへの転送用
	//定数バッファリソース設定
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff;//256バイトアライメント
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//定数バッファの生成
	result = device_->CreateCommittedResource(
		&cbHeapProp,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc,//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffTransform_));
	assert(SUCCEEDED(result));

	//定数バッファのマッピング
	result = constBuffTransform_->Map(0, nullptr, (void**)&constMapTransform_);//マッピング
	assert(SUCCEEDED(result));

	//値を書き込むと自動的に転送される
	//ワールド行列を再計算
	ReCalcMatWorld();

	//ワールド変換行列と、平行投影変換行列を掛ける
	constMapTransform_->mat = worldTransform_.matWorld *= matOrtGrapricProjection_;
}

void Sprite::ReCalcMatWorld() {
	worldTransform_.matWorld = Matrix4Identity();

	worldTransform_.matWorld *=
		Matrix4RotationZ(worldTransform_.rotation);
	worldTransform_.matWorld *=
		Matrix4Translation(
			Vector3(
				worldTransform_.position.x,
				worldTransform_.position.y,
				0.0f));
}

void Sprite::AdjustTextureSize() {
	ID3D12Resource* textureBuffer = spriteBas_->GetTextureBuffer(textureIndex_);
	assert(textureBuffer);

	//テクスチャ情報取得
	D3D12_RESOURCE_DESC texResDesc = textureBuffer->GetDesc();

	textureSize_.x = static_cast<float>(texResDesc.Width);
	textureSize_.y = static_cast<float>(texResDesc.Height);
}