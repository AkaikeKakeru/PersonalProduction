#pragma once
#include "BaseStatePlayScene.h"

class GamePlaySceneOpening :
	public BaseStatePlayScene{
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;
};
