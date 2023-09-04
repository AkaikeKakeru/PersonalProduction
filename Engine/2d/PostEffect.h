#pragma once
#include "SpriteBasis.h"
#include <Matrix4.h>
#include <MyMath.h>

class PostEffect {
private://省略
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public://構造体
	//定数バッファ用データ構造体(マテリアル)
	typedef struct ConstBufferDataMaterial {
		Vector4 color_ = { 0,0,0,0 };//色(RGBA)
	}ConstBufferDataMaterial;

	//定数バッファ用データ構造体(3D変換行列)
	typedef struct ConstBufferDataTransform {
		Matrix4 mat_ = Matrix4Identity();//3D変換行列
	}ConstBufferDataTransform;

	//ワールド変換構造体
	typedef struct WorldTransform {
		Vector3 scale_ = { 0,0,0 };		//スケール
		float rotation_ = 0.0f;	//回転
		Vector2 position_ = { 0,0 };//平行移動
		Matrix4 matWorld_ = Matrix4Identity();	//ワールド変換行列
	}WorldTransform;

	//頂点データ構造体
	typedef struct VertexPosUv {
		Vector3 pos = { 0,0,0 };//xyz座標
		Vector2 uv = { 0,0 };//uv座標
	}Vertex;

	typedef struct TopBottomLeftRight {
		float left;//左
		float right;//右
		float top;//上
		float bottom;//下
	}Direction;

	//頂点部位
	typedef enum VerticesParts {
		LeftBottom,//左下
		LeftTop,//左上
		RightBottom,//右下
		RightTop,//右上
	}VerticesParts;

	typedef struct Texture {
		//上下左右
		Direction dir_ = { 0,0,0,0 };

		//色
		Vector4 color_ = { 1,1,1,1 };

		//アンカーポイント
		Vector2 anchorPoint_ = { 0,0 };

		//非表示フラグ
		bool isInvisible_ = false;
	}Texture;

	//ルートパラメータEnum
	enum RootParameterState {
		rootParam_Material,
		rootParam_Texture,
		rootParam_Transform,
		rootParam_SubTexture
	};

public://メンバ関数
	//初期化
	void Initialize();
	//描画コマンド実行
	void Draw();

	//頂点バッファビュー作成
	void CreateVertexBufferView();
	//定数バッファ生成
	void GenerateConstBuffer();
	//定数バッファマテリアル生成
	void GenerateConstMaterial();
	//定数バッファトランスフォーム生成
	void GenerateConstTransform();

	//パイプライン生成
	void CreateGraphicsPipeLineState();
	//シェーダーファイルのコンパイル
	void CompileShaderFile();
	//頂点レイアウト組み立て
	void AssembleVertexLayout();
	//グラフィックスパイプライン組み立て
	void AssembleGraphicsPipeline();
	//ルートシグネイチャ生成
	void GenerateRootSignature();
	//パイプラインステート生成
	void GeneratePipelineState();

	//テクスチャ描画事前処理
	void PreDraw();
	//テクスチャセットコマンド
	void TextureCommand();

	//シーン描画前処理
	void PreDrawScene();
	//シーン描画後処理
	void PostDrawScene();

	//テクスチャバッファ生成
	void GenerateTextureBuffer();
	//テクスチャバッファへ転送
	void TransferTextureBuffer();
	//SRV作成
	void CreateSRV();
	//RTV作成
	void CreateRTV();
	//深度バッファ生成
	void GenerateDepthBuffer();
	//DSV作成
	void CreateDSV();

public://アクセッサ
	   //非表示フラグを取得
	bool GetIsInvisible() const {
		return texture_.isInvisible_;
	}

	//非表示フラグをセット
	void SetIsInvisible(bool isInvisible) {
		texture_.isInvisible_ = isInvisible;
	}

private:
	//頂点数
	static const int kVerticesNum_ = 4;
	//頂点レイアウトの要素数
	static const int kInputLayoutElement_ = 2;
	//ルートパラメータ数
	static const int kRootParamCount_ = 4;
	//テクスチャバッファ数
	static const int kTextureBufferCount_ = 2;

private://静的メンバ変数
	//画面クリアカラー
	static const float clearColor_[4];

private://メンバ変数
	//テクスチャ
	Texture texture_;

	//デバイス
	ComPtr<ID3D12Device> device_;
	//コマンドリスト
	ComPtr<ID3D12GraphicsCommandList> cmdList_;

	//頂点データ
	Vertex vertices_[kVerticesNum_]{};
	//頂点データマップ
	Vertex* vertMap_ = nullptr;
	//頂点バッファ
	ComPtr<ID3D12Resource> vertBuff_;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_{};

	//定数バッファマテリアル
	ComPtr<ID3D12Resource> constBuffMaterial_;
	//定数バッファマテリアルマップ
	ConstBufferDataMaterial* constMapMaterial_ = nullptr;
	//定数バッファトランスフォーム
	ComPtr<ID3D12Resource> constBuffTransform_;
	//定数バッファマテリアルマップ
	ConstBufferDataTransform* constMapTransform_ = nullptr;

	//頂点シェーダオブジェクト
	ComPtr<ID3DBlob> vsBlob_;
	//ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> psBlob_;
	//エラーオブジェクト
	ComPtr<ID3DBlob> errorBlob_;
	//頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout_[kInputLayoutElement_];
	//グラフィックスパイプライン
	ComPtr<ID3D12PipelineState> pipeLineState_;
	//ルートシグネチャ
	ComPtr<ID3D12RootSignature> rootSignature_;
	//グラフィックスパイプラインデスク
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipeLineDesc_;

	//テクスチャバッファ
	ComPtr<ID3D12Resource> texBuff_[kTextureBufferCount_];
	//シェーダーリソースビュー用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapSRV_;

	//深度バッファ
	ComPtr<ID3D12Resource> depthBuff_;
	//レンダ―ターゲットビュー用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapRTV_;
	//深度ステンシル用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapDSV_;

	//リソースバリアデスク
	D3D12_RESOURCE_BARRIER barrierDesc_{};

public://コンストラクタ
	PostEffect();
};
