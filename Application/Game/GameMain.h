#pragma once
#include "SafeDelete.h"
#include "Framework.h"
#include "BaseScene.h"

class GameMain :public Framework{
public://構造体

public://関数
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;

private://変数
	static SceneManager* sceneManager_ ;
};