#include "Object3d.h"
#include <d3dcompiler.h>
#include <DirectXTex.h>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "MyMath.h"
#include "WinApp.h"
#include "Model.h"

#include "BaseCollider.h"
#include "CollisionManager.h"

#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace std;

/// <summary>
/// 静的メンバ変数の実体
/// </summary>
ID3D12Device* Object3d::device_ = nullptr;
ID3D12GraphicsCommandList* Object3d::cmdList_ = nullptr;
ComPtr<ID3D12RootSignature> Object3d::rootsignature_;
Object3d::PipelineSet Object3d::pipelineSet_;

LightGroup* Object3d::lightGroup_ = nullptr;

void Object3d::StaticInitialize(ID3D12Device* device) {
	// nullptrチェック
	assert(device);

	Object3d::device_ = device;

	// パイプライン初期化
	InitializeGraphicsPipeline();

	Model::StaticInitialize(device);
}

void Object3d::PreDraw(ID3D12GraphicsCommandList* cmdList) {
	// PreDrawとPostDrawがペアで呼ばれていなければエラー
	assert(Object3d::cmdList_ == nullptr);

	// コマンドリストをセット
	Object3d::cmdList_ = cmdList;

	// プリミティブ形状を設定
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Object3d::PostDraw() {
	// コマンドリストを解除
	Object3d::cmdList_ = nullptr;
}

Object3d* Object3d::Create() {

	// 3Dオブジェクトのインスタンスを生成
	Object3d* object3d = new Object3d();
	if (object3d == nullptr) {
		return nullptr;
	}

	// 初期化
	if (!object3d->Initialize()) {
		delete object3d;
		assert(0);
		return nullptr;
	}

	//スケールのセット
	float scale_val = 5;
	object3d->worldTransform_.scale_ = { scale_val,scale_val,scale_val };

	return object3d;
}

void Object3d::InitializeGraphicsPipeline() {
	HRESULT result = S_FALSE;
	ComPtr<ID3DBlob> vsBlob; // 頂点シェーダオブジェクト
	ComPtr<ID3DBlob> psBlob;	// ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob; // エラーオブジェクト

								// 頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resource/shader/ObjVS.hlsl",	// シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "vs_5_0",	// エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&vsBlob, &errorBlob);
	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	// ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resource/shader/ObjPS.hlsl",	// シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "ps_5_0",	// エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&psBlob, &errorBlob);
	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	// 頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{ // xy座標(1行で書いたほうが見やすい)
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // 法線ベクトル(1行で書いたほうが見やすい)
			"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // uv座標(1行で書いたほうが見やすい)
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	// グラフィックスパイプラインの流れを設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};
	//シェーダの設定
	pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

	//サンプルマスクの設定
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;//標準設定

														//ラスタライザの設定
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//カリングしない
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;//ポリゴン内塗りつぶし
	pipelineDesc.RasterizerState.DepthClipEnable = true;//深度クリッピングを有効に
														// デプスステンシルステート
	pipelineDesc.DepthStencilState.DepthEnable = true;
	pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RBGA全てのチャンネルを描画
																	//共通設定
	blenddesc.BlendEnable = true;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	//半透明合成
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	// ブレンドステートの設定
	pipelineDesc.BlendState.RenderTarget[0] = blenddesc;

	// 深度バッファのフォーマット
	pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// 頂点レイアウトの設定
	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

	// 図形の形状設定（三角形）
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	pipelineDesc.NumRenderTargets = 1;	// 描画対象は1つ
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0～255指定のRGBA
	pipelineDesc.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

									   //デスクリプタレンジの設定
	D3D12_DESCRIPTOR_RANGE descriptorRange{};
	descriptorRange.NumDescriptors = 1;
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0;//テクスチャレジスタ0番
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//ルートパラメータの設定
	D3D12_ROOT_PARAMETER rootParams[4] = {};
	//定数バッファ0番
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//種類
	rootParams[0].Descriptor.ShaderRegister = 0;					//定数バッファ番号
	rootParams[0].Descriptor.RegisterSpace = 0;						//デフォルト値
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//全てのシェーダから見える
																	//定数バッファ1番
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//種類
	rootParams[1].Descriptor.ShaderRegister = 1;					//定数バッファ番号
	rootParams[1].Descriptor.RegisterSpace = 0;						//デフォルト値
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//全てのシェーダから見える
																	//テクスチャレジスタ0番
	rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	//種類
	rootParams[2].DescriptorTable.pDescriptorRanges = &descriptorRange;			//デスクリプタレンジ
	rootParams[2].DescriptorTable.NumDescriptorRanges = 1;						//デスクリプタレンジ数
	rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;				//全てのシェーダから見える
																				//定数バッファ2番
	rootParams[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//種類
	rootParams[3].Descriptor.ShaderRegister = 2;					//定数バッファ番号
	rootParams[3].Descriptor.RegisterSpace = 0;						//デフォルト値
	rootParams[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//全てのシェーダから見える

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

	ComPtr<ID3DBlob> rootSigBlob;
	// バージョン自動判定のシリアライズ
	result = D3D12SerializeRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob,
		&errorBlob);
	assert(SUCCEEDED(result));

	// ルートシグネチャの生成
	result = device_->CreateRootSignature(
		0, rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(), 
		IID_PPV_ARGS(&pipelineSet_.rootsignature_));
	assert(SUCCEEDED(result));

	//パイプラインにルートシグネチャをセット
	pipelineDesc.pRootSignature = pipelineSet_.rootsignature_.Get();

	// グラフィックスパイプラインの生成
	result = device_->CreateGraphicsPipelineState(
		&pipelineDesc,
		IID_PPV_ARGS(&pipelineSet_.pipelinestate_));
	assert(SUCCEEDED(result));
}

bool Object3d::Initialize() {
	worldTransform_.Initialize();

	//クラス名の文字列を取得
	name_ = typeid(*this).name();

	return true;
}

void Object3d::Update() {
	assert(camera_);

	worldTransform_.UpdateMatrix();

	//定数バッファへ転送
	TransferMatrixWorld();

	//衝突判定更新
	if (collider_) {
		collider_->Update();
	}
}

void Object3d::Draw() {
	// nullptrチェック
	assert(device_);
	assert(Object3d::cmdList_);

	if (model_ == nullptr) return;

	// パイプラインステートの設定
	cmdList_->SetPipelineState(pipelineSet_.pipelinestate_.Get());
	// ルートシグネチャの設定
	cmdList_->SetGraphicsRootSignature(pipelineSet_.rootsignature_.Get());
	// 定数バッファビューをセット
	cmdList_->SetGraphicsRootConstantBufferView(0, worldTransform_.constBuff_->GetGPUVirtualAddress());

	//ライト描画
	lightGroup_->Draw(cmdList_, 3);

	model_->Draw(cmdList_);
}

void Object3d::Draw(const WorldTransform& worldTransform)
{
	worldTransform_ = worldTransform;
	// nullptrチェック
	assert(device_);
	assert(Object3d::cmdList_);

	if (model_ == nullptr) return;

	// パイプラインステートの設定
	cmdList_->SetPipelineState(pipelineSet_.pipelinestate_.Get());
	// ルートシグネチャの設定
	cmdList_->SetGraphicsRootSignature(pipelineSet_.rootsignature_.Get());
	// 定数バッファビューをセット
	cmdList_->SetGraphicsRootConstantBufferView(0, worldTransform_.constBuff_->GetGPUVirtualAddress());

	//ライト描画
	lightGroup_->Draw(cmdList_, 3);

	model_->Draw(cmdList_);
}

void Object3d::TransferMatrixWorld() {
	const Matrix4& matViewProjection = camera_->GetViewProjectionMatrix();
	const Vector3& cameraPos = camera_->GetEye();

	worldTransform_.constMap_->viewproj_ = matViewProjection;
	worldTransform_.constMap_->world_ = worldTransform_.matWorld_;
	worldTransform_.constMap_->cameraPos_ = cameraPos;
}

void Object3d::OnCollision(const CollisionInfo& info) {
#ifdef _DEBUG
	CollisionInfo info_ = info;
#endif // DEBUG
}

void Object3d::SetCollider(BaseCollider* collider) {
	collider->SetObject(this);
	collider_ = collider;

	//衝突マネージャーに登録
	CollisionManager::GetInstance()->AddCollider(collider);
	//コライダーの更新
	collider->Update();
}

Object3d::~Object3d() {
	if (collider_) {
		//衝突マネージャーから登録を解除
		CollisionManager::GetInstance()->RemoveCollider(collider_);
		delete collider_;
	}
}
