#pragma once

#include "BaseStatePlayScene.h"

class GamePlaySceneRun :
	public BaseStatePlayScene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;
};
