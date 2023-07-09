#pragma once
#include "BaseScene.h"

#include "Input.h"
#include "DrawBasis.h"
#include "Model.h"
#include "Sprite.h"
#include "Object3d.h"

#include "Camera.h"
#include "RailCamera.h"

#include "LightGroup.h"

#include "ImGuiManager.h"

#include <memory>
#include <List>

class Player;
class Enemy;
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
public:
	Vector3 CreateRotationVector(Vector3 axisAngle, float angleRadian);

private: //静的メンバ変数
	//基盤
	static DirectXBasis* dxBas_;
	static Input* input_;
	static DrawBasis* drawBas_;
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
	Enemy* enemy_ = nullptr;
};