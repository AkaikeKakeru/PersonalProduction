/*プレイシーン状態管理*/

#pragma once
#include"BaseStatePlayScene.h"
#include "AbstractPlaySceneStateFactory.h"

#include <memory>
#include <List>

class GamePlayScene;
class Player;

/*プレイシーン状態管理*/
class PlaySceneStateManager final {
public:
	void Update();
	void Draw();
	void Draw3d();
	void Draw2d();
	void DrawParticle();
	void Finalize();

	void ChangeState(const std::string& stateName);

	//状態ファクトリーのセット
	void SetSceneFactory(AbstractPlaySceneStateFactory* stateFactory) {
		stateFactory_ = stateFactory; }

private:
	//現在状態
	BaseStatePlayScene* state_ = nullptr;
	//次の状態
	BaseStatePlayScene* nextState_ = nullptr;

	//状態ファクトリ―(借りてくる)
	AbstractPlaySceneStateFactory* stateFactory_ = nullptr;

	//ゲームプレイシーン
	GamePlayScene* gameScene_ = nullptr;

private:
	PlaySceneStateManager() = default;
	~PlaySceneStateManager() = default;
	//コピーコンストラクタを無効
	PlaySceneStateManager(const PlaySceneStateManager&) = delete;
	//代入演算子を無効
	const PlaySceneStateManager& operator=(const PlaySceneStateManager&) = delete;

public://static
	static PlaySceneStateManager* GetInstance();
};
