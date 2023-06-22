#include "ImGuiManager.h"
#include <imgui_impl_win32.h>
#include <imGui_impl_DX12.h>
#include "WinApp.h"

template <class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

//DirectX基盤
DirectXBasis* ImGuiManager::dXBas_ = DirectXBasis::GetInstance();

//SRV用デスクリプタヒープ
ComPtr<ID3D12DescriptorHeap> ImGuiManager::srvHeap_;

ImGuiManager* ImGuiManager::GetInstance() {
	static ImGuiManager instance;
	return &instance;
}

void ImGuiManager::StaticInitialize() {

	HRESULT result;
	dXBas_ = DirectXBasis::GetInstance();

	//コンテキストを生成
	ImGui::CreateContext();
	//スタイルを設定
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(WinApp::GetInstance()->GetHWND());

	//デスクリプタヒープ設定
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	desc.NumDescriptors = 1;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	//デスクリプタヒープ生成
	result = dXBas_->GetDevice()->CreateDescriptorHeap(
		&desc, IID_PPV_ARGS(&srvHeap_));
	assert(SUCCEEDED(result));

	ImGui_ImplDX12_Init(
		dXBas_->GetDevice().Get(),
		static_cast<int>(dXBas_->GetBackBufferCount()),
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, srvHeap_.Get(),
		srvHeap_->GetCPUDescriptorHandleForHeapStart(),
		srvHeap_->GetGPUDescriptorHandleForHeapStart()
	);

	ImGuiIO& io = ImGui::GetIO();
	//標準フォント追加
	io.Fonts->AddFontDefault();
}

void ImGuiManager::Initialize(DirectXBasis* dXBas) {
	assert(dXBas);

	HRESULT result;
	dXBas_ = dXBas;

	//コンテキストを生成
	ImGui::CreateContext();
	//スタイルを設定
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(WinApp::GetInstance()->GetHWND());

	//デスクリプタヒープ設定
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	desc.NumDescriptors = 1;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	//デスクリプタヒープ生成
	result = dXBas_->GetDevice()->CreateDescriptorHeap(
		&desc, IID_PPV_ARGS(&srvHeap_));
	assert(SUCCEEDED(result));

	ImGui_ImplDX12_Init(
		dXBas_->GetDevice().Get(),
		static_cast<int>(dXBas_->GetBackBufferCount()),
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, srvHeap_.Get(),
		srvHeap_->GetCPUDescriptorHandleForHeapStart(),
		srvHeap_->GetGPUDescriptorHandleForHeapStart()
	);

	ImGuiIO& io = ImGui::GetIO();
	//標準フォント追加
	io.Fonts->AddFontDefault();
}

void ImGuiManager::Finalize() {
	//後始末
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	//デスクリプタヒープ解放
	srvHeap_.Reset();
}

void ImGuiManager::Begin() {
	//ImGuiフレーム開始
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiManager::End() {
	//描画前準備
	ImGui::Render();
}

void ImGuiManager::Draw() {
	ID3D12GraphicsCommandList* cmdList = dXBas_->GetCommandList().Get();

	//デスクリプタヒープの配列をセットするコマンド
	ID3D12DescriptorHeap* ppHeaps[] = { srvHeap_.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	//描画コマンド発行
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmdList);
}
