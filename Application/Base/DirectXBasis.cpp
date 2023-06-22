#include "DirectXBasis.h"
#include <string>
#include <cassert>

DirectXBasis* DirectXBasis::GetInstance() {
	static DirectXBasis instance;
	return &instance;
}

void DirectXBasis::Initialize(WinApp* winApp) {
	assert(winApp);
	winApp_ = winApp;

	fixFPS_ = FixFPS::GetInstace();
	fixFPS_->Initialize();

	InitDevice();
	InitCommand();
	InitSwapChain();
	InitRenderTargetView();
	InitDepthBuffer();
	InitFence();
}

void DirectXBasis::InitDevice() {
	HRESULT result;

#ifdef _DEBUG
	//デバッグレイヤをオンに
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController_)))) {
		debugController_->EnableDebugLayer();
		debugController_->SetEnableGPUBasedValidation(TRUE);
	}
#endif

//アダプタの列挙
	//DXGIファクトリ生成
	result = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
	assert(SUCCEEDED(result));

	//アダプタの列挙用
	std::vector<ComPtr<IDXGIAdapter4>> adapters;
	//特定の名前を持つアダプターオブジェクトが入る
	ComPtr<IDXGIAdapter4> tmpAdapter;

	//パフォーマンスが高いものから順に、全てのアダプタを列挙
	for (UINT i = 0;
		dxgiFactory_->EnumAdapterByGpuPreference(i,
			DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
			IID_PPV_ARGS(&tmpAdapter)) != DXGI_ERROR_NOT_FOUND;
		i++) {
		//動的配列に追加
		adapters.push_back(tmpAdapter);
	}

	//アダプタ選別
	for (size_t i = 0; i < adapters.size(); i++) {
		DXGI_ADAPTER_DESC3 adapterDesc;
		//アダプタ情報更新
		adapters[i]->GetDesc3(&adapterDesc);

		//ソフトウェアデバイスを回避
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			//デバイスを採用してループを抜ける
			tmpAdapter = adapters[1];
			break;
		}
	}

//デバイス生成
	//対応レベルの配列
	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D_FEATURE_LEVEL featureLevel;

	for (size_t i = 0; i < _countof(levels); i++) {
		//採用したアダプタでデバイス生成
		result = D3D12CreateDevice(
			tmpAdapter.Get(), levels[i],
			IID_PPV_ARGS(&device_));
		if (result == S_OK) {
			//デバイスを生成できた時点でループを抜ける
			featureLevel = levels[i];
			break;
		}
	}

	//エラー発生時に強制ブレークを掛ける
	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue_)))) {

		infoQueue_->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		infoQueue_->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);

		infoQueue_->Release();
	}
}

void DirectXBasis::InitCommand() {
	HRESULT result;
//コマンドアロケータ
	//コマンドアロケータ生成
	result = device_->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&cmdAllocator_));
	assert(SUCCEEDED(result));

//コマンドリスト
	//コマンドリスト生成
	result = device_->CreateCommandList(0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		cmdAllocator_.Get(), nullptr,
		IID_PPV_ARGS(&cmdList_));
	assert(SUCCEEDED(result));

//コマンドキュー
	//コマンドキューの設定
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
	//コマンドキューの生成
	result = device_->CreateCommandQueue(
		&cmdQueueDesc, IID_PPV_ARGS(&cmdQueue_));
	assert(SUCCEEDED(result));
}

void DirectXBasis::InitSwapChain() {
	HRESULT result;
	//スワップチェーンの設定
	swapChainDesc_.Width = winApp_->Win_Width;
	swapChainDesc_.Height = winApp_->Win_Height;
	swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc_.SampleDesc.Count = 1;
	swapChainDesc_.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	swapChainDesc_.BufferCount = 2;
	swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc_.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	//IDXGISwapChain1のComPtr
	ComPtr<IDXGISwapChain1> swapChain1;
	//スワップチェーンの生成
	result = dxgiFactory_->CreateSwapChainForHwnd(
		cmdQueue_.Get(),
		winApp_->GetHWND(),
		&swapChainDesc_,
		nullptr,
		nullptr,
		&swapChain1);

	//IDXGISwapChain4に変換
	swapChain1.As(&swapChain_);
	assert(SUCCEEDED(result));
}

