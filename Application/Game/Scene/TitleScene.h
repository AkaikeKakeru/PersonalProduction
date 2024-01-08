/*タイトルシーン*/

#pragma once
#include "BaseScene.h"

#include "Input.h"
#include "SpriteBasis.h"
#include "Model.h"
#include "Sprite.h"
#include "Object3d.h"

#include "Skydome.h"

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

/*タイトルシーン*/
class TitleScene : public BaseScene {
public://構造体

public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;

	//カメラの処理
	void CameraUpdate();
	//プレイヤーオブジェクトの処理
	void PlayerUpdate();
	//暗幕の処理
	void BlackOutUpdate();

	/// <summary>
	///float値のループ
	/// </summary>
	/// <param name="f">変化対象float</param>
	/// <param name="s">変化の量(速さ)</param>
	/// <param name="min">最小値</param>
	/// <param name="max">最大値</param>
	/// <returns>変化後の値</returns>
	float RoopFloat(float f, float speed, float min, float max);

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
	Object3d* playerObj_ = nullptr;
	Model* playerModel_ = nullptr;

	Skydome* skydome_ = nullptr;
	Model* skydomeModel_ = nullptr;

	//カートモデル
	Object3d* cart_ = nullptr;
	Model* cartModel_ = nullptr;

	/// スプライト
	/// </summary>
	Sprite* sprite_ = nullptr;

	//テキスト
	Text* text_ = nullptr;

	//ボタン
	Button* buttonStart_ = nullptr;
	float alpha_ = 0.0f;

	//タイトルループまで
	int32_t roopTimer_ = 60 * 30;

	//タイトルループフラグ
	bool isRoop_ = false;

	//画面の暗幕
	Fade* blackOut_ = nullptr;

	//タイルならべのシーン遷移
	ArrangeTile* arrangeTile_ = nullptr;
};
