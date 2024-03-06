/*プレイシーン状態の基礎*/

#pragma once

//前方宣言
class PlaySceneStateManager;

//プレイシーン状態の基礎
class BaseStatePlayScene {
public:
	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void Finalize() = 0;

	virtual void SetStateManager(PlaySceneStateManager* stateManager) {
		stateManager_ = stateManager; }

private:
	//状態マネージャ(借りてくる)
	static PlaySceneStateManager* stateManager_;

public:
	virtual ~BaseStatePlayScene() = default;
};
