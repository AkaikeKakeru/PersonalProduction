#pragma once
#include "BaseScene.h"

#include "Input.h"
#include "DrawBasis.h"
#include "Model.h"
#include "Sprite.h"
#include "Object3d.h"

#include "Camera.h"
#include "Light.h"

#include <memory>
#include <List>

class GamePlayScene : public BaseScene {
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

private:
	//基盤
	static DirectXBasis* dxBas_;
	static Input* input_;
	static DrawBasis* drawBas_;

	Camera* camera_ = nullptr;
	Camera* camera_player = nullptr;
	Light* light_ = nullptr;

	/// <summary>
	/// オブジェクト
	/// </summary>
	Object3d* planeObj_ = nullptr;
	Model* planeModel_ = nullptr;

	Object3d* skydomeObj_ = nullptr;
	Model* skydomeModel_ = nullptr;

	/// <summary>
	/// スプライト
	/// </summary>
	Sprite* sprite_ = new Sprite();
};