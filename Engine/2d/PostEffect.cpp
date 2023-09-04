#include "PostEffect.h"
#include "WinApp.h"
#include "Input.h"
#include <d3dcompiler.h>
#include <cassert> 
#include <d3dx12.h>
#pragma comment(lib,"d3dcompiler.lib")

template <class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

//静的メンバ変数の実体//

//クリアカラー
const float PostEffect::clearColor_[4] = {
	0.25f,//R
	0.5f,//G
	0.1f,//B
	0.0f//A
};//緑っぽい色

void PostEffect::Initialize() {
	//デバイス
	device_ = DirectXBasis::GetInstance()->GetDevice();
	//コマンドリスト
	cmdList_ = DirectXBasis::GetInstance()->GetCommandList();

	//頂点バッファ作成
	CreateVertexBufferView();

	//定数バッファ生成
	GenerateConstBuffer();

	//テクスチャバッファ生成
	GenerateTextureBuffer();

	//テクスチャバッファへ転送
	TransferTextureBuffer();

	//SRV作成
	CreateSRV();

	//RTV作成
	CreateRTV();

	//深度バッファ生成
	GenerateDepthBuffer();

	//DSV作成
	CreateDSV();

	//パイプライン生成
	CreateGraphicsPipeLineState();
}

void PostEffect::Draw() {
	//非表示
	if (texture_.isInvisible_) {
		return;
	}

	//if (Input::GetInstance()->TriggerKey(DIK_0)) {
	//	//仮のテクスチャ番号
	//	static int tex = 0;
	//	//テクスチャ番号を0と1で切り替え
	//	tex = (tex + 1) % 2;

	//	//SRV設定
	//	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	//	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	//	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	//	srvDesc.Texture2D.MipLevels = 1;
	//	//デスクリプタヒープにSRV作成
	//	device_->CreateShaderResourceView(
	//		texBuff_[tex].Get(),
	//		&srvDesc,
	//		descHeapSRV_->GetCPUDescriptorHandleForHeapStart());
	//}

	//描画前処理//
	PreDraw();

	//描画処理//

	//頂点バッファビューの設定コマンド
	cmdList_->IASetVertexBuffers(0, 1, &vbView_);

	//定数バッファビュー(CBV)の設定コマンド
	cmdList_->SetGraphicsRootConstantBufferView(rootParam_Material, constBuffMaterial_->GetGPUVirtualAddress());

	//グラフィックスルートデスクリプタテーブルの設定コマンド
	TextureCommand();

	//定数バッファビュー(CBV)の設定コマンド
	cmdList_->SetGraphicsRootConstantBufferView(rootParam_Transform, constBuffTransform_->GetGPUVirtualAddress());

	//インスタンス描画
	cmdList_->DrawInstanced(kVerticesNum_, 1, 0, 0);
}

