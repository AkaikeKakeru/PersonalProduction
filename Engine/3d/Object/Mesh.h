#pragma once

#include "Material.h"
#include <Windows.h>
#include <d3d12.h>
#include <vector>
#include <wrl.h>

#include "Vector3.h"
#include "Vector2.h"
#include <unordered_map>

class Mesh {
private: // エイリアス
	//省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public://サブクラス
	// 頂点データ構造体
	struct VertexPosNormalUv {
		Vector3 pos; // xyz座標
		Vector3 normal; // 法線ベクトル
		Vector2 uv;  // uv座標
	};

public: // 静的メンバ関数
	// 静的初期化
	static void StaticInitialize(ID3D12Device* device);

public: //メンバ関数
	//バッファ生成
	void CreateBuffers();

	//平滑化された頂点法線の計算
	void CalculateSmoothVertexNormals();

	/// 名前を取得
	const std::string& GetName() { return name_; }
	/// マテリアルの取得
	Material* GetMaterial() { return material_; }
	// 頂点バッファ取得
	const D3D12_VERTEX_BUFFER_VIEW& GetVBView() { return vbView_; }
	// インデックスバッファ取得
	const D3D12_INDEX_BUFFER_VIEW& GetIBView() { return ibView_; }
	//頂点データの数を取得
	inline size_t GetVertexCount() { return vertices_.size(); }

	/// 名前をセット
	void SetName(const std::string& name) { name_ = name; };
	/// マテリアルの割り当て
	void SetMaterial(Material* material) { material_ = material; };

	/// 頂点データの追加
	void AddVertex(const VertexPosNormalUv& vertex) {
		vertices_.emplace_back(vertex);
	}
	/// 頂点インデックスの追加
	void AddIndex(unsigned short index) {
		indices_.emplace_back(index);
	};
	//エッジ平滑化データの追加
	void AddSmoothData(unsigned short indexPosition, unsigned short indexVertex) {
		smoothData[indexPosition].emplace_back(indexVertex);
	};

	// 描画
	void Draw(ID3D12GraphicsCommandList* cmdList);

private: // 静的メンバ変数
	// デバイス
	static ComPtr<ID3D12Device> device_;

private://メンバ変数
	// 名前
	std::string name_;
	// 頂点バッファ
	ComPtr<ID3D12Resource> vertBuff_;
	// インデックスバッファ
	ComPtr<ID3D12Resource> indexBuff_;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_{};
	// インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView_{};

	// 頂点データ配列
	std::vector<VertexPosNormalUv> vertices_;
	// 頂点インデックス配列
	std::vector<unsigned short> indices_;

	//マテリアル
	Material* material_{};

	//頂点法線スムージング用データ
	std::unordered_map<unsigned short, std::vector<unsigned short>> smoothData;
};