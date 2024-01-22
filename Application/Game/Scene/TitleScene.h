/*タイトルシーン*/

#pragma once
#include "BaseScene.h"

#include "Input.h"
#include "SpriteBasis.h"
#include "Model.h"
#include "Sprite.h"
#include "Object3d.h"
#include "ObjectManager.h"

#include "Skydome.h"

#include "Text.h"
#include "Button.h"

#include "Camera.h"
#include "LightGroup.h"

#include "SceneManager.h"
#include "Fade.h"
#include "ArrangeTile.h"
#include "TubeManager.h"

#ifdef _DEBUG
#include "ImGuiManager.h"
#endif

/*タイトルシーン*/
class TitleScene : public BaseScene {
public://構造体
	enum modelName {
		playerModel_ = ObjectManager::playerActiveModel_,
		skydomeModel_ = ObjectManager::skydomeModel_,
		tubeModel_ = ObjectManager::tubeModel_,
		cartModel_ = ObjectManager::cartModel_,
		bottomBGModel_ = ObjectManager::bottomBGModel_,
	};
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
	static ObjectManager* objManager_;

	Camera* camera_ = nullptr;
	LightGroup* light_ = nullptr;

#ifdef _DEBUG
	//ImGuiマネージャー
	ImGuiManager* imGuiManager_ = nullptr;
#endif

	// オブジェクト
	std::unique_ptr<Object3d> playerObj_ = nullptr;
	std::unique_ptr<Skydome> skydome_ = nullptr;

	//カートモデル
	std::unique_ptr<Object3d> cart_ = nullptr;

	std::unique_ptr<Object3d> bottomBG_ = nullptr;

	//テキスト
	std::unique_ptr<Text> text_ = nullptr;

	//ボタン
	std::unique_ptr<Button> buttonStart_ = nullptr;
	float alpha_ = 0.0f;

	//タイトルループまで
	int32_t roopTimer_ = 60 * 30;

	//タイトルループフラグ
	bool isRoop_ = false;

	//画面の暗幕
	std::unique_ptr<Fade> blackOut_ = nullptr;

	//タイルならべのシーン遷移
	std::unique_ptr<ArrangeTile> arrangeTile_ = nullptr;

	//背景筒マネージャー
	std::unique_ptr<TubeManager> tubeManager_;
};
