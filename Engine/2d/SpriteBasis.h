#pragma once
#include "DirectXBasis.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include <string>
#include <array>

class SpriteBasis {
private://省略
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

private://構造体

public://基本関数

	static void Initialize();
	static void PreDraw();
	static void PostDraw();

public://publicな固有関数
	//テクスチャ読み込み
	void LoadTexture(uint32_t textureIndex, const std::string& fileName);
	//描画用テクスチャコマンド
	void SetTextureCommand(uint32_t textureIndex);

private://固有関数
	/// <summary>
	/// パイプライン
	/// </summary>

	//シェーダーファイルのコンパイル
	static void CompileShaderFile();
	//頂点レイアウト組み立て
	static void AssembleVertexLayout();
	//グラフィックスパイプライン作成
	static void CreateGraphicsPipeline();
	//グラフィックスパイプライン組み立て
	static void AssembleGraphicsPipeline();
	//ルートシグネイチャ生成
	static void GenerateRootSignature();
	//パイプラインステート生成
	static void GeneratePipelineState();

	/// <summary>
	/// テクスチャ
	/// </summary>

	//テクスチャバッファ生成
	void GenerateTextureBuffer(uint32_t textureIndex, const wchar_t* wfileName);
	//デスクリプタヒープ生成
	static void GenerateDescriptorHeap();
	//シェーダーリソースビュー作成
	void CreateShaderResourceView(uint32_t textureIndex);

private://定数
	//頂点レイアウトの要素数
	static const int kInputLayoutElement = 2;
	//ルートパラメータ数
	static const int kRootParamCount = 3;

	//SRVの最大個数
	static const size_t kMaxSRVCount = 2056;
	//デフォルトテクスチャ格納ディレクトリ
	static std::string kDefaultTextureDirectoryPath;

private://メンバ変数
	//Draw基盤
	static DirectXBasis* dxBas_;

	//デバイス
	static ComPtr<ID3D12Device> device_;
	//コマンドリスト
	static ComPtr<ID3D12GraphicsCommandList> cmdList_;

	/// <summary>
	/// パイプライン
	/// </summary>

	//頂点シェーダオブジェクト
	static ComPtr<ID3DBlob> vsBlob_;
	//ピクセルシェーダオブジェクト
	static ComPtr<ID3DBlob> psBlob_;
	//エラーオブジェクト
	static ComPtr<ID3DBlob> errorBlob_;
	//頂点レイアウト
	static D3D12_INPUT_ELEMENT_DESC inputLayout_[kInputLayoutElement];
	//グラフィックスパイプラインデスク
	static D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc_;
	//ルートシグネイチャ
	static ComPtr<ID3D12RootSignature> rootSignature_;
	//パイプラインステート
	static ComPtr<ID3D12PipelineState> pipelineState_;

	/// <summary>
	/// テクスチャ
	/// </summary>

	//テクスチャバッファ
	std::array<ComPtr<ID3D12Resource>, kMaxSRVCount> texBuffs_;
	//テクスチャリソースデスク
	D3D12_RESOURCE_DESC texResDesc_{};
	//デスクリプタヒープ
	static ComPtr<ID3D12DescriptorHeap> srvHeap_;
	//SRVヒープのアドレス
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle_;
	//インクリメントサイズ
	static UINT incrementSize_;
	//テクスチャ格納ディレクトリ
	static std::string textureDhirectoryPath_;

public://ゲッタ
	ComPtr<ID3D12Device> GetDevice() { return device_; }
	ComPtr<ID3D12GraphicsCommandList> GetCommandList() { return cmdList_; }
	ID3D12Resource* GetTextureBuffer(uint32_t index) const { return texBuffs_[index].Get(); }

private:
	SpriteBasis() = default;
	~SpriteBasis() = default;
	//コピーコンストラクタを無効
	SpriteBasis(const SpriteBasis&) = delete;
	//代入演算子を無効
	const SpriteBasis& operator=(const SpriteBasis&) = delete;

public://static
	static SpriteBasis* GetInstance();
};