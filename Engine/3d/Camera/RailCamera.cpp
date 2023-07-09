#include "RailCamera.h"
#include <imgui.h>

Input* RailCamera::input_ = Input::GetInstance();

void RailCamera::Initialize(const Vector3 pos,
	const Vector3 rotaRadianAngle) {
	worldTransform_.position_ = pos;
	worldTransform_.rotation_ = rotaRadianAngle;

	viewProjection_.Initialize();

	splinePosStart_ = { 0.0f,0.0f,0.0f };
	splinePos1_ = { 30.0f,0.0f,50.0f };
	splinePos2_ = { -30.0f,0.0f,100.0f };
	splinePosEnd_ = { 0.0f,0.0f,0.0f };

	splineDirStart_ = { 0.0f,0.0f,0.0f };
	splineDir1_ = { 0.0f, -ConvertToRadian(90), 0.0f };
	splineDir2_ = { 0.0f,ConvertToRadian(90),0.0f };
	splineDirEnd_ = { 0.0f,0.0f,0.0f };

#ifdef _DEBUG
	{
		debugPos_[0] = { worldTransform_.position_.x };
		debugPos_[1] = { worldTransform_.position_.y };
		debugPos_[2] = { worldTransform_.position_.z };

		debugDir_[0] = { worldTransform_.rotation_.x };
		debugDir_[1] = { worldTransform_.rotation_.y };
		debugDir_[2] = { worldTransform_.rotation_.z };
	}
#endif // _DEBUG
}

void RailCamera::Update() {
	Vector3 movePos = { 0.0f,0.0f,0.0f };
	Vector3 moveRot = { 0.0f,0.0f,0.0f };

	Vector3 updatePos = worldTransform_.position_;
	Vector3 updateRota = worldTransform_.rotation_;

	//経過時間
	//timeRateが1.0f以上になったら、次の区間へ進む
	float timeRate = nowTime_ / endTime_;

	//時間経過
	if (isPhaseAdvance_) {
		if (phaseIndex_ < points_.size() - 3) {
			phaseIndex_++;
			nowTime_ = 0.0f;
		}
		else {
			phaseIndex_ = 1;
			timeRate = 1.0f;
		}

		isPhaseAdvance_ = false;
	}

	if (nowTime_ >= kTotalTime_) {
		nowTime_ = kTotalTime_;
	}
	else {
		nowTime_ += 1.0f;
	}

	{
	//if (timeRate >= 1.0f) {
	//	if (phaseIndex_ < points_.size() - 3) {
	//		phaseIndex_++;
	//		timeRate -= 1.0f;
	//	}
	//	else {
	//		phaseIndex_ = 1;
	//		timeRate = 1.0f;
	//	}
	//}

	//if (nowTime_ >= kTotalTime_) {
	//	nowTime_ = 0.0f;
	//}
	//else {
	//	nowTime_ += 1.0f;
	//}
	}

	//座標更新

	SetSplinePoint(
		splinePosStart_,
		splinePos1_,
		splinePos2_,
		splinePosEnd_);

	updatePos =
		SplinePosition(points_, phaseIndex_, timeRate);

	SetSplinePoint(
		splineDirStart_,
		splineDir1_,
		splineDir2_,
		splineDirEnd_);

	updateRota =
		SplinePosition(points_, phaseIndex_, timeRate);

	// オブジェクト移動
	if (input_->PressKey(DIK_W) ||
		input_->PressKey(DIK_S) ||
		input_->PressKey(DIK_D) ||
		input_->PressKey(DIK_A)) {
		if (input_->PressKey(DIK_W)) {
			moveRot += Vector3{ ConvertToRadian(1.0f),0,0};
			movePos += Vector3{ 0,0.1f,0 };
		}
		else if (input_->PressKey(DIK_S)) {
			moveRot -= Vector3{ ConvertToRadian(1.0f),0,0 };
			movePos -= Vector3{ 0,0.1f,0 };
		}
		if (input_->PressKey(DIK_D)) {
			moveRot += Vector3{ 0,ConvertToRadian(1.0f),0 };
			movePos += Vector3{ 0.1f,0,0 };
		}
		else if (input_->PressKey(DIK_A)) {
			moveRot -= Vector3{ 0,ConvertToRadian(1.0f),0 };
			movePos -= Vector3{ 0.1f,0,0 };
		}
	}

	updateRota += moveRot;
	updatePos += movePos;

	worldTransform_.rotation_ = updateRota;
	worldTransform_.position_ = updatePos;

	worldTransform_.UpdateMatrix();

	//#ifdef _DEBUG
	//	{
	//		worldTransform_.position_ = Vector3 {
	//			debugPos_[0],
	//			debugPos_[1],
	//			debugPos_[2], };
	//
	//		worldTransform_.rotation_ = Vector3 {
	//			debugDir_[0],
	//			debugDir_[1],
	//			debugDir_[2], };
	//	}
	//#endif // _DEBUG

	viewProjection_.eye_ = {
		worldTransform_.matWorld_.m[3][0],
		worldTransform_.matWorld_.m[3][1],
		worldTransform_.matWorld_.m[3][2] };

	//ワールド前方ベクトル
	Vector3 forward(0, 0, 1);
	//レールカメラの回転を反映
	forward = Vector3CrossMatrix4(forward, worldTransform_.matWorld_);

	//視点から前方に進んだ位置が注視点
	viewProjection_.target_ = viewProjection_.eye_ + forward;

	//ワールド上方ベクトル
	Vector3 up(0, 1, 0);
	//レールカメラの回転を反映
	viewProjection_.up_ = Vector3CrossMatrix4(up, worldTransform_.matWorld_);

	//ビュープロジェクション更新
	viewProjection_.UpdateMatrix();
	viewProjection_.TransferMatrix();
}

void RailCamera::DrawImGui() {
	debugPos_[0] = { worldTransform_.position_.x };
	debugPos_[1] = { worldTransform_.position_.y };
	debugPos_[2] = { worldTransform_.position_.z };

	debugDir_[0] = { worldTransform_.rotation_.x };
	debugDir_[1] = { worldTransform_.rotation_.y };
	debugDir_[2] = { worldTransform_.rotation_.z };

	ImGui::Begin("RailCamera");
	ImGui::SetWindowPos(ImVec2(0, 150));
	ImGui::SetWindowSize(ImVec2(500, 100));
	ImGui::SliderFloat3(
		"RailCameraPos", debugPos_, -PosRange_, PosRange_);
	ImGui::SliderFloat3(
		"RailCameraDir", debugDir_, 0, DirRange_);
	ImGui::End();
}

Vector3 RailCamera::SplinePosition(
	const std::vector<Vector3>& points,
	size_t startIndex,
	float t) {
	//補間するべき点の数
	size_t n = points.size() - 2;

	if (startIndex > n) return points[n]; //Pnの値を返す
	if (startIndex < 1) return points[1]; //P1の値を返す

	//p0～p3 の制御点を取得する。 ※ p1～p2を補完する。
	Vector3 p0 = points[startIndex -1];
	Vector3 p1 = points[startIndex];
	Vector3 p2 = points[startIndex + 1];
	Vector3 p3 = points[startIndex + 2];

	// Catmull-Rom の式で補間
	Vector3 position = CatmullRomSpline(p0, p1, p2, p3, t);

	return position;
}