void DirectXBasis::InitRenderTargetView() {
	//レンダ―ターゲットビュー(RTV)は、デスクリプタヒープに生成する
//デスクリプタヒープ
	//デスクリプタヒープの設定
	rtvHeapDesc_.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc_.NumDescriptors = swapChainDesc_.BufferCount;

	//デスクリプタヒープの生成
	device_->CreateDescriptorHeap(&rtvHeapDesc_, IID_PPV_ARGS(&rtvHeap_));

//レンダーターゲットビュー
	//バックバッファのリサイズ
	backBuffers_.resize(swapChainDesc_.BufferCount);

	//スワップチェーンの全てのバッファについて処理する
	for (size_t i = 0; i < backBuffers_.size(); i++) {
		//スワップチェーンからバッファを取得
		swapChain_->GetBuffer((UINT)i, IID_PPV_ARGS(&backBuffers_[i]));
		//デスクリプタヒープのハンドルを取得
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle =
			rtvHeap_->GetCPUDescriptorHandleForHeapStart();
		//裏か表かでアドレスがズレる
		rtvHandle.ptr += i * device_->GetDescriptorHandleIncrementSize(rtvHeapDesc_.Type);
		//レンダ―ターゲットビューの設定
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		//シェーダーの計算結果をSRGBに変換して書き込む
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		//レンダ―ターゲットビューの生成
		device_->CreateRenderTargetView(backBuffers_[i].Get(), &rtvDesc, rtvHandle);
	}
}

void DirectXBasis::InitDepthBuffer() {
	HRESULT result;

//深度バッファ
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

//深度デスクリプタヒープ
	//深度ビュー用デスクリプタヒープ作成
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	result = device_->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap_));

	//深度ビュー作成
	dsvDesc_.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc_.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	device_->CreateDepthStencilView(
		depthBuff_.Get(),
		&dsvDesc_,
		dsvHeap_.Get()->GetCPUDescriptorHandleForHeapStart());
}

void DirectXBasis::InitFence() {
	HRESULT result;
	//フェンスの生成
	result = device_->CreateFence(fenceVal_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
}

void DirectXBasis::PreDraw() {
	//バックバッファの番号取得
	UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();

//リソースバリアの変更コマンド
	///1.書き込み可能に
	barrierDesc_.Transition.pResource = backBuffers_[bbIndex].Get();
	barrierDesc_.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrierDesc_.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	cmdList_->ResourceBarrier(1, &barrierDesc_);

//描画先の変更指定コマンド
	///2.描画先の変更
	//レンダ―ターゲットビューのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle =
		rtvHeap_->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += bbIndex * device_->GetDescriptorHandleIncrementSize(rtvHeapDesc_.Type);
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap_->GetCPUDescriptorHandleForHeapStart();
	cmdList_->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

//画面クリアコマンド
	///3.画面クリア            R,     G,    B,    A
	FLOAT clearColor[] = { 0.1f, 0.25f, 0.5f, 0.0f };
	cmdList_->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	cmdList_->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	///4.描画コマンドスタート↓
	//ビューポート設定コマンド
	D3D12_VIEWPORT viewport{};
	viewport.Width = winApp_->Win_Width;
	viewport.Height = winApp_->Win_Height;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	//ビューポート設定コマンドを、コマンドリストに積む
	cmdList_->RSSetViewports(1, &viewport);

	//シザー矩形の設定コマンド
	D3D12_RECT scissorRect{};
	scissorRect.left = 0;										//切り抜き座標左
	scissorRect.right = scissorRect.left + winApp_->Win_Width;	//切り抜き座標右
	scissorRect.top = 0;										//切り抜き座標上
	scissorRect.bottom = scissorRect.top + winApp_->Win_Height;	//切り抜き座標下
	//シザー矩形設定コマンドを、コマンドリストに積む
	cmdList_->RSSetScissorRects(1, &scissorRect);
}

void DirectXBasis::PostDraw() {
	///4.描画コマンドエンド↑

	HRESULT result;

	//バックバッファの番号取得
	//UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();

//リソースバリア復帰コマンド
	///5.リソースバリアを隠す
	barrierDesc_.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrierDesc_.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	cmdList_->ResourceBarrier(1, &barrierDesc_);

//コマンドの実行
	//命令のクローズ
	result = cmdList_->Close();
	assert(SUCCEEDED(result));
	//コマンドリストの実行
	ID3D12CommandList* cmdLists[] = { cmdList_.Get() };
	cmdQueue_->ExecuteCommandLists(1, cmdLists);

	//画面に表示するバッファをフリップ
	result = swapChain_->Present(1, 0);
	assert(SUCCEEDED(result));

//コマンド完了待ち
	//コマンドの実行完了を待つ
	cmdQueue_->Signal(fence_.Get(), ++fenceVal_);
	if (fence_->GetCompletedValue() != fenceVal_) {
		_Post_ _Notnull_ HANDLE event = CreateEvent(nullptr, false, false, nullptr);

		if (event != 0) {
			fence_->SetEventOnCompletion(fenceVal_, event);
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}
	}

	//FPSを固定
	fixFPS_->Update();

//コマンドリストをリセット
	//キューをクリア
	result = cmdAllocator_->Reset();
	assert(SUCCEEDED(result));
	//再びコマンドリストを貯める準備
	result = cmdList_->Reset(cmdAllocator_.Get(), nullptr);
	assert(SUCCEEDED(result));
}