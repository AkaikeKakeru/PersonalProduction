/*ゲームクリアシーン*/

#pragma once
#include "BaseScene.h"

#include "Input.h"
#include "SpriteBasis.h"
#include "Sprite.h"
#include "Object3d.h"
#include "Model.h"

#include "Text.h"
#include "Button.h"

#include "Camera.h"
#include "LightGroup.h"

#include "SceneManager.h"

#include "Fade.h"
#include "ArrangeTile.h"
#include "Ease.h"

#ifdef _DEBUG
#include "ImGuiManager.h"
#endif

/*ゲームクリアシーン*/
class GameClearScene : public BaseScene{
public://構造体

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
private:
	static DirectXBasis* dxBas_;
	static Input* input_;
	static SpriteBasis* spriteBas_;

	Camera* camera_ = nullptr;
	LightGroup* light_ = nullptr;

#ifdef _DEBUG
	//ImGuiマネージャー
	ImGuiManager* imGuiManager_ = nullptr;
#endif

	// オブジェクト
	Object3d* planeObj_ = nullptr;
	Model* planeModel_ = nullptr;

	Object3d* skydomeObj_ = nullptr;
	Model* skydomeModel_ = nullptr;

	//扉の位置
	Vector3 doorPos_{};

	Model* doorModel_ = nullptr;
	//左扉
	Object3d* doorL_ = nullptr;
	//右扉
	Object3d* doorR_ = nullptr;

	// スプライト
	Sprite* sprite_ = nullptr;

	//テキスト
	Text* text_ = nullptr;

	//ボタン
	Button* buttonTitle_ = nullptr;
	Button* buttonRetry_ = nullptr;

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
	Fade* blackOut_ = nullptr;

	//タイルならべのシーン遷移
	ArrangeTile* arrangeTile_ = nullptr;

#pragma endregion
};
