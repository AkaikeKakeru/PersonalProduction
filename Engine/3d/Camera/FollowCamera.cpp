#include "FollowCamera.h"

Input* FollowCamera::input_ = Input::GetInstance();

void FollowCamera::Initialize() {
	worldTransform_.position_ = startPos_;
	worldTransform_.rotation_ = startRota_;

	viewProjection_.Initialize();

	rotSpeed_ = {
		ConvertToRadian(2.0f),
		ConvertToRadian(2.0f),
		ConvertToRadian(2.0f) };

	offset_ = defaultOffSetPos_;

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

void FollowCamera::Update() {
	offset_ = defaultOffSetPos_;

	//追従対象のワールド座標を、別途確保
	Vector3 targetPos = {
		target_->matWorld_.m[3][0],
		target_->matWorld_.m[3][1],
		target_->matWorld_.m[3][2] };

	//追従対象がいるなら、カメラの位置を更新
	if (target_) {
		//追従対象のワールド行列で、オフセットを回転させる
		offset_ = Vector3CrossMatrix4(
			offset_,
			target_->matWorld_);

		//追従対象からオフセット分を遠ざけた座標を、カメラ位置に指定
		worldTransform_.position_ = targetPos + offset_;
	}

	worldTransform_.UpdateMatrix();

	//ビュープロジェクションの視点はカメラ位置
	viewProjection_.eye_ = worldTransform_.position_;
	//ビュープロジェクションの注視点は追従対象のワールド座標
	viewProjection_.target_ = targetPos;

	//ワールド上方ベクトル
	Vector3 up(0, 1, 0);
	//レールカメラの回転を反映
	viewProjection_.up_ = Vector3CrossMatrix4(up, worldTransform_.matWorld_);
	//ビュープロジェクション更新
	viewProjection_.UpdateMatrix();
	viewProjection_.TransferMatrix();
}

void FollowCamera::Finalize() {
}
