#pragma once
#include "BaseStatePlayScene.h"

class GamePlaySceneEnding :
	public BaseStatePlayScene{
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;
};
