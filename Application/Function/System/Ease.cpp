#include "Ease.h"

void Ease::Update() {
	if (is_) {
		//タイム
		timeRatio_ = (float)timer_ / timerMax_;

		switch (type_) {
		case Default_:
			nowVec_ = Lerp(
				startVec_,
				endVec_,
				timeRatio_
			);
			break;

		case In_:
			nowVec_ = EaseIn(
				startVec_,
				endVec_,
				timeRatio_
			);
			break;

		case Out_:
			nowVec_ = EaseOut(
				startVec_,
				endVec_,
				timeRatio_
			);
			break;

		case InOut_:
			nowVec_ = EaseInOut(
				startVec_,
				endVec_,
				timeRatio_
			);
			break;

		default:
			break;
		}

		//タイマーを進める
		if (timer_ < timerMax_) {
			timer_++;
		}
		//最大値を超えたら、現在値を最大値に
		else {
			timer_ = timerMax_;
			isEnd_ = true;
			is_ = false;
		}
	}
}

const Vector3 Ease::Lerp(
	const Vector3& start,
	const Vector3& end,
	const float t) {
	return start * (1.0f - t) + end * t;
}

const Vector3 Ease::EaseIn(
	const Vector3& start,
	const Vector3& end,
	const float t) {
	//ease-in
	float y = t * t;
	return start * (1.0f - y) + end * y;
}

const Vector3 Ease::EaseOut(
	const Vector3& start,
	const Vector3& end,
	const float t) {
	//ease-out
	float y = t * (2.0f - t);
	return start * (1.0f - y) + end * y;
}

const Vector3 Ease::EaseInOut(
	const Vector3& start,
	const Vector3& end,
	const float t) {
	float a = t * t;
	float b = 3.0f - (2.0f * t);

	//ease-in,ease-out
	float y = a * b;
	return start * (1.0f - y) + end * y;
}

const Vector3 Ease::CatmullRomSpline(
	Vector3 P0,
	Vector3 P1,
	Vector3 P2,
	Vector3 P3,
	float t) {
	Vector3 a = (2.0f * P1) + ((-P0 + P2) * t);
	Vector3 b = ((2.0f * P0) - (5.0f * P1) + (4.0f * P2) - P3) * t * t;
	Vector3 c = (-P0 + (3.0f * P1) - (3.0f * P2) + P3) * t * t * t;

	return 0.5f * (a + b + c);
}

Vector2 ConvertVector3ToVector2(const Vector3 v) {
	return Vector2(v.x, v.y);
}

Vector3 ConvertVector2ToVector3(const Vector2 v) {
	return Vector3(v.x, v.y, 0.0f);
}
