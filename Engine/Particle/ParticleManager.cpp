#include "ParticleManager.h"
#include <d3dcompiler.h>
#include <DirectXTex.h>
#include "Random.h"

#include "DirectXBasis.h"

#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;
template <class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

/// <summary>
/// 静的メンバ変数の実体
/// </summary>
ComPtr<ID3D12Device> ParticleManager::device_ = nullptr;
ComPtr<ID3D12GraphicsCommandList> ParticleManager::cmdList_ = nullptr;
ComPtr<ID3D12PipelineState> ParticleManager::pipelineState_;
ComPtr<ID3D12RootSignature> ParticleManager::rootSignature_;
ComPtr<ID3DBlob> ParticleManager::vsBlob_;
ComPtr<ID3DBlob> ParticleManager::gsBlob_;
ComPtr<ID3DBlob> ParticleManager::psBlob_;
ComPtr<ID3DBlob> ParticleManager::errorBlob_;
D3D12_INPUT_ELEMENT_DESC ParticleManager::inputLayout_[kInputLayoutElement];
D3D12_GRAPHICS_PIPELINE_STATE_DESC ParticleManager::pipelineDesc_ = {};

void ParticleManager::StaticInitialize(ID3D12Device* device) {
	// nullptrチェック
	assert(device);

	ParticleManager::device_ = device;

	Particle::SetDevice(device_.Get());

	// パイプライン初期化
	CompileShaderFile();
	AssembleVertexLayout();
	CreateGraphicsPipeline();
}

void ParticleManager::PreDraw(ID3D12GraphicsCommandList* cmdList) {
	// PreDrawとPostDrawがペアで呼ばれていなければエラー
	assert(ParticleManager::cmdList_ == nullptr);

	// コマンドリストをセット
	ParticleManager::cmdList_ = cmdList;

	// パイプラインステートの設定
	cmdList->SetPipelineState(pipelineState_.Get());
	// ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(rootSignature_.Get());
	// プリミティブ形状を設定
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
}

void ParticleManager::PostDraw() {
	// コマンドリストを解除
	ParticleManager::cmdList_ = nullptr;
}

ParticleManager* ParticleManager::Create() {
	// 3Dオブジェクトのインスタンスを生成
	ParticleManager* instance = new ParticleManager();
	if (instance == nullptr) {
		return nullptr;
	}

	// 初期化
	if (!instance->Initialize()) {
		delete instance;
		assert(0);
		return nullptr;
	}

	return instance;
}

void ParticleManager::CompileShaderFile() {
	HRESULT result = S_FALSE;

	// 頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resource/shader/ParticleVS.hlsl",	// シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "vs_5_0",	// エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
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

	// ジオメトリシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resource/shader/ParticleGS.hlsl",	// シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "gs_5_0",	// エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&gsBlob_, &errorBlob_);
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

	// ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resource/shader/ParticlePS.hlsl",	// シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "ps_5_0",	// エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
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

void ParticleManager::AssembleVertexLayout() {
	//頂点レイアウト
	enum LayoutElement {
		Position,
		Texcoord,
	};

	// xy座標
	inputLayout_[Position] = {
		"POSITION",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0
	};

	//スケール
	inputLayout_[Texcoord] = {
		"TEXCOORD",
		0,
		DXGI_FORMAT_R32G32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0
	};
}

void ParticleManager::CreateGraphicsPipeline() {
	// グラフィックスパイプラインの流れを設定

	AssembleGraphicsPipeline();
	GenerateRootSignature();
	GeneratePipelineState();
}

void ParticleManager::AssembleGraphicsPipeline() {
	// グラフィックスパイプラインの流れを設定

	//シェーダの設定
	pipelineDesc_.VS.pShaderBytecode = vsBlob_.Get()->GetBufferPointer();
	pipelineDesc_.VS.BytecodeLength = vsBlob_.Get()->GetBufferSize();
	pipelineDesc_.GS.pShaderBytecode = gsBlob_.Get()->GetBufferPointer();
	pipelineDesc_.GS.BytecodeLength = gsBlob_.Get()->GetBufferSize();
	pipelineDesc_.PS.pShaderBytecode = psBlob_.Get()->GetBufferPointer();
	pipelineDesc_.PS.BytecodeLength = psBlob_.Get()->GetBufferSize();

	// サンプルマスクの設定
	pipelineDesc_.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定

	 //ラスタライザの設定
	pipelineDesc_.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//カリングしない
	pipelineDesc_.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;//ポリゴン内塗りつぶし
	pipelineDesc_.RasterizerState.DepthClipEnable = true;//深度クリッピングを有効に

	// デプスステンシルステート
	pipelineDesc_.DepthStencilState.DepthEnable = true;
	pipelineDesc_.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	pipelineDesc_.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	pipelineDesc_.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	//デプスの書き込みを禁止
	pipelineDesc_.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

	//レンダ―ターゲットビューのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc =
		pipelineDesc_.BlendState.RenderTarget[0];
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

	// 深度バッファのフォーマット
	pipelineDesc_.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	//頂点レイアウトの設定
	pipelineDesc_.InputLayout.pInputElementDescs = inputLayout_;
	pipelineDesc_.InputLayout.NumElements = _countof(inputLayout_);

	//三角形に設定
	pipelineDesc_.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;

	//その他の設定
	pipelineDesc_.NumRenderTargets = 1;//描画対象は1つ
	pipelineDesc_.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//0～255指定のRGBA
	pipelineDesc_.SampleDesc.Count = 1;//1ピクセルにつき1回サンプリング
}

void ParticleManager::GenerateRootSignature() {
	HRESULT result = S_FALSE;

	//デスクリプタレンジの設定
	D3D12_DESCRIPTOR_RANGE descriptorRange{};
	descriptorRange.NumDescriptors = 1;
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0;//テクスチャレジスタ0番
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//ルートパラメータの設定
	D3D12_ROOT_PARAMETER rootParams[kRootParamCount] = {};
	//定数バッファ0番
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//種類
	rootParams[0].Descriptor.ShaderRegister = 0;					//定数バッファ番号
	rootParams[0].Descriptor.RegisterSpace = 0;						//デフォルト値
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//全てのシェーダから見える
	//テクスチャレジスタ0番
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	//種類
	rootParams[1].DescriptorTable.pDescriptorRanges = &descriptorRange;			//デスクリプタレンジ
	rootParams[1].DescriptorTable.NumDescriptorRanges = 1;						//デスクリプタレンジ数
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;				//全てのシェーダから見える

	//テクスチャサンプラーの設定
	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
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

	// ルートシグネチャの生成
	result = device_->CreateRootSignature(
		0,
		rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature_)
	);
	assert(SUCCEEDED(result));

	pipelineDesc_.pRootSignature = rootSignature_.Get();
}

