#include "PlaySceneStateFactory.h"

#include "GamePlaySceneOpening.h"
#include "GamePlaySceneRun.h"
#include "GamePlaySceneBossBattle.h"
#include "GamePlaySceneEnding.h"

BaseStatePlayScene* PlaySceneStateFactory::CreateState(
	const std::string& stateName) {
	//次の状態を生成
	BaseStatePlayScene* newState = nullptr;
	//if (stateName == "OPENING") {
	//	newState = new GamePlaySceneOpening();
	//}
	//else
		if (stateName == "RUN") {
		newState = new GamePlaySceneRun();
	}
	//else if (stateName == "BOSSBATTLE") {
	//	newState = new GamePlaySceneBossBattle();
	//}
	//else if (stateName == "ENDING") {
	//	newState = new GamePlaySceneEnding();
	//}
	return newState;
}
