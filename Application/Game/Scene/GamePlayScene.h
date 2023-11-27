/*ゲームプレイシーン*/

#pragma once
#include "BaseScene.h"

#include "Input.h"
#include "SpriteBasis.h"
#include "Model.h"
#include "Sprite.h"
#include "Object3d.h"
#include "Particle.h"
#include "ParticleManager.h"

#include "Camera.h"
#include "RailCamera.h"
#include "DebugCamera.h"

#include "LightGroup.h"

#include "Cursor.h"

#ifdef _DEBUG
#include "ImGuiManager.h"
#endif

#include <memory>
#include <List>

#include "Player.h"
#include "PlayerBullet.h"
#include "Enemy.h"
#include "EnemyBullet.h"
#include "Skydome.h"

#include "Fade.h"
#include "ArrangeTile.h"

#pragma region popLoader
#include <sstream>
#pragma endregion

class CollisionManager;

/*ゲームプレイシーン*/
class GamePlayScene :
	public BaseScene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;

private:
	void Initialize3d();
	void Initialize2d();

	void Update3d();
	void Update2d();

	void Draw3d();
	void DrawParticle();
	void Draw2d();

public://定数
	const int kFinalPhaseIndex_ = 3;

public:
	//自機弾を追加
	void AddPlayerBullet(std::unique_ptr<PlayerBullet> playerBullet);

	//敵弾を追加
	void AddEnemyBullet(std::unique_ptr<EnemyBullet> enemyBullet);
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

	//黒幕の更新
	void BlackOutUpdate();

public:
	//フェーズ番号取得
	size_t GetPhaseIndex() {
		return phaseIndex_;
	}

	//今プレイヤーの与えるダメージ量の取得
	float GetNowDamagePlayer() {
		return nowDamagePlayer_;
	}

	//今プレイヤーの与えるダメージ量のセット
	void SetNowDamagePlayer(float damage) {
		nowDamagePlayer_ = damage;
	}

	//今エネミーの与えるダメージ量の取得
	float GetNowDamageEnemy() {
		return nowDamageEnemy_;
	}

	//今エネミーの与えるダメージ量のセット
	void SetNowDamageEnemy(float damage) {
		nowDamageEnemy_ = damage;
	}

	//今エネミーの弾種の取得
	int GetNowBulletTypeEnemy() {
		return nowBulletTypeEnemy_;
	}

	//今エネミーの弾種のセット
	void SetNowBulletTypeEnemy(int bulletType) {
		nowBulletTypeEnemy_ = bulletType;
	}

	//カーソルの取得
	Cursor* GetCursor() {
		return &cursor_;
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

	//衝突マネージャー
	static CollisionManager* collisionManager_;

public: //メンバ変数
	Camera* camera_ = nullptr;
	Camera* camera_player = nullptr;

	RailCamera* railCamera_;

	DebugCamera* debugCamera_;

	LightGroup* light_ = nullptr;

	/// <summary>
	/// オブジェクト
	/// </summary>
	Object3d* planeObj_ = nullptr;
	Model* planeModel_ = nullptr;
	Model* planeEnemyModel_ = nullptr;

	Skydome* skydome_ = nullptr;

	Model* skydomeModel_ = nullptr;

	Model* bulletModel_ = nullptr;

	//扉の位置
	Vector3 doorPos_{};

	Model* doorModel_ = nullptr;
	//左扉
	Object3d* doorL_ = nullptr;
	//右扉
	Object3d* doorR_ = nullptr;

	/// <summary>
	/// パーティクル
	/// </summary>
	Particle* particle_ = nullptr;
	ParticleManager* pm_ = nullptr;

	/// <summary>
	/// スプライト
	/// </summary>
	Sprite* sprite_ = nullptr;

	//プレイヤー
	Player* player_ = nullptr;

	//弾
	std::list<std::unique_ptr<PlayerBullet>> playerBullets_;

	//エネミー
	std::list<std::unique_ptr<Enemy>> enemys_;

	//エネミー弾
	std::list<std::unique_ptr<EnemyBullet>> enemyBullets_;

	//今プレイヤーの与えるダメージ量
	float nowDamagePlayer_ = 0.0f;

	//今エネミーの与えるダメージ量
	float nowDamageEnemy_ = 0.0f;

	//今のエネミーの弾種
	int nowBulletTypeEnemy_ = EnemyBullet::Gun_BulletType;

	//フェーズ番号
	size_t phaseIndex_ = 0;

	//デバッグカメラのオンオフ
	bool isDebugCamera_ = false;

#pragma region popLoader
	//敵発生コマンド
	std::stringstream enemyPopCommands_;

	//発生待機フラグ
	bool isWait_ = false;
#pragma endregion

	//画面の暗幕
	Fade* blackOut_ = nullptr;

	//タイルならべのシーン遷移
	ArrangeTile* arrangeTile_ = nullptr;

	//タイマー最大値
	int timerMax_ = 60;
	//タイマー現在値
	int timerNow_ = 0;

	/*カーソル用の変数*/
	//カーソル
	Cursor cursor_;
	//エネミーのワールド座標
	Vector3 enemyWorldPos_{0.0f,0.0f,30.0f};
	//ロックオン時の標的座標
	Vector3 LockOnTargetPos_{0.0f,0.0f,30.0f};
};
