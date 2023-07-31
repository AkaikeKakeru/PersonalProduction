#pragma once
#include "BaseScene.h"

#include "Input.h"
#include "SpriteBasis.h"
#include "Model.h"
#include "Sprite.h"
#include "Object3d.h"

#include "Text.h"
#include "Button.h"

#include "Camera.h"
#include "LightGroup.h"

#include "SceneManager.h"
#include "ImGuiManager.h"

class TitleScene : public BaseScene{
public://構造体

public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;
private:
	static DirectXBasis* dxBas_;
	static Input* input_;
	static SpriteBasis* spriteBas_;

	Camera* camera_ = nullptr;
	LightGroup* light_ = nullptr;

	//ImGuiマネージャー
	ImGuiManager* imGuiManager_ = nullptr;

	/// <summary>
	/// オブジェクト
	/// </summary>
	/// <summary>
	Object3d* planeObj_ = nullptr;
	Model* planeModel_ = nullptr;

	Object3d* skydomeObj_ = nullptr;
	Model* skydomeModel_ = nullptr;
	/// スプライト
	/// </summary>
	Sprite* sprite_ = new Sprite();

	//テキスト
	Text* text_ = nullptr;

	//ボタン
	Button* buttonStart_ = nullptr;
};