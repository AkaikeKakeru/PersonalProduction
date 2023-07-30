#include "Input.h"
#include <cassert>
#include <WinApp.h>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

void Input::Initialize() {
	GenerateDirectInput();
	GenerateKeyBoardDevice();
	GenerateMouseDevice();
}

void Input::Update() {
	//保存配列を上書きする前に、内容をPre保存配列にコピー
	memcpy(keyPre_, key_, sizeof(key_));
	mouseStatePre_ = mouseState_;

	//キーボード情報取得開始
	keyboard_->Acquire();
	//全キーの入力情報を取得する
	keyboard_->GetDeviceState(sizeof(key_), key_);

	//マウス情報取得開始
	mouse_->Acquire();
	//全マウスの入力情報を取得する
	mouse_->GetDeviceState(sizeof(mouseState_), &mouseState_);

	POINT mousePosition;
	//マウスのスクリーン座標を取得
	GetCursorPos(&mousePosition);

	//クライアントエリア座標に変換
	ScreenToClient(WinApp::GetInstance()->GetHWND(), &mousePosition);
	mousePos_.x = static_cast<float>(mousePosition.x);
	mousePos_.y = static_cast<float>(mousePosition.y);
}

bool Input::PressKey(BYTE keyNum) {
	//指定番号のキーが、押されているなら
	if (key_[keyNum]) {
		return true;
	}
	//それ以外はfalse
	return false;
}

bool Input::TriggerKey(BYTE keyNum) {
	//指定番号のキーが↓
	//1F前の時点で、押されていないなら
	if (!keyPre_[keyNum]) {
		//今は押されているなら
		if (key_[keyNum]) {
			return true;
		}
	}
	//それ以外はfalse
	return false;
}

bool Input::ReleaseKey(BYTE keyNum) {
	//指定番号のキーが↓
	//1F前の時点で、押されているなら
	if (keyPre_[keyNum]) {
		//今は押されていないなら
		if (!key_[keyNum]) {
			return true;
		}
	}
	//それ以外はfalse
	return false;
}

bool Input::PressMouse(int32_t buttonNum){
	//マウスデバイスのボタンに、指定番号がないならエラー
	assert(0 <= buttonNum && buttonNum < _countof(mouseState_.rgbButtons));
	//指定番号の番号のボタンが、押されているなら
	if ((mouseState_.rgbButtons[buttonNum] & 0x80) != 0) {
		return true;
	}

	return false;
}

bool Input::TriggerMouse(int32_t buttonNum){
	//マウスデバイスのボタンに、指定番号がないならエラー
	assert(0 <= buttonNum && buttonNum < _countof(mouseStatePre_.rgbButtons));
	if ((mouseStatePre_.rgbButtons[buttonNum] & 0x80) == 0) {
		//マウスデバイスのボタンに、指定番号がないならエラー
		assert(0 <= buttonNum && buttonNum < _countof(mouseState_.rgbButtons));
		if ((mouseState_.rgbButtons[buttonNum] & 0x80) != 0) {
			return true;
		}
	}

	return false;
}

bool Input::ReleaseMouse(int32_t buttonNum){
	//マウスデバイスのボタンに、指定番号がないならエラー
	assert(0 <= buttonNum && buttonNum < _countof(mouseStatePre_.rgbButtons));
	if ((mouseStatePre_.rgbButtons[buttonNum] & 0x80) != 0) {
		//マウスデバイスのボタンに、指定番号がないならエラー
		assert(0 <= buttonNum && buttonNum < _countof(mouseState_.rgbButtons));
		if ((mouseState_.rgbButtons[buttonNum] & 0x80) == 0) {
			return true;
		}
	}

	return false;
}

void Input::GenerateDirectInput() {
	HRESULT result;

	//DirectInput初期化
	result = DirectInput8Create(
		WinApp::GetInstance()->GetHInstance(), DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&dInput_, nullptr);
	assert(SUCCEEDED(result));
}

void Input::GenerateKeyBoardDevice() {
	HRESULT result;

	//キーボードデバイス生成
	result = dInput_->CreateDevice(
		GUID_SysKeyboard, &keyboard_, NULL);
	assert(SUCCEEDED(result));

	//入力データ形式のセット
	result = keyboard_->SetDataFormat(
		&c_dfDIKeyboard);//標準形式
	assert(SUCCEEDED(result));

	//排他制御レベルのセット
	result = keyboard_->SetCooperativeLevel(
		WinApp::GetInstance()->GetHWND(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
}

void Input::GenerateMouseDevice(){
	HRESULT result;

	//マウスデバイス生成
	result = dInput_->CreateDevice(
		GUID_SysMouse, &mouse_, nullptr);
	assert(SUCCEEDED(result));

	//入力データ形式のセット
	result = mouse_->SetDataFormat(
		&c_dfDIMouse2);//拡張8ボタンまで
	assert(SUCCEEDED(result));

	//排他制御レベルのセット
	result = mouse_->SetCooperativeLevel(
		WinApp::GetInstance()->GetHWND(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	assert(SUCCEEDED(result));
}

Input* Input::GetInstance() {
	static Input instance;
	return &instance;
}

Input::~Input(){
	//キーボード入力情報の取得を終了
	if (keyboard_) {
		keyboard_->Unacquire();
	}
	//マウス入力情報の取得を終了
	if (mouse_) {
		mouse_->Unacquire();
	}
}