#pragma once
#include "Mesh.h"

#include "Matrix4.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include <unordered_map>

class Model {
private://省略
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public://静的メンバ関数
	   //静的初期化
	static void StaticInitialize(ID3D12Device* device);
	//OBJ読み込み
	static Model* LoadFromOBJ(const std::string& modelname, bool smoothing = false);

private://メンバ関数
		// テクスチャ読み込み
	void LoadTextures();

	// マテリアル読み込み
	void LoadMaterial(const std::string& directoryPath, const std::string& filename);

	// マテリアル登録
	void AddMaterial(Material* material) {
		// コンテナに登録
		materials_.emplace(material->name_, material);
	}

	void LoadFromOBJInternal(const std::string& modelname, bool smoothing = false);

	//デスクリプタヒープの初期化
	void InitializeDescriptorHeap();


public://メンバ関数
	   // デストラクタ
	~Model();

	//描画
	void Draw(ID3D12GraphicsCommandList* cmdList);

private:
	//デバイス
	static ComPtr<ID3D12Device> device_;
	// デスクリプタサイズ
	static UINT descriptorIncrementSize_;
	//ディレクトリパス
	static const std::string Directory_;

private:
	// 名前
	std::string name_;
	// メッシュコンテナ
	std::vector<Mesh*> meshes_;
	// マテリアルコンテナ
	std::unordered_map<std::string, Material*> materials_;
	// デフォルトマテリアル
	Material* defaultMaterial_ = nullptr;

	// デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeap_;
};