void PostEffect::CreateVertexBufferView() {
	HRESULT result;
	//上下左右の数値の設定
	texture_.dir_.left = -texture_.anchorPoint_.x;
	texture_.dir_.right = +texture_.anchorPoint_.x;
	texture_.dir_.top = +texture_.anchorPoint_.y;
	texture_.dir_.bottom = -texture_.anchorPoint_.y;

	float leftUv = 0.0f;//左
	float rightUv = 1.0f;//右
	float topUv = 0.0f;//上
	float bottomUv = 1.0f;//下

						  //頂点データを設定
	vertices_[LeftBottom].pos = Vector3(texture_.dir_.left, texture_.dir_.bottom, 0);
	vertices_[LeftTop].pos = Vector3(texture_.dir_.left, texture_.dir_.top, 0);
	vertices_[RightBottom].pos = Vector3(texture_.dir_.right, texture_.dir_.bottom, 0);
	vertices_[RightTop].pos = Vector3(texture_.dir_.right, texture_.dir_.top, 0);

	vertices_[LeftBottom].uv = Vector2(leftUv, bottomUv);
	vertices_[LeftTop].uv = Vector2(leftUv, topUv);
	vertices_[RightBottom].uv = Vector2(rightUv, bottomUv);
	vertices_[RightTop].uv = Vector2(rightUv, topUv);

	//頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	UINT sizeVB = static_cast<UINT>(sizeof(vertices_[0]) * _countof(vertices_));

#pragma region 頂点バッファ設定
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

#pragma region 頂点バッファ生成
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

#pragma region 頂点バッファへ転送
	//GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
	result = vertBuff_->Map(0, nullptr, (void**)&vertMap_);
	assert(SUCCEEDED(result));
	//全頂点に対して
	//座標をコピー
	std::copy(std::begin(vertices_), std::end(vertices_), vertMap_);

	//繋がりを解除
	vertBuff_->Unmap(0, nullptr);
#pragma endregion

#pragma region 頂点バッファビュー作成
	//頂点バッファビューの作成

	//GPU仮想アドレス
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	//頂点バッファのサイズ
	vbView_.SizeInBytes = sizeVB;
	//頂点1つ分のデータサイズ
	vbView_.StrideInBytes = sizeof(vertices_[0]);
#pragma endregion
}

void PostEffect::GenerateConstBuffer() {
	GenerateConstMaterial();
	GenerateConstTransform();
}

void PostEffect::GenerateConstMaterial() {
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
	constMapMaterial_->color_ = texture_.color_;
}

void PostEffect::GenerateConstTransform() {
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

	//ワールド変換行列と、平行投影変換行列を掛ける
	constMapTransform_->mat_ = Matrix4Identity();
}

void PostEffect::CreateGraphicsPipeLineState() {
	CompileShaderFile();
	AssembleVertexLayout();

	AssembleGraphicsPipeline();
	GenerateRootSignature();
	GeneratePipelineState();
}

void PostEffect::CompileShaderFile() {
	HRESULT result;

	//頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resource/shader/PostEffectVS.hlsl",//シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,//インクルード可能にする
		"main", "vs_5_0",//エントリーポイント名、シェーダ―モデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバッグ用設定
		0,
		&vsBlob_, &errorBlob_);

	//エラーなら
	if (FAILED(result)) {
		//errorBlobからのエラー内容をコピー
		std::string error;
		error.resize(errorBlob_->GetBufferSize());

		std::copy_n((char*)errorBlob_->GetBufferPointer(),
			errorBlob_->GetBufferSize(),
			error.begin());
		error += "\n";
		//エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	//ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resource/shader/PostEffectPS.hlsl",//シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,//インクルード可能にする
		"main", "ps_5_0",//エントリーポイント名、シェーダ―モデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバッグ用設定
		0,
		&psBlob_, &errorBlob_);

	//エラーなら
	if (FAILED(result)) {
		//errorBlobからのエラー内容をコピー
		std::string error;
		error.resize(errorBlob_->GetBufferSize());

		std::copy_n((char*)errorBlob_->GetBufferPointer(),
			errorBlob_->GetBufferSize(),
			error.begin());
		error += "\n";
		//エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}
}

void PostEffect::AssembleVertexLayout() {
	//頂点レイアウト
	enum LayoutElement {
		Position,
		TEXCOORD,
	};

	inputLayout_[Position] = {
		"POSITION",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0
	};

	inputLayout_[TEXCOORD] = {
		"TEXCOORD",
		0,
		DXGI_FORMAT_R32G32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0
	};
}

