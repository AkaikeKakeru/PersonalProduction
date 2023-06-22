#pragma once

//前方宣言
class SceneManager;

class BaseScene {
public:
	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void Finalize() = 0;

	virtual void SetSceneManager(SceneManager* sceneManager) { sceneManager_ = sceneManager; }

private:
	//シーンマネージャ(借りてくる)
	static SceneManager* sceneManager_;

public:
	virtual ~BaseScene() = default;
};