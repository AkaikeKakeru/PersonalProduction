#pragma once
#define DIRECTINPUT_VERSION 0x0800 //DirectInput Version
#include <dinput.h>
#include <wrl.h>

#include "Vector2.h"
#include <cstdint>

class Input {
private://省略
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

private://構造体
	typedef enum MouseButtonNum {
		LeftButton,
		RightButton,
		CenterButton,
	}ButtonNum;

public://基本の関数
	void Initialize();
	void Update();

public: //public固有関数
	/// <summary>
	/// キーを押しているか
	/// </summary>
	/// <param name="keyNum">キー番号</param>
	/// <returns>押しているか否か</returns>
	bool PressKey(BYTE keyNum);

	/// <summary>
	/// キーを今押した瞬間か
	/// </summary>
	/// <param name="keyNum">キー番号</param>
	/// <returns>今押した瞬間か否か</returns>
	bool TriggerKey(BYTE keyNum);

	/// <summary>
	/// キーを離した瞬間か
	/// </summary>
	/// <param name="keyNum">キー番号</param>
	/// <returns>離した瞬間か否か</returns>
	bool ReleaseKey(BYTE keyNum);

	/// <summary>
	/// マウスを押しているか
	/// </summary>
	/// <param name="buttonNum">マウス番号</param>
	/// <param name="buttonNum">0:左,1:右,2:中央</param>
	/// <returns>押しているか否か</returns>
	bool PressMouse(int32_t buttonNum);

	/// <summary>
	/// マウスを今押した瞬間か
	/// </summary>
	/// <param name="buttonNum">マウス番号</param>
	/// <param name="buttonNum">0:左,1:右,2:中央</param>
	/// <returns>今押した瞬間か否か</returns>
	bool TriggerMouse(int32_t buttonNum);

	/// <summary>
	/// マウスを今離した瞬間か
	/// </summary>
	/// <param name="buttonNum">マウス番号</param>
	/// <param name="buttonNum">0:左,1:右,2:中央</param>
	/// <returns>今離した瞬間か否か</returns>
	bool ReleaseMouse(int32_t buttonNum);

	const Vector2& GetMousePosition() const { return mousePos_; }

public://固有関数
	void GenerateDirectInput();
	void GenerateKeyBoardDevice();
	void GenerateMouseDevice();

private://メンバ変数
	//DirectInput
	ComPtr<IDirectInput8> dInput_;

	//キーボードデバイス
	ComPtr<IDirectInputDevice8> keyboard_;
	//マウスデバイス
	ComPtr<IDirectInputDevice8> mouse_;

	//今押されているかのキー情報保存配列
	BYTE key_[256] = {};
	//1F押されてたかの確認用キー情報保存配列
	BYTE keyPre_[256] = {};

	//マウスステート
	DIMOUSESTATE2 mouseState_;
	//1F前のマウスステート
	DIMOUSESTATE2 mouseStatePre_;

	//マウス座標
	Vector2 mousePos_;

private:
	Input() = default;
	~Input();
	//コピーコンストラクタを無効
	Input(const Input&) = delete;
	//代入演算子を無効
	const Input& operator=(const Input&) = delete;

public://static
	static Input* GetInstance();
};