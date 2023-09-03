#pragma once

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include "Camera.h"
#include "Particle.h"

#include "Matrix4.h"
#include "Vector3.h"
#include "Vector2.h"

/// <summary>
/// 3Dオブジェクト
/// </summary>
class ParticleManager {
private: //省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public: // サブクラス
	// 定数バッファ用データ構造体
	struct ConstBufferData {
		//Vector4 color;	// 色 (RGBA)
		Matrix4 mat;// ３Ｄ変換行列
		Matrix4 matBillboard;	//ビルボード行列
	};

private://定数
	//頂点レイアウトの要素数
	static const int kInputLayoutElement = 2;
	//ルートパラメータ数
	static const int kRootParamCount = 3;

	//SRVの最大個数
	static const size_t kMaxSRVCount = 2056;
	//デフォルトテクスチャ格納ディレクトリ
	static std::string kDefaultTextureDirectoryPath;

public: // 静的メンバ関数
	/// <summary>
	/// 静的初期化
	/// </summary>
	static void StaticInitialize(ID3D12Device* device);

	/// <summary>
	/// 描画前処理
	/// </summary>
	/// <param name="cmdList">コマンドリスト</param>
	static void PreDraw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// 描画後処理
	/// </summary>
	static void PostDraw();

	/// <summary>
	/// 生成
	/// </summary>
	/// <returns></returns>
	static ParticleManager* Create();

private: // 静的メンバ変数
	// デバイス
	static ComPtr<ID3D12Device> device_;
	// コマンドリスト
	static ComPtr<ID3D12GraphicsCommandList> cmdList_;

	/// <summary>
	/// パイプライン
	/// </summary>
	// パイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState> pipelineState_;
	// ルートシグネチャ
	static ComPtr<ID3D12RootSignature> rootSignature_;

	//頂点シェーダオブジェクト
	static ComPtr<ID3DBlob> vsBlob_;
	//ジオメトリシェーダオブジェクト
	static ComPtr<ID3DBlob> gsBlob_;
	//ピクセルシェーダオブジェクト
	static ComPtr<ID3DBlob> psBlob_;
	//エラーオブジェクト
	static ComPtr<ID3DBlob> errorBlob_;
	//頂点レイアウト
	static D3D12_INPUT_ELEMENT_DESC inputLayout_[kInputLayoutElement];
	//グラフィックスパイプラインデスク
	static D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc_;

private:// 静的メンバ関数
	/// グラフィックパイプライン初期化

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

public: // メンバ関数
	//初期化
	bool Initialize();
	//更新
	void Update();
	//描画
	void Draw();

	/// <summary>
	/// パーティクル発射
	/// </summary>
	/// <param name="p">パーティクル</param>
	/// <param name="setMove">初期位置</param>
	/// <param name="setPos">位置</param>
	/// <param name="setVel">速度</param>
	/// <param name="setAcc">加速度(重力など)</param>
	/// <param name="setQua">個数</param>
	///  <param name="scaleStart">開始スケール</param>
	///  <param name="scaleEnd">終了スケール</param>
	///  <param name="life">持続</param>
	void Active(
		Particle* p,
		const Vector3& setmove,
		const Vector3& setpos,
		const Vector3& setvel,
		const Vector3& setacc,
		const int& setQua,
		const float& scaleStart,
		const float& scaleEnd,
		const int& life);

public://アクセッサ
	//パーティクルモデルのセット
	void SetParticleModel(Particle* pmodel) {
		particle_ = pmodel;
	}
	//カメラのセット
	void SetCamera(Camera* camera) {
		camera_ = camera;
	}

private: // メンバ変数
	// 定数バッファ
	ComPtr<ID3D12Resource> constBuff_;
	//スケール
	Vector3 scale_ = { 1.0f,1.0f,1.0f };

	//色
	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };

	//パーティクル
	Particle* particle_ = nullptr;

	//カメラ
	Camera* camera_ = nullptr;

private:
	ParticleManager() = default;
	~ParticleManager() = default;
	//コピーコンストラクタを無効
	ParticleManager(const ParticleManager&) = delete;
	//代入演算子を無効
	const ParticleManager& operator=(const ParticleManager&) = delete;

public://static
	static ParticleManager* GetInstance();
};
