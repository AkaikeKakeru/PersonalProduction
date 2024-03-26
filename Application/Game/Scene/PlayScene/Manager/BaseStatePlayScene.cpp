/*シーンの基礎*/

#include "BaseStatePlayScene.h"
#include "PlaySceneStateManager.h"
#include "GamePlayScene.h"

PlaySceneStateManager* BaseStatePlayScene::stateManager_ = GamePlayScene::GetStateManager();
Camera* BaseStatePlayScene::camera_ = PlaySceneStateManager::GetCamera();
