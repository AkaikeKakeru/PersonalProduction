/*ポーズ画面*/

#pragma once

#include "Input.h"
#include "SpriteBasis.h"
#include "Model.h"
#include "Sprite.h"
#include "Object3d.h"

#include "GameMain.h"

#ifdef _DEBUG
#include "ImGuiManager.h"
#endif

#pragma region popLoader
#include <sstream>
#include <Ease.h>
#pragma endregion

class BossBattleScene;
class CollisionManager;

/*ポーズ画面*/
class PauseScreen {

public:
	//イージング最大時間
	static const int cMaxTimeEase_ = 10;

public:
	//enum mouseSpriteIndex {
	//	mouseSpriteL_,
	//	mouseSpriteR_,
	//	mouseSpriteWheel_,
	//	mouseSpriteTextS_,
	//	mouseSpriteTextH_,
	//};

public:
	void Initialize();
	void Update();
	void Draw();
	void Finalize();

	void ResetEase_Run();
	void ResetEase_Remove();

public://定数

public:

public:
	const bool IsRun() {
		return isRun_;
	} 

	const bool IsEaseRun() {
		return sizeEase_.Is();
	}

	void SetIsRemove(bool is) {
		isRemove_ = is;
	}

private: //静的メンバ変数
	//基盤
	static DirectXBasis* dxBas_;
	static Input* input_;
	static SpriteBasis* spriteBas_;

#ifdef _DEBUG
	//ImGuiマネージャー
	static ImGuiManager* imGuiManager_;
#endif _
	
public: //メンバ変数
	//ポーズウィンドウスプライト
	std::unique_ptr<Sprite> windowSprite_;

	//マウススプライトリスト
	std::unique_ptr<Sprite> pauseLogoSprite_;


	////マウススプライトリスト
	//std::vector<std::unique_ptr<Sprite>> mouseSprites_;

	//size用イーズ
	Ease sizeEase_;

	//実行フラグ
	bool isRun_;
	//取り除きフラグ
	bool isRemove_;

public:
};