void PostEffect::AssembleGraphicsPipeline() {
	//グラフィックスパイプライン設定

	//シェーダの設定
	pipeLineDesc_.VS.pShaderBytecode = vsBlob_->GetBufferPointer();
	pipeLineDesc_.VS.BytecodeLength = vsBlob_->GetBufferSize();
	pipeLineDesc_.PS.pShaderBytecode = psBlob_->GetBufferPointer();
	pipeLineDesc_.PS.BytecodeLength = psBlob_->GetBufferSize();

	//サンプルマスクの設定
	pipeLineDesc_.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;//標準設定

														 //ラスタライザの設定
	pipeLineDesc_.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//カリングしない
	pipeLineDesc_.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;//ポリゴン内塗りつぶし
	pipeLineDesc_.RasterizerState.DepthClipEnable = true;//深度クリッピングを有効に

														 //ブレンドステート

														 //レンダ―ターゲットビューのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipeLineDesc_.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;//RGBA全てにチャネルを描画

																   //共通設定
	blenddesc.BlendEnable = true;//ブレンドを有効にする
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;//加算
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;//ソースの値を100%使う
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;//デストの値を0%使う

												//加算合成
												//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;//加算
												//blenddesc.SrcBlend = D3D12_BLEND_ONE;//ソースの値を100%使う
												//blenddesc.DestBlend = D3D12_BLEND_ONE;//デストを100%使う

												//減算合成
												//blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;//デストからソースを減算
												//blenddesc.SrcBlend = D3D12_BLEND_ONE;//ソースの値を100%使う
												//blenddesc.DestBlend = D3D12_BLEND_ONE;//デストを100%使う

												//色反転
												//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;//加算
												//blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;//1.0f - デストカラーの値
												//blenddesc.DestBlend = D3D12_BLEND_ZERO;//使わない

												//半透明合成
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;//加算
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;//ソースのアルファ値
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//1.0ff - ソースのアルファ値

	pipeLineDesc_.InputLayout.pInputElementDescs = inputLayout_;
	pipeLineDesc_.InputLayout.NumElements = _countof(inputLayout_);

	//三角形に設定
	pipeLineDesc_.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	pipeLineDesc_.NumRenderTargets = 1;//描画対象は1つ
	pipeLineDesc_.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//0～255指定のRGBA
	pipeLineDesc_.SampleDesc.Count = 1;//1ピクセルにつき1回サンプリング
}

void PostEffect::GenerateRootSignature() {
	HRESULT result;

	//デスクリプタレンジ0番の設定
	D3D12_DESCRIPTOR_RANGE descriptorRange0{};
	descriptorRange0.NumDescriptors = 1;
	descriptorRange0.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange0.BaseShaderRegister = 0;//テクスチャレジスタ0番
	descriptorRange0.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//デスクリプタレンジ1番の設定
	D3D12_DESCRIPTOR_RANGE descriptorRange1{};
	descriptorRange1.NumDescriptors = 1;
	descriptorRange1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange1.BaseShaderRegister = 1;//テクスチャレジスタ0番
	descriptorRange1.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//ルートパラメータの設定
	D3D12_ROOT_PARAMETER rootParams[kRootParamCount_] = {};
	//定数バッファ0番
	rootParams[rootParam_Material].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//種類
	rootParams[rootParam_Material].Descriptor.ShaderRegister = 0;					//定数バッファ番号
	rootParams[rootParam_Material].Descriptor.RegisterSpace = 0;						//デフォルト値
	rootParams[rootParam_Material].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//全てのシェーダから見える
																					//テクスチャレジスタ0番
	rootParams[rootParam_Texture].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	//種類
	rootParams[rootParam_Texture].DescriptorTable.pDescriptorRanges = &descriptorRange0;			//デスクリプタレンジ
	rootParams[rootParam_Texture].DescriptorTable.NumDescriptorRanges = 1;						//デスクリプタレンジ数
	rootParams[rootParam_Texture].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;				//全てのシェーダから見える
																								//定数バッファ1番
	rootParams[rootParam_Transform].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//種類
	rootParams[rootParam_Transform].Descriptor.ShaderRegister = 1;					//定数バッファ番号
	rootParams[rootParam_Transform].Descriptor.RegisterSpace = 0;						//デフォルト値
	rootParams[rootParam_Transform].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//全てのシェーダから見える
																					//テクスチャレジスタ1番
	rootParams[rootParam_SubTexture].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	//種類
	rootParams[rootParam_SubTexture].DescriptorTable.pDescriptorRanges = &descriptorRange1;			//デスクリプタレンジ
	rootParams[rootParam_SubTexture].DescriptorTable.NumDescriptorRanges = 1;						//デスクリプタレンジ数
	rootParams[rootParam_SubTexture].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;				//全てのシェーダから見える

																									//テクスチャサンプラーの設定
	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	//ルートシグネチャの設定
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParams;//ルートパラメータの先頭アドレス
	rootSignatureDesc.NumParameters = _countof(rootParams);//ルートパラメータ―数
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	//ルートシグネチャのシリアライズ
	ComPtr<ID3DBlob> rootSigBlob;

	result = D3D12SerializeRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob,
		&errorBlob_);
	assert(SUCCEEDED(result));

	result = device_->CreateRootSignature(
		0,
		rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(result));
	//パイプラインにルートシグネチャをセット
	pipeLineDesc_.pRootSignature = rootSignature_.Get();
}

