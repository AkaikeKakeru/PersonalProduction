/*プレイシーン状態ファクトリー*/

#pragma once
#include "AbstractPlaySceneStateFactory.h"

/*プレイシーン状態ファクトリー*/
class PlaySceneStateFactory :
	public AbstractPlaySceneStateFactory {
public:
	//状態生成
	BaseStatePlayScene* CreateState(const std::string& stateName) override;
};
