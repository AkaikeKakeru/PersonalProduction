#pragma once

#include "BaseStatePlayScene.h"
#include "Input.h"

#include "ObjectManager.h"

#include "Player.h"
#include "PlayerBullet.h"
#include "Enemy.h"
#include "EnemyBullet.h"

#include "Particle.h"
#include "ParticleManager.h"

#include "Camera.h"
#include "DebugCamera.h"
#include <FollowCamera.h>

#include <memory>
#include <List>

class GamePlaySceneRun :
	public BaseStatePlayScene {

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

	enum mouseSpriteIndex {
		mouseSpriteL_,
		mouseSpriteR_,
		mouseSpriteWheel_,
		mouseSpriteTextS_,
		mouseSpriteTextH_,
	};

public://定数
	const int kFinalPhaseIndex_ = 3;
	const int kBossPhaseIndex_ = kFinalPhaseIndex_ + 1;

public://関数
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

	//敵を追加
	void AddEnemy(const Vector3 pos,
		const Vector3 rota,
		const Vector3 scale,
		const int bulletType);
#pragma region popLoader
	//敵発生データの読込
	void LoadEnemyPopData(std::string filename);

	//敵発生コマンドの実行
	void UpdateEnemyPopCommands();

	//発生コマンドの読込
	Vector3 LoadCommandsVector3(
		std::istringstream* line_stream,
		std::string word);
#pragma endregion	

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

	//エネミー
	std::list<std::unique_ptr<Enemy>> enemys_;

#pragma region popLoader
	//敵発生コマンド
	std::stringstream enemyPopCommands_;

	//発生待機フラグ
	bool isWait_ = false;
#pragma endregion

	//今プレイヤーの与えるダメージ量
	float nowDamagePlayer_ = 3.0f;

	//今エネミーの与えるダメージ量
	float nowDamageEnemy_ = 3.0f;

	//今のエネミーの弾種
	int nowBulletTypeEnemy_ = EnemyBullet::Gun_BulletType;

/*スプライト*/
	//マウススプライトリスト
	std::vector<std::unique_ptr<Sprite>> mouseSprites_;

	/*カーソル用の変数*/
	//カーソル
	Cursor cursor_;
	//エネミーのワールド座標
	Vector3 enemyWorldPos_{ 0.0f,0.0f,30.0f };
	//ロックオン時の標的座標
	Vector3 LockOnTargetPos_{ 0.0f,0.0f,30.0f };

	/*パーティクル*/

	//パーティクル
	std::unique_ptr<Particle> particle_ = nullptr;
	//パーティクルマネージャー
	std::unique_ptr<ParticleManager> pm_ = nullptr;

	/*フラグ*/
	//デバッグカメラのオンオフ
	bool isDebugCamera_ = false;

	//噴きあがりフラグ
	bool isGushing_ = false;

	//フェーズ番号
	int phaseIndex_ = 0;

public://アクセッサ
	//フェーズ番号取得
	size_t GetPhaseIndex() {
		return phaseIndex_;
	}

	//フェーズ番号セット
	void SetPhaseIndex(const size_t& index) {
		phaseIndex_ = (int)index;
	}
};
