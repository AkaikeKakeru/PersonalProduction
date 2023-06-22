#pragma once
#include "DirectXBasis.h"

class ImGuiManager {
private://省略
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	static void StaticInitialize();
	void Initialize(DirectXBasis* dXBas);
	void Finalize();

	//ImGui受付開始
	void Begin();
	//ImGui受付終了
	void End();
	//画面への描画
	void Draw();

public:
	//DirectX基盤
	static DirectXBasis* dXBas_;

	//SRV用デスクリプタヒープ
	static ComPtr<ID3D12DescriptorHeap> srvHeap_;

	//DirectX基盤
	//DirectXBasis* dXBas_;

	//SRV用デスクリプタヒープ
	//ComPtr<ID3D12DescriptorHeap> srvHeap_;
private:
	ImGuiManager() = default;
	~ImGuiManager() = default;
	//コピーコンストラクタを無効
	ImGuiManager(const ImGuiManager&) = delete;
	//代入演算子を無効
	const ImGuiManager& operator=(const ImGuiManager&) = delete;

public://static
	static ImGuiManager* GetInstance();
};