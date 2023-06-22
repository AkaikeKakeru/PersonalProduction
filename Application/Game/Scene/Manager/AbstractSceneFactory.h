#pragma once

#include "BaseScene.h"
#include <string>

//概念的シーン工場
class AbstractSceneFactory {
public:
	//仮想デストラクタ
	virtual ~AbstractSceneFactory() = default;

	//シーン生成
	virtual BaseScene* CreateScene(const std::string& sceneName) = 0;
};