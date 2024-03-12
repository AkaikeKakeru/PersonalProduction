/*ゲームメイン*/

#pragma once
#include "SafeDelete.h"
#include "Framework.h"
#include "BaseScene.h"

#include "PlaySceneStateManager.h"
#include "AbstractPlaySceneStateFactory.h"

/*ゲームメイン*/
class GameMain :public Framework{
public://構造体

public://関数
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;

public:
	static PlaySceneStateManager* GetStateManager() {
		return stateManager_;
	}

private://変数
	static SceneManager* sceneManager_ ;

	static PlaySceneStateManager* stateManager_;

public:
	//状態ファクトリ―
	AbstractPlaySceneStateFactory* stateFactory_ = nullptr;
};
