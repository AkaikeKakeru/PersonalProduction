/*カーソルの2D座標を求める*/

#include "Cursor.h"
#include "Input.h"
#include "Framework.h"

#include <WinApp.h>
#include <cassert>

void Cursor::Initialize(const Camera& camera) {
	SetCamera(camera);
	CreateMatrixInverseVPV();

	float kCenterAnchorPoint_ = 0.5f;

	spriteReticle_ = std::make_unique<Sprite>();
	spriteReticle_->Initialize(Framework::kCursorTextureIndex_);
	spriteReticle_->SetAnchorPoint({
		kCenterAnchorPoint_, kCenterAnchorPoint_ });
	spriteReticle_->SetPosition({ WinApp::Win_Width / 2, WinApp::Win_Height / 2 });
	spriteReticle_->SetSize({ 64,64 });
}

void Cursor::Update(
	const Camera& camera,
	const Vector3& targetWorldPos) {
	SetCamera(camera);
	SetTargetPos(targetWorldPos);

	CreateMatrixInverseVPV();

	CheckRayDirection();
	LockOn(targetPos_);

	//EasePosition();

	//reticlePos_ = reticleMove_;


	spriteReticle_->SetPosition(TransFromWorldToScreen(reticlePos_));
	spriteReticle_->Update();
}

void Cursor::Draw() {
	spriteReticle_->Draw();
}

Vector3& Cursor::Get3DReticlePosition(
	//Camera* camera,
	const Vector3 targetWorldPos) {
	CreateMatrixInverseVPV();

	CheckRayDirection();
	LockOn(targetWorldPos);

	//EasePosition();

	//reticlePos_ = reticleMove_;

	return reticlePos_;
}

void Cursor::CreateMatrixInverseVPV() {
	//ビューポート行列
	Matrix4 matViewPort = Matrix4Identity();
	matViewPort.m[0][0] = static_cast<float>(WinApp::Win_Width) / 2;
	matViewPort.m[1][1] = static_cast<float>(-(WinApp::Win_Height)) / 2;
	matViewPort.m[3][0] = static_cast<float>(WinApp::Win_Width) / 2;
	matViewPort.m[3][1] = static_cast<float>(WinApp::Win_Height) / 2;

	//ビュープロジェクションビューポート行列の更新
	matVPV_ = camera_->GetViewMatrix()
		* camera_->GetProjectionMatrix()
		* matViewPort;

	//上を逆行列化
	matInverseVPV_ = Matrix4Inverse(matVPV_);
}

void Cursor::CheckRayDirection() {
	//マウスの座標を取得
	Vector2 mousePosition_ =
		Input::GetInstance()->GetMousePosition();

	//照準位置を書き換え
	reticlePos_ = TransFromScreenToWorld(mousePosition_);
}

void Cursor::LockOn(const Vector3& targetWorldPos) {
	//標的ワールド位置を、スクリーン座標に変換
	Vector2 screenOfTargetWorldPos = TransFromWorldToScreen(targetWorldPos);
	//レティクル位置を、スクリーン座標に変換
	Vector2 screenPosOfReticle = TransFromWorldToScreen(reticlePos_);

	//ロックオン中なら、リリースするタイミングをうかがう
	if (isLockOn_) {
		//if (isStand_) {
			//スクリーン座標における、二者の位置関係を確認する
			//※このifにおけるレティクル位置は、ロックオン範囲分を画面外側にずらして考えるものとする
			//全て通るなら、リリースを行う
			if (/*レティクルが標的より外側にいるかどうか*/
				   screenOfTargetWorldPos.x > screenPosOfReticle.x + kLockOnRange_
				|| screenOfTargetWorldPos.x < screenPosOfReticle.x - kLockOnRange_
				|| screenOfTargetWorldPos.y > screenPosOfReticle.y + kLockOnRange_
				|| screenOfTargetWorldPos.y < screenPosOfReticle.y - kLockOnRange_) {
				isLockOn_ = false;

				//マウスの座標を取得
				Vector2 mousePosition_ =
					Input::GetInstance()->GetMousePosition();
				reticlePos_ = TransFromScreenToWorld(mousePosition_);
			}
		//}
	}
	//ちがうなら、ロックオンのタイミングをうかがう
	else {
		//if (isStand_) {
			//スクリーン座標における、二者の位置関係を確認する
			//※このifにおけるレティクル位置は、ロックオン範囲分を画面外側にずらして考えるものとする
			//全て通るなら、ロックオンを行う
			if (/*標的がレティクルより左にいるかどうか*/
				   screenOfTargetWorldPos.x <= screenPosOfReticle.x + kLockOnRange_
				/*標的がレティクルより右にいるかどうか*/
				&& screenOfTargetWorldPos.x >= screenPosOfReticle.x - kLockOnRange_
				/*標的がレティクルより上にいるかどうか*/
				&& screenOfTargetWorldPos.y <= screenPosOfReticle.y + kLockOnRange_
				/*標的がレティクルより下にいるかどうか*/
				&& screenOfTargetWorldPos.y >= screenPosOfReticle.y - kLockOnRange_) {
				isLockOn_ = true;


				//reticlePos_ = {
				//	screenOfTargetWorldPos.x,
				//	screenOfTargetWorldPos.y,
				//	0.0f
				//};


			}
		//}

		//レティクルのスクリーン座標を、ワールド座標に変換しなおす
		//reticleMove_ = TransFromScreenToWorld(screenPosOfReticle);
	}
}

