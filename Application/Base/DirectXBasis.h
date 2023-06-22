#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <vector>

#include "WinApp.h"
#include "FixFPS.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

class DirectXBasis{
private://省略
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public://基本関数	
	void Initialize(WinApp * winApp);
	void PreDraw();
	void PostDraw();

private://固有関数
	///初期化
	//デバイス
	void InitDevice();
	//コマンド
	void InitCommand();
	//スワップチェーン
	void InitSwapChain();
	//レンダ―ターゲットビュー
	void InitRenderTargetView();
	//深度バッファ
	void InitDepthBuffer();
	//フェンス
	void InitFence();

public://アクセス
	//デバイス取得
	ComPtr<ID3D12Device> GetDevice() { return device_; }
	//コマンドリスト取得
	ComPtr<ID3D12GraphicsCommandList> GetCommandList() { return cmdList_; }

	//バックバッファの数取得
	size_t GetBackBufferCount() const { return backBuffers_.size(); }

public://定数
	static const int BackBufferCount = 2;

private://変数
	WinApp* winApp_ = nullptr;
	FixFPS* fixFPS_ = nullptr;

	//デバッグレイヤー
	ID3D12Debug1* debugController_{};
	//自動ブレーク
	ID3D12InfoQueue* infoQueue_{};

	/// <summary>
	/// 重要生成物
	/// </summary>
	ComPtr<ID3D12Device> device_;
	ComPtr<IDXGIFactory7> dxgiFactory_;
	ComPtr<IDXGISwapChain4> swapChain_;
	ComPtr<ID3D12CommandAllocator> cmdAllocator_;
	ComPtr<ID3D12GraphicsCommandList> cmdList_;
	ComPtr<ID3D12CommandQueue> cmdQueue_;
	ComPtr<ID3D12DescriptorHeap> rtvHeap_;
	std::vector<ComPtr<ID3D12Resource>> backBuffers_;
	ComPtr<ID3D12Fence> fence_;


	ComPtr<ID3D12Resource> depthBuff_;
	ComPtr<ID3D12DescriptorHeap> dsvHeap_;
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc_ = {};

	/// <summary>
	/// 重要生成物の設定用
	/// </summary>
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc_{};
	D3D12_RESOURCE_BARRIER barrierDesc_{};
	UINT64 fenceVal_ = 0;

private:
	DirectXBasis() = default;
	~DirectXBasis() = default;
	//コピーコンストラクタを無効
	DirectXBasis(const DirectXBasis&) = delete;
	//代入演算子を無効
	const DirectXBasis& operator=(const DirectXBasis&) = delete;

public://static
	static DirectXBasis* GetInstance();
};