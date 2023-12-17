#include "TubeManager.h"

void TubeManager::Initialize() {
	float popPos = -scale_.z * 2;

	AddTube(
		{0,0,kTubePopPosZ_},
		{0,0,0},
		scale_
	);
	AddTube(
		{0,0,kTubePopPosZ_ + popPos},
		{0,0,0},
		scale_
	);
	AddTube(
		{0,0,kTubePopPosZ_ + (popPos * 2)},
		{0,0,0},
		scale_
	);
	AddTube(
		{0,0,kTubePopPosZ_ + (popPos * 3)},
		{0,0,0},
		scale_
	);
}

void TubeManager::Update() {

	timerRePop_++;

	if (timerRePop_ >= kRePopTime_) {
		AddTube(
			{0,0,kTubePopPosZ_},
			{0,0,0},
			scale_
		);

		timerRePop_ = 0;
	}

	for (std::unique_ptr<TubeBG>& tube : tubes_) {
		tube->Update();

		if (tube->GetPosition().z <= kTubeDeadPosZ_) {
			tube->SetIsDead(true);
		}
	}

	//筒をデスフラグで削除
	tubes_.remove_if([](std::unique_ptr<TubeBG>& tube) {
		return tube->IsDead();
		});
}

void TubeManager::Draw() {
	for (std::unique_ptr<TubeBG>& tube : tubes_) {
		tube->Draw();
	}
}

void TubeManager::Finalize() {
	for (std::unique_ptr<TubeBG>& tube : tubes_) {
		tube->Finalize();
	}
}

void TubeManager::AddTube(
	const Vector3 pos,
	const Vector3 rota,
	const Vector3 scale) {
	std::unique_ptr<TubeBG> newTube =
		std::make_unique<TubeBG>();
	newTube->Initialize();

	newTube->SetScale(scale);
	newTube->SetRotation(rota);
	newTube->SetPosition(pos);
	newTube->SetSpeed(speed_);

	newTube->SetModel(tubeModel_);

	newTube->SetCamera(camera_);
	newTube->Update();

	//リストに登録
	tubes_.push_back(std::move(newTube));
}
