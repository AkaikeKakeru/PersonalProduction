/*ゲームクリアシーン*/

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
#include "Ease.h"

#include "TubeManager.h"

#ifdef _DEBUG
#include "ImGuiManager.h"
#endif

/*ゲームクリアシーン*/
class GameClearScene : public BaseScene{
public://構造体
	enum modelName {
		playerActiveModel_ = ObjectManager::playerActiveModel_,
		skydomeModel_ = ObjectManager::skydomeModel_,
		tubeModel_ = ObjectManager::tubeModel_,
		cartModel_ = ObjectManager::cartModel_,
		bottomBGModel_ = ObjectManager::bottomClearBGModel_,
		doorModel_ = ObjectManager::doorModel_,
	};

private://定数
	int const cPlayerSpeed_ = 4;

public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;

	void Introduction();

	float RoopFloat(float f, float speed, float min, float max);

	void CameraEase();

	void UIEase();

	void BlackOutUpdate();

	void PlayerUpdate();

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
	std::unique_ptr<Object3d> playerObj_ = nullptr;

	std::unique_ptr<Object3d> skydomeObj_ = nullptr;

	//カートモデル
	std::unique_ptr<Object3d> cart_ = nullptr;

	//扉の位置
	Vector3 doorPos_{};

	//左扉
	std::unique_ptr<Object3d> doorL_ = nullptr;
	//右扉
	std::unique_ptr<Object3d> doorR_ = nullptr;

	std::unique_ptr<Object3d> bottomBG_ = nullptr;

	//オブジェリスト
	std::list<std::unique_ptr<Object3d>> objs_;

	// スプライト
	Sprite* sprite_ = nullptr;

	//テキスト
	std::unique_ptr<Text> text_ = nullptr;

	//ボタン
	std::unique_ptr<Button> buttonTitle_ = nullptr;
	std::unique_ptr<Button> buttonRetry_ = nullptr;

	//導入中かのフラグ
	bool isIntro_ = true;

	//イージング
	Ease easeCameraPosition_;

	Ease easeTextPosition_;

	Ease easeButtonPosition_;

#pragma region SceneChange

	//タイトル行きまで
	int32_t goTitleTimer_ = 60 * 8;

	//自動タイトル行きフラグ
	bool isGoTitle_ = false;

	//画面の暗幕
	std::unique_ptr<Fade> blackOut_ = nullptr;

	//タイルならべのシーン遷移
	std::unique_ptr<ArrangeTile> arrangeTile_ = nullptr;

#pragma endregion

	//背景筒マネージャー
	TubeManager* tubeManager_;
};
