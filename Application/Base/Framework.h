#pragma once
#include "WinApp.h"
#include "DirectXBasis.h"
#include "Input.h"
#include "Audio.h"
#include "ImGuiManager.h"
#include "SceneManager.h"
#include "AbstractSceneFactory.h"

class Framework {
public:
	//実行
	void Run();
	virtual void Initialize();
	virtual void Update();
	virtual void Draw() = 0;
	virtual void Finalize();

	virtual bool IsEndRequest() { return isEndRequest_; }

	WinApp* GetWinApp()const { return winApp_; }
	DirectXBasis* GetDirectXBasis()const { return dxBas_; }
	Input* GetInput()const { return input_; }

	static SceneManager* GetSceneManager(){ return sceneManager_; }

private:
	//ウィンドウアプリ
	WinApp* winApp_ = nullptr;
	//DirectX基盤
	DirectXBasis* dxBas_ = nullptr;
	//入力
	Input* input_ = nullptr;

	//オーディオ
	Audio* audio_ = nullptr;

	//サウンドデータ
	Audio::SoundData soundData1 = {};

	bool isEndRequest_ = false;

	//シーンマネージャー
	static SceneManager* sceneManager_;

	//ImGuiマネージャー
	ImGuiManager* imGuiManager_ = nullptr;
public:
	//シーンファクトリ―
	AbstractSceneFactory* sceneFactory_ = nullptr;
public: 
	virtual ~Framework() = default;
};