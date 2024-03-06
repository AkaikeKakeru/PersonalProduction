/*プレイシーン状態管理*/

#include "PlaySceneStateManager.h"
#include "ImGuiManager.h"
#include <cassert>

void PlaySceneStateManager::Update(){
	//次の状態の予約があるなら
	if (nextState_) {
		//旧状態の終了
		if (state_) {
			state_->Finalize();
			delete state_;
		}

		//状態切り替え
		state_ = nextState_;
		nextState_ = nullptr;

		//状態マネージャをセット
		state_->SetStateManager(this);

		//次の状態を初期化
		state_->Initialize();
	}

	//実行中状態の更新
	state_->Update();
}

void PlaySceneStateManager::Draw(){
	state_->Draw();
#ifdef _DEBUG
	ImGuiManager::GetInstance()->Draw();
#endif // DEBUG
}

void PlaySceneStateManager::Finalize(){
	//最後の状態の終了と解放
	state_->Finalize();
	delete state_;
}

void PlaySceneStateManager::ChangeState(const std::string& stateName) {
	assert(stateFactory_);
	assert(nextState_ == nullptr);

	//次状態を生成
	nextState_ = stateFactory_->CreateState(stateName);
}

PlaySceneStateManager* PlaySceneStateManager::GetInstance(){
	static PlaySceneStateManager instance ;

	return &instance;
}
