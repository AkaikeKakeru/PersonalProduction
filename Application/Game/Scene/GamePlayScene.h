#pragma once
#include "BaseScene.h"

#include "Input.h"
#include "SpriteBasis.h"
#include "Model.h"
#include "Sprite.h"
#include "Object3d.h"

#include "Camera.h"
#include "RailCamera.h"
#include "LightGroup.h"
#include "ImGuiManager.h"

#include <memory>
#include <List>

#include "Player.h"
#include "Enemy.h"
#include "EnemyBullet.h"

class CollisionManager;

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
	void Draw2d();

public://定数
	const int kFinalPhaseIndex_ = 3;

public:
	Vector3 CreateRotationVector(Vector3 axisAngle, float angleRadian);

	//敵弾を追加
	void AddEnemyBullet(std::unique_ptr<EnemyBullet> enemyBullet);
	//敵を追加
	void AddEnemy(const Vector3 pos,const Vector3 rota,const Vector3 scale);

	//次の敵の湧き情報を調べる
	void SightNextEnemy();

	//フェーズ番号取得
	size_t GetPhaseIndex() {
		return phaseIndex_;
	}

private: //静的メンバ変数
	//基盤
	static DirectXBasis* dxBas_;
	static Input* input_;
	static SpriteBasis* spriteBas_;
	//ImGuiマネージャー
	static ImGuiManager* imGuiManager_;
	//衝突マネージャー
	static CollisionManager* collisionManager_;

public: //メンバ変数
	Camera* camera_ = nullptr;
	Camera* camera_player = nullptr;

	RailCamera* railCamera_;

	LightGroup* light_ = nullptr;

	/// <summary>
	/// オブジェクト
	/// </summary>
	Object3d* planeObj_ = nullptr;
	Model* planeModel_ = nullptr;
	Model* planeEnemyModel_ = nullptr;

	Object3d* skydomeObj_ = nullptr;
	Model* skydomeModel_ = nullptr;

	Model* bulletModel_ = nullptr;

	/// <summary>
	/// スプライト
	/// </summary>
	Sprite* sprite_ = nullptr;

	//プレイヤー
	Player* player_ = nullptr;

	//エネミー
	std::list<std::unique_ptr<Enemy>> enemys_;

	//Enemy* enemy_ = nullptr;

	//エネミー弾
	std::list<std::unique_ptr<EnemyBullet>> enemyBullets_;

	//フェーズ番号
	size_t phaseIndex_ = 0;
};