void PostEffect::GeneratePipelineState() {
	HRESULT result;
	//パイプラインステートの生成
	result = device_->CreateGraphicsPipelineState(
		&pipeLineDesc_,
		IID_PPV_ARGS(&pipeLineState_));
	assert(SUCCEEDED(result));
}

void PostEffect::PreDraw() {
	//パイプラインステートとルートシグネイチャの設定コマンド
	cmdList_->SetPipelineState(pipeLineState_.Get());
	cmdList_->SetGraphicsRootSignature(rootSignature_.Get());

	//プリミティブ形状の設定コマンド
	cmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);//三角形ストリップ								   //デスクリプタヒープの配列をセットするコマンド

																		   //デスクリプタヒープ設定コマンド
	ID3D12DescriptorHeap* ppHeaps[] = { descHeapSRV_.Get() };
	cmdList_->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
}

void PostEffect::TextureCommand() {
	//SRVヒープの先頭ハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle =
		descHeapSRV_->GetGPUDescriptorHandleForHeapStart();

	//SRVヒープの先頭にあるSRVをテクスチャルートパラメータに設定
	cmdList_->SetGraphicsRootDescriptorTable(
		rootParam_Texture,
		CD3DX12_GPU_DESCRIPTOR_HANDLE(
			descHeapSRV_->GetGPUDescriptorHandleForHeapStart(), 0,
			device_->GetDescriptorHandleIncrementSize(
				D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		)
	);

	//次のアドレスにあるSRVをサブテクスチャルートパラメータに設定
	cmdList_->SetGraphicsRootDescriptorTable(
		rootParam_SubTexture,
		CD3DX12_GPU_DESCRIPTOR_HANDLE(
			descHeapSRV_->GetGPUDescriptorHandleForHeapStart(), 1,
			device_->GetDescriptorHandleIncrementSize(
				D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		)
	);
}
void PostEffect::PreDrawScene() {
	//リソースバリアデスク設定(シェーダーリソースから、描画可能状態に)
	for (int i = 0; i < kTextureBufferCount_; i++) {
		barrierDesc_.Transition.pResource = texBuff_[i].Get();
		barrierDesc_.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		barrierDesc_.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

		//リソースバリアを変更
		cmdList_->ResourceBarrier(1, &barrierDesc_);
	}

	//レンダ―ターゲットビュー用デスクリプタヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHs[kTextureBufferCount_]{};

	for (int i = 0; i < kTextureBufferCount_; i++) {
		rtvHs[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(
			descHeapRTV_->GetCPUDescriptorHandleForHeapStart(), i,
			device_->GetDescriptorHandleIncrementSize(
				D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
	}

	//深度ステンシルビュー用デスクリプタヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH =
		descHeapDSV_->GetCPUDescriptorHandleForHeapStart();

	//レンダ―ターゲットをセット
	cmdList_->OMSetRenderTargets(kTextureBufferCount_, rtvHs, false, &dsvH);

	//ビューポートの設定
	D3D12_VIEWPORT viewports[kTextureBufferCount_]{};
	//シザー矩形の設定
	D3D12_RECT scissorRects[kTextureBufferCount_]{};

	for (int i = 0; i < kTextureBufferCount_; i++) {
		viewports[i].Width = WinApp::Win_Width;
		viewports[i].Height = WinApp::Win_Height;
		viewports[i].TopLeftX = 0;
		viewports[i].TopLeftY = 0;
		viewports[i].MinDepth = 0.0f;
		viewports[i].MaxDepth = 1.0f;

		scissorRects[i].left = 0;										//切り抜き座標左
		scissorRects[i].right = scissorRects[i].left + WinApp::Win_Width;	//切り抜き座標右
		scissorRects[i].top = 0;										//切り抜き座標上
		scissorRects[i].bottom = scissorRects[i].top + WinApp::Win_Height;	//切り抜き座標下
	}

	//ビューポート設定コマンドを、コマンドリストに積む
	cmdList_->RSSetViewports(kTextureBufferCount_, viewports);
	//シザー矩形設定コマンドを、コマンドリストに積む
	cmdList_->RSSetScissorRects(kTextureBufferCount_, scissorRects);

	for (int i = 0; i < kTextureBufferCount_; i++) {
		//RTVクリア
		cmdList_->ClearRenderTargetView(rtvHs[i], clearColor_, 0, nullptr);
	}
	//DSVクリア
	cmdList_->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void PostEffect::PostDrawScene() {
	for (int i = 0; i < kTextureBufferCount_; i++) {
		//リソースバリアデスク設定(描画可能状態から、シェーダーリソースに)
		barrierDesc_.Transition.pResource = texBuff_[i].Get();
		barrierDesc_.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrierDesc_.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

		//リソースバリアを変更
		cmdList_->ResourceBarrier(1, &barrierDesc_);
	}
}

void PostEffect::GenerateTextureBuffer() {
	HRESULT result = 0;

	//テクスチャバッファヒープ設定
	D3D12_HEAP_PROPERTIES texHeapProp{};
	texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	texHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	//テクスチャバッファのクリア設定
	D3D12_CLEAR_VALUE texClearValue{};
	texClearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	for (size_t i = 0; i < 4; i++) {
		texClearValue.Color[i] = clearColor_[i];
	}

	//テクスチャリソース設定
	D3D12_RESOURCE_DESC texResDesc{};
	texResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texResDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	texResDesc.Width = WinApp::Win_Width;
	texResDesc.Height = (UINT)WinApp::Win_Height;
	texResDesc.DepthOrArraySize = 1;
	texResDesc.MipLevels = 0;
	texResDesc.SampleDesc.Count = 1;
	texResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	for (int i = 0; i < kTextureBufferCount_; i++) {
		//テクスチャバッファ生成
		result = device_->CreateCommittedResource(
			&texHeapProp,
			D3D12_HEAP_FLAG_NONE,
			&texResDesc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			&texClearValue,
			IID_PPV_ARGS(&texBuff_[i])
		);
		assert(SUCCEEDED(result));
	}
}

void PostEffect::TransferTextureBuffer() {
	HRESULT result = 0;

	//テクスチャを赤でクリア//

	//画素数(1200 * 720 = 921600ピクセル)
	const UINT pixcelCount =
		WinApp::Win_Width * WinApp::Win_Height;
	//画像1行分のデータサイズ
	const UINT rowPitch =
		sizeof(UINT) * WinApp::Win_Width;
	//画像全体のデータサイズ
	const UINT depthPitch = rowPitch * WinApp::Win_Height;
	for (int i = 0; i < kTextureBufferCount_; i++) {
		//画像イメージ
		UINT* img = new UINT[pixcelCount];
		for (int j = 0; j < pixcelCount; j++) {
			img[j] = 0xff0000ff;
		}

		//テクスチャバッファにデータ転送
		result = texBuff_[i]->WriteToSubresource(
			0, nullptr,
			img, rowPitch, depthPitch);
		assert(SUCCEEDED(result));
		delete[] img;
	}
}

void PostEffect::CreateSRV() {
	HRESULT result = 0;

	//SRV用デスクリプタヒープ設定
	D3D12_DESCRIPTOR_HEAP_DESC srvDescHeapDesc = {};
	srvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvDescHeapDesc.NumDescriptors = kTextureBufferCount_;
	//SRV用デスクリプタヒープを生成
	result = device_->CreateDescriptorHeap(
		&srvDescHeapDesc,
		IID_PPV_ARGS(&descHeapSRV_)
	);
	assert(SUCCEEDED(result));

	//SRV設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	for (int i = 0; i < kTextureBufferCount_; i++) {
		//デスクリプタヒープにSRV作成
		device_->CreateShaderResourceView(
			texBuff_[i].Get(),
			&srvDesc,
			CD3DX12_CPU_DESCRIPTOR_HANDLE(
				descHeapSRV_->GetCPUDescriptorHandleForHeapStart(), i,
				device_->GetDescriptorHandleIncrementSize(
					D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
			)
		);
	}
}

void PostEffect::CreateRTV() {
	HRESULT result = 0;

	//RTV用デスクリプタヒープ設定
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescHeapDesc{};
	rtvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDescHeapDesc.NumDescriptors = kTextureBufferCount_;

	//RTV用デスクリプタヒープ生成
	result = device_->CreateDescriptorHeap(
		&rtvDescHeapDesc,
		IID_PPV_ARGS(&descHeapRTV_)
	);
	assert(SUCCEEDED(result));

	//レンダ―ターゲットビューの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	//シェーダーの計算結果をSRGBに変換して書き込む
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	for (int i = 0; i < kTextureBufferCount_; i++) {
		//レンダ―ターゲットビューの生成
		device_->CreateRenderTargetView(
			texBuff_[i].Get(),
			nullptr,
			CD3DX12_CPU_DESCRIPTOR_HANDLE(
				descHeapRTV_->GetCPUDescriptorHandleForHeapStart(), i,
				device_->GetDescriptorHandleIncrementSize(
					D3D12_DESCRIPTOR_HEAP_TYPE_RTV))
		);
	}
}

void PostEffect::GenerateDepthBuffer() {
	HRESULT result = 0;

	//深度リソース設定
	D3D12_RESOURCE_DESC depthResDesc{};
	depthResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResDesc.Width = WinApp::Win_Width;
	depthResDesc.Height = WinApp::Win_Height;
	depthResDesc.DepthOrArraySize = 1;
	depthResDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthResDesc.SampleDesc.Count = 1;
	depthResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	//深度値用ヒーププロパティ
	D3D12_HEAP_PROPERTIES depthHeapProp{};
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	//深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;

	//リソース生成
	result = device_->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(&depthBuff_));
}

void PostEffect::CreateDSV() {
	HRESULT result = 0;

	//深度ビュー用デスクリプタヒープ設定
	D3D12_DESCRIPTOR_HEAP_DESC dsvDescHeapDesc = {};
	dsvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvDescHeapDesc.NumDescriptors = 1;

	//DSV用デスクリプタヒープを生成
	result = device_->CreateDescriptorHeap(
		&dsvDescHeapDesc,
		IID_PPV_ARGS(&descHeapDSV_)
	);
	assert(SUCCEEDED(result));

	//深度ビュー設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	//デスクリプタヒープにDSV作成
	device_->CreateDepthStencilView(
		depthBuff_.Get(),
		&dsvDesc,
		descHeapDSV_->GetCPUDescriptorHandleForHeapStart()
	);
}

PostEffect::PostEffect() {
	texture_.anchorPoint_ = { 1.0f,1.0f };

	texture_.isInvisible_ = false;
}
