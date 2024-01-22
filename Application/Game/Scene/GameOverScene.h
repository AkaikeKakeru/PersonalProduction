/*ゲームオーバーシーン*/

#pragma once
#include "BaseScene.h"

#include "Input.h"
#include "SpriteBasis.h"
#include "Sprite.h"
#include "Object3d.h"
#include "ObjectManager.h"
#include "Model.h"

#include "Text.h"
#include "Button.h"

#include "Camera.h"
#include "LightGroup.h"

#include "SceneManager.h"
#include "Fade.h"
#include "ArrangeTile.h"

#ifdef _DEBUG
#include "ImGuiManager.h"
#endif

/*ゲームオーバーシーン*/
class GameOverScene : public BaseScene {
public://構造体
	enum modelName {
		enemyModel_ = ObjectManager::enemyModel_,
		skydomeModel_ = ObjectManager::skydomeModel_,
	};

public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;

	//暗幕の処理
	void BlackOutUpdate();

private:
	static DirectXBasis* dxBas_;
	static Input* input_;
	static SpriteBasis* spriteBas_;
	static ObjectManager* objManager_;

	Camera* camera_ = nullptr;
	LightGroup* light_ = nullptr;

#ifdef _DEBUG
	//ImGuiマネージャー
	ImGuiManager* imGuiManager_ = nullptr;
#endif

	// オブジェクト
	std::unique_ptr<Object3d> planeObj_ = nullptr;
	std::unique_ptr<Object3d> skydomeObj_ = nullptr;

	//テキスト
	std::unique_ptr<Text> text_ = nullptr;

	//ボタン
	std::unique_ptr<Button> buttonTitle_ = nullptr;
	std::unique_ptr<Button> buttonRetry_ = nullptr;

	//画面の暗幕
	std::unique_ptr<Fade> blackOut_ = nullptr;

	//タイルならべのシーン遷移
	std::unique_ptr<ArrangeTile> arrangeTile_ = nullptr;
};
