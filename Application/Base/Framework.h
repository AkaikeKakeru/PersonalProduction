/*コードの全体の処理を、
初期化　更新　描画のグループとして三つに独立させる*/

/*プロジェクト内の、ウィンドウ作成や基盤の初期化等の枠組みを行う*/

#pragma once
#include "WinApp.h"
#include "DirectXBasis.h"
#include "Input.h"
#include "Audio.h"
#include "ImGuiManager.h"
#include "SceneManager.h"
#include "AbstractSceneFactory.h"
#include "PostEffect.h"

//枠組み
class Framework {
public:
	//実行
	void Run();
	virtual void Initialize();
	virtual void Update();
	virtual void Draw() = 0;
	virtual void Finalize();

	virtual bool IsEndRequest() { return isEndRequest_; }

public:
	//ウィンドウズアプリケーションの取得
	WinApp* GetWinApp() const {
		return winApp_;
	}
	//DirectX基盤の取得
	DirectXBasis* GetDirectXBasis() const {
		return dxBas_;
	}
	//入力の取得
	Input* GetInput() const {
		return input_;
	}
	//ポストエフェクトの取得
	PostEffect* GetPostEffect() const {
		return postEffect_;
	}
	//シーンマネージャーの取得
	static SceneManager* GetSceneManager() {
		return sceneManager_;
	}

public: //定数
	static const int kTextTextureIndex_ = 100;
	static const int kWhiteTextureIndex_ = 200;
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

	//シーンマネージャー
	static SceneManager* sceneManager_;

	//ImGuiマネージャー
	ImGuiManager* imGuiManager_ = nullptr;

	//ポストエフェクト
	PostEffect* postEffect_ = nullptr;

	bool isEndRequest_ = false;

public:
	//シーンファクトリ―
	AbstractSceneFactory* sceneFactory_ = nullptr;
public:
	virtual ~Framework() = default;
};