void ParticleManager::GeneratePipelineState() {
	HRESULT result = S_FALSE;

	// グラフィックスパイプラインの生成
	result = device_->CreateGraphicsPipelineState(
		&pipelineDesc_,
		IID_PPV_ARGS(&pipelineState_)
	);
	assert(SUCCEEDED(result));
}

bool ParticleManager::Initialize() {
	HRESULT result;

	// nullptrチェック
	assert(device_);

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
	result = device_->CreateCommittedResource(
		&heapProps, // アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff_));
	assert(SUCCEEDED(result));

	return true;
}

void ParticleManager::Update() {
	HRESULT result;

	particle_->Update();
	Matrix4 matVP = camera_->GetViewProjectionMatrix();
	Matrix4 matBillboard = camera_->GetBillboardMatrix();

	// 定数バッファへデータ転送
	ConstBufferData* constMap = nullptr;
	result = constBuff_->Map(0, nullptr, (void**)&constMap);
	constMap->mat = matVP;	// 行列の合成
	constMap->matBillboard = matBillboard;
	constBuff_->Unmap(0, nullptr);
}

void ParticleManager::Draw() {
	// nullptrチェック
	assert(device_);
	assert(ParticleManager::cmdList_);

	// 定数バッファビューをセット
	cmdList_->SetGraphicsRootConstantBufferView(
		0,
		constBuff_->GetGPUVirtualAddress()
	);

	particle_->Draw(cmdList_.Get());
}

void ParticleManager::Active(
	Particle* p,
	const Vector3& setMove,
	const Vector3& setPos,
	const Vector3& setVel,
	const Vector3& setAcc,
	const int& setQua,
	const float& scaleStart,
	const float& scaleEnd,
	const int& life) {
	for (int i = 0; i < setQua; i++) {
		//位置座標をランダムに分布
		Vector3 pos{};
		pos.x = RandomOutput(-setPos.x / 2, setPos.x / 2) + setMove.x;
		pos.y = RandomOutput(-setPos.y / 2, setPos.y / 2) + setMove.y;
		pos.z = RandomOutput(-setPos.z / 2, setPos.z / 2) + setMove.z;

		//速度をランダムに分布
		Vector3 vel{};
		vel.x = RandomOutput(-setVel.x / 2, setVel.x / 2);
		vel.y = RandomOutput(-setVel.y / 2, setVel.y / 2);
		vel.z = RandomOutput(-setVel.z / 2, setVel.z / 2);

		//重力に見立てて加速度のYのみ別枠でランダムに分布
		Vector3 acc{};
		acc.y = RandomOutput(-setAcc.y / 2, setAcc.y / 2);

		//追加
		p->Add(life, pos, vel, acc, scaleStart, scaleEnd);
	}
}

ParticleManager* ParticleManager::GetInstance() {
	static ParticleManager instance;
	return &instance;
}
