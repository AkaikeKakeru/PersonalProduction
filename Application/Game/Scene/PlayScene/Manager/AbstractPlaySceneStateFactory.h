/*概念的なプレイシーン状態ファクトリー*/

#pragma once

#include "BaseStatePlayScene.h"
#include <string>

//概念的プレイシーン状態工場
class AbstractPlaySceneStateFactory {
public:
	//仮想デストラクタ
	virtual ~AbstractPlaySceneStateFactory() = default;

	//プレイシーン状態生成
	virtual BaseStatePlayScene* CreateState(const std::string& stateName) = 0;
};
