/*シーンの基礎*/

#include "BaseStatePlayScene.h"
#include "GamePlayScene.h"

PlaySceneStateManager* BaseStatePlayScene::stateManager_ = GamePlayScene::GetStateManager();