void Cursor::LockOnBef(const Vector3& targetWorldPos) {
	//最終結果保存用
	Vector3 result{};

	//標的ワールド位置を、スクリーン座標に変換
	Vector2 screenOfTargetWorldPos = TransFromWorldToScreen(targetWorldPos);
	//レティクル位置を、スクリーン座標に変換
	Vector2 screenPosOfReticle = TransFromWorldToScreen(reticlePos_);

	//ロックオン中なら、リリースするタイミングをうかがう
	if (isLockOn_) {
		if (isStand_) {
			//スクリーン座標における、二者の位置関係を確認する
			//※このifにおけるレティクル位置は、ロックオン範囲分を画面外側にずらして考えるものとする
			//全て通るなら、リリースを行う
			if (/*レティクルが標的より外側にいるかどうか*/
				screenOfTargetWorldPos.x > screenPosOfReticle.x + kLockOnRange_
				|| screenOfTargetWorldPos.x < screenPosOfReticle.x - kLockOnRange_
				|| screenOfTargetWorldPos.y > screenPosOfReticle.y + kLockOnRange_
				|| screenOfTargetWorldPos.y < screenPosOfReticle.y - kLockOnRange_) {
				isLockOn_ = false;
				isStand_ = false;

				{
					//イージングの開始位置を、標的のスクリーン座標に設定
					//※標的のスクリーン座標をワールド座標に変換しなおす
					easeStartPos_ = TransFromScreenToWorld({
						screenOfTargetWorldPos.x,
						screenOfTargetWorldPos.y });

					//イージングの終了位置を、レティクルのスクリーン座標に設定
					//※レティクルのスクリーン座標を、ワールド座標に変換しなおす
					easeEndPos_ = TransFromScreenToWorld({
						screenPosOfReticle.x,
						screenPosOfReticle.y
						}
					);

					//イージングをリセット
					ease_.Reset(Ease::InOut_, kEaseTimerRelease_, easeStartPos_, easeEndPos_);
					ease_.SetIs(true);
				}
			}
		}
	}
	//ちがうなら、ロックオンのタイミングをうかがう
	else {
		if (isStand_) {
			//スクリーン座標における、二者の位置関係を確認する
			//※このifにおけるレティクル位置は、ロックオン範囲分を画面外側にずらして考えるものとする
			//全て通るなら、ロックオンを行う
			if (/*標的がレティクルより左にいるかどうか*/
				screenOfTargetWorldPos.x <= screenPosOfReticle.x + kLockOnRange_
				/*標的がレティクルより右にいるかどうか*/
				&& screenOfTargetWorldPos.x >= screenPosOfReticle.x - kLockOnRange_
				/*標的がレティクルより上にいるかどうか*/
				&& screenOfTargetWorldPos.y <= screenPosOfReticle.y + kLockOnRange_
				/*標的がレティクルより下にいるかどうか*/
				&& screenOfTargetWorldPos.y >= screenPosOfReticle.y - kLockOnRange_) {
				isLockOn_ = true;
				isStand_ = false;

				{
					//イージングの開始位置を、レティクルのスクリーン座標に設定
					//※レティクルのスクリーン座標をワールド座標に変換しなおす
					easeStartPos_ = TransFromScreenToWorld({
						screenPosOfReticle.x,
						screenPosOfReticle.y });

					//イージングの終了位置を、標的のスクリーン座標に設定
					//※標的のスクリーン座標を、ワールド座標に変換しなおす
					easeEndPos_ = TransFromScreenToWorld({
						screenOfTargetWorldPos.x,
						screenOfTargetWorldPos.y
						}
					);

					//イージングをリセット
					ease_.Reset(Ease::InOut_, kEaseTimerLockOn_, easeStartPos_, easeEndPos_);
					ease_.SetIs(true);
				}
			}
		}

		//レティクルのスクリーン座標を、標的スクリーン座標の位置に移動させる
		result = {
			screenPosOfReticle.x,
			screenPosOfReticle.y,
			0.0f };

		//レティクルのスクリーン座標を、ワールド座標に変換しなおす
		reticlePos_ = TransFromScreenToWorld({
			result.x,
			result.y
			}
		);
	}
}

void Cursor::EasePosition() {
	////イーズ終わりで、待機中にする
	//if (ease_.IsEnd()) {
	//	isStand_ = true;
	//}

	////待機中でなければイージングでレティクルの座標を更新
	//if (!isStand_) {
	//	ease_.Update();
	//	reticleMove_ = ease_.GetReturn();
	//}
}

Vector3& Cursor::TransFromScreenToWorld(const Vector2& screenPos) {
	//最終結果保存用
	static Vector3 result{};

	assert(distance_);

	//ニア
	Vector3 posNear_ = Vector3(
		screenPos.x,
		screenPos.y,
		0);
	posNear_ = Vector3TransformCoord(posNear_, matInverseVPV_);
	//ファー
	Vector3 posFar_ = Vector3(
		screenPos.x,
		screenPos.y,
		1);
	posFar_ = Vector3TransformCoord(posFar_, matInverseVPV_);

	//レイ
	result = posNear_ - posFar_;
	result = Vector3Normalize(result);

	//ニア→レイ
	result = posNear_ - (result * distance_);

	return result;
}

Vector2& Cursor::TransFromWorldToScreen(const Vector3& worldPos) {
	//最終結果保存用
	static Vector2 result;
	//途中過程保存用
	Vector3 process = Vector3TransformCoord(worldPos, matVPV_);

	result = { process.x,process.y };

	return result;
}
