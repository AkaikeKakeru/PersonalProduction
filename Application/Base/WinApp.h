#pragma once
#include <Windows.h>

class WinApp {
public://static
	   //プロシージャ
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public://基本関数	
	void Initialize();

public://固有関数
	   //メッセージ処理
	bool ProcessMessage();

public://アクセス
	HWND GetHWND() const { return hwnd_; }
	HINSTANCE GetHInstance() const { return w_.hInstance; }
public://定数
	   //横幅
	static const int Win_Width = 1280;
	//縦幅
	static const int Win_Height = 720;

private:
	HWND hwnd_;
	WNDCLASSEX w_{};
private:
	WinApp() = default;
	~WinApp() = default;
	WinApp(const WinApp&) = delete;
	const WinApp& operator=(const WinApp&) = delete;
public:
	static WinApp* GetInstance();

};