/*イージング*/

#pragma once
#include <Vector2.h>
#include <Vector3.h>

/*イージング*/
class Ease {
public:
	//イーズタイプ
	enum EaseType {
		Default_ = 0,
		In_ = 1,
		Out_ = 2,
		InOut_ = 3,
	};

public:
	void Update();

	/// <summary>
	/// リセット
	/// </summary>
	/// <param name="type">タイプ</param>
	/// <param name="timerMax">タイマー最大値</param>
	/// <param name="startVec">開始Vector3</param>
	/// <param name="endVec">終了Vector3</param>
	void Reset(
		int type = Default_,
		int timerMax = 60,
		Vector3 startVec = { 0,0,0 },
		Vector3 endVec = { 0,0,0 }
	) {
		type_ = type,
			timer_ = 0;
		timerMax_ = timerMax;
		startVec_ = startVec;
		endVec_ = endVec;
		nowVec_ = startVec;
		is_ = true;
		isEnd_ = false;
	};

public:
	//スプライン曲線用
	const Vector3 CatmullRomSpline(
		Vector3 P0,
		Vector3 P1,
		Vector3 P2,
		Vector3 P3,
		float t);

private:
	///線形補間
	//基本補間
	const Vector3 Lerp(
		const Vector3& start,
		const Vector3& end,
		const float t);

	//開始時早い補間
	const Vector3 EaseIn(
		const Vector3& start,
		const Vector3& end,
		const float t);

	//終了時早い補間
	const Vector3 EaseOut(
		const Vector3& start,
		const Vector3& end,
		const float t);

	//開始時と終了時早い補間
	const Vector3 EaseInOut(
		const Vector3& start,
		const Vector3& end,
		const float t);

public:
	Vector3 GetReturn() {
		return nowVec_;
	}

	float GetTimeRatio() {
		return timeRatio_;
	}

	void SetStart(Vector3 start) {
		startVec_ = start;
	}
	void SetEnd(Vector3 end) {
		startVec_ = end;
	}

	void SetTimer(int time, int MaxTime) {
		timer_ = time;
		timerMax_ = MaxTime;
	}

	bool Is() {
		return is_;
	}
	void SetIs(bool is) {
		is_ = is;
	}

	bool IsEnd() {
		return isEnd_;
	}
	void SetIsEnd(bool is) {
		isEnd_ = is;
	}

private:
	//タイプ
	int type_ = Default_;

	//タイム(割合)
	float timeRatio_;

	//タイマー
	int timer_ = 0;

	//タイマー最大値
	int timerMax_ = 60;

	//開始
	Vector3 startVec_;

	//終了
	Vector3 endVec_;

	//現在
	Vector3 nowVec_;

	//実行フラグ
	bool is_;

	//終了フラグ
	bool isEnd_;
};

//ベクター3をベクター2に変換
Vector2 ConvertVector3ToVector2(const Vector3 v);
//ベクター2をベクター3に変換
Vector3 ConvertVector2ToVector3(const Vector2 v);
