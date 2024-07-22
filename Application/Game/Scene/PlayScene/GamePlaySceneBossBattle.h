/*ボスバトルシーン*/
#pragma once

#include "BaseStatePlayScene.h"
#include "Input.h"

#include "ObjectManager.h"

#include "Player.h"
#include "PlayerBullet.h"
#include "Boss.h"

#include "Particle.h"
#include "ParticleManager.h"

#include "Camera.h"
#include "DebugCamera.h"
#include <FollowCamera.h>

#include <memory>
#include <List>

class GamePlaySceneBossBattle :
	public BaseStatePlayScene{

public://構造体など
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

public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;

	void Update3d() override;
	void Update2d() override;

	void Draw3d() override;
	void Draw2d() override;
	void DrawParticle() override;
	void DrawImGui() override;

private:
	void Initialize3d();
	void InitializeParticle();

	void InitializeCamera();
	void InitializeCharacter();

	void UpdateCamera();
	void UpdateCharacter();
	void UpdateDamage();

	void DrawCharacter();
	void DrawCharacterUI();

	void FinalizeCharacter();

	void RemoveUniquePtr();
	void DebugShortCut();
	void PhaseChange();

private://静的変数
	static Input* input_;
	static ObjectManager* objManager_;
	static Camera* camera_;
private://変数

/*カメラ*/
	//カメラ(追従)
	std::unique_ptr<FollowCamera> followCamera_;

	//カメラ(デバッグ)
	std::unique_ptr<DebugCamera> debugCamera_;

	/*キャラクター*/
	//プレイヤー
	std::unique_ptr<Player> player_ = nullptr;

	//ボス
	std::unique_ptr<Boss> boss_ = nullptr;

	//今プレイヤーの与えるダメージ量
	float nowDamagePlayer_ = 0.0f;

	//今エネミーの与えるダメージ量
	float nowDamageEnemy_ = 0.0f;

	/*カーソル用の変数*/
	//カーソル
	Cursor cursor_;
	//エネミーのワールド座標
	Vector3 enemyWorldPos_{ 0.0f,0.0f,30.0f };
	//ロックオン時の標的座標
	Vector3 LockOnTargetPos_{ 0.0f,0.0f,30.0f };

	/*フラグ*/
	//デバッグカメラのオンオフ
	bool isDebugCamera_ = false;
};
