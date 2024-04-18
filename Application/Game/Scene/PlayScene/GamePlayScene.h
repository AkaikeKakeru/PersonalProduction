/*ゲームプレイシーン*/

#pragma once
#include "BaseScene.h"

#include "Input.h"
#include "SpriteBasis.h"
#include "Model.h"
#include "Sprite.h"
#include "Object3d.h"
#include "ObjectManager.h"

#include "Particle.h"
#include "ParticleManager.h"
#include "TubeManager.h"

#include "Camera.h"
#include "DebugCamera.h"

#include "LightGroup.h"
#include "Cursor.h"
#include "PauseScreen.h"

#ifdef _DEBUG
#include "ImGuiManager.h"
#endif

#include <memory>
#include <List>

#include "Player.h"
#include "PlayerBullet.h"
#include "Enemy.h"
#include "EnemyBullet.h"
#include "Boss.h"
#include "Skydome.h"
#include "Cart.h"

#include "Fade.h"
#include "ArrangeTile.h"

#include "PlaySceneStateManager.h"
#include "AbstractPlaySceneStateFactory.h"
#include <FollowCamera.h>

#pragma region popLoader
#include <sstream>
#pragma endregion

class BossBattleScene;
class CollisionManager;

/*ゲームプレイシーン*/
class GamePlayScene :
	public BaseScene {
public:
	enum modelName {
		playerActiveModel_ = ObjectManager::playerActiveModel_,
		playerHideModel_ = ObjectManager::playerHideModel_,
		enemyModel_ = ObjectManager::enemyModel_,
		skydomeModel_ = ObjectManager::skydomeModel_,
		bulletModel_ = ObjectManager::bulletModel_,
		tubeModel_ = ObjectManager::tubeModel_,
		cartModel_ = ObjectManager::cartModel_,
		bottomBGModel_ = ObjectManager::bottomBGModel_,
		doorModel_ = ObjectManager::doorModel_,
	};

	enum objName {
		doorL_,
		doorR_,
		bottomBG_,
	};

	enum mouseSpriteIndex {
		mouseSpriteL_,
		mouseSpriteR_,
		mouseSpriteWheel_,
		mouseSpriteTextS_,
		mouseSpriteTextH_,
	};

public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;

private:
	void Initialize3d();
	void Initialize2d();

	void InitializeCamera();
	void InitializeBackground();
	void InitializeCharacter();

	void InitializeMouseUI();
	void InitializeBlackOut();

	void Update3d();
	void Update2d();

	void UpdateCamera();
	void UpdateBackground();
	void UpdateCharacter();
	void UpdateDamage();

	void UpdateMouseUI();
	void UpdateBlackOut();

	void Draw3d();
	void Draw2d();

	void DrawCharacter();
	void DrawBackground();
	void DrawParticle();
	void DrawMouseUI();
	void DrawCharacterUI();
	void DrawBlackOut();

	void FinalizeBackGround();
	void FinalizeCharacter();
	void FinalizeBlackOut();

public://定数
	const int kFinalPhaseIndex_ = 3;
	const int kBossPhaseIndex_ = kFinalPhaseIndex_ + 1;

public:

public:
	//状態マネージャーの取得
	static PlaySceneStateManager* GetStateManager() {
		return stateManager_;
	}

	//カーソルの取得
	Cursor* GetCursor() {
		return &cursor_;
	}

	//噴きあがりフラグのセット
	void SetIsGushing(bool isGushing) {
		isGushing_ = isGushing;
	}

private: //静的メンバ変数
	//基盤
	static DirectXBasis* dxBas_;
	static Input* input_;
	static SpriteBasis* spriteBas_;

	static ObjectManager* objManager_;

#ifdef _DEBUG
	//ImGuiマネージャー
	static ImGuiManager* imGuiManager_;
#endif _

	//衝突マネージャー
	static CollisionManager* collisionManager_;

	//状態マネージャー
	static PlaySceneStateManager* stateManager_;

	//カメラ
	static Camera* camera_;

public: //メンバ変数

	//カメラ(レール)
	std::unique_ptr<RailCamera> railCamera_;

	//カメラ(追従)
	std::unique_ptr<FollowCamera> followCamera_;

	//カメラ(デバッグ)
	std::unique_ptr<DebugCamera> debugCamera_;

	//ライト
	LightGroup* light_ = nullptr;

	//天球
	std::unique_ptr<Skydome> skydome_ = nullptr;

	//扉の位置
	Vector3 doorPos_{};

	//オブジェリスト
	std::list<std::unique_ptr<Object3d>> objs_;

	//パーティクル
	std::unique_ptr<Particle> particle_ = nullptr;
	//パーティクルマネージャー
	std::unique_ptr<ParticleManager> pm_ = nullptr;

	//デバッグカメラのオンオフ
	bool isDebugCamera_ = false;

	//画面の暗幕
	std::unique_ptr<Fade> blackOut_ = nullptr;

	//タイルならべのシーン遷移
	std::unique_ptr<ArrangeTile> arrangeTile_ = nullptr;

	//タイマー最大値
	int timerMax_ = 60;
	//タイマー現在値
	int timerNow_ = 0;

	/*カーソル用の変数*/
	//カーソル
	Cursor cursor_;
	//エネミーのワールド座標
	Vector3 enemyWorldPos_{ 0.0f,0.0f,30.0f };
	//ロックオン時の標的座標
	Vector3 LockOnTargetPos_{ 0.0f,0.0f,30.0f };

	//噴きあがりフラグ
	bool isGushing_ = false;

	//背景筒マネージャー
	std::unique_ptr<TubeManager> tubeManager_;

	//マウススプライトリスト
	std::vector<std::unique_ptr<Sprite>> mouseSprites_;

	//ポーズスクリーン
	std::unique_ptr<PauseScreen> pauseScreen_;
	
	//ポーズフラグ
	bool isPause_ = false;

public:
	//状態ファクトリ―
	AbstractPlaySceneStateFactory* stateFactory_ = nullptr;

	static Camera* GetCamera() {
		return camera_;
	};
};
