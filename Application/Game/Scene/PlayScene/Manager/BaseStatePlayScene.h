/*プレイシーン状態の基礎*/

#pragma once

#include <memory>
#include <List>

//前方宣言
class PlaySceneStateManager;
class Camera;

//プレイシーン状態の基礎
class BaseStatePlayScene {
public:
	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void Draw3d() = 0;
	virtual void Draw2d() = 0;
	virtual void DrawParticle() = 0;
	virtual void DrawImGui() = 0;

	virtual void Finalize() = 0;

	virtual void Update3d() = 0;
	virtual void Update2d() = 0;

	virtual void SetStateManager(PlaySceneStateManager* stateManager) {
		stateManager_ = stateManager; }

	virtual void SetCamera(Camera* camera) {
		camera_ = camera;
	}

private:
	//状態マネージャ(借りてくる)
	static PlaySceneStateManager* stateManager_;

	//カメラ
	static Camera* camera_;



public:
	virtual ~BaseStatePlayScene() = default;
};
