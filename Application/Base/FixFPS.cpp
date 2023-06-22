#include "FixFPS.h"
#include <thread>

FixFPS* FixFPS::GetInstace() {
	static FixFPS instance;
	return &instance;
}

void FixFPS::Initialize() {
	//現在時間を記録
	reference_ = std::chrono::steady_clock::now();
}

void FixFPS::Update() {
	//1/60ピッタリの時間
	const std::chrono::microseconds
		kMinTime(uint64_t(1000000.0f / 60.0f));

	//1/60よりわずかに短い時間
	const std::chrono::microseconds
		kMinCheckTime(uint64_t(1000000.0f / 65.0f));

	//現在時間取得
	std::chrono::steady_clock::time_point now
		= std::chrono::steady_clock::now();

	//前回記録からの経過時間取得
	std::chrono::microseconds elapsed
		= std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);

	//1/60(よりわずかに短い時間)経ってない場合
	if (elapsed < kMinCheckTime) {
		//1/60秒経過するまで微小なスリープを繰り返す
		while (std::chrono::steady_clock::now() - reference_ < kMinTime) {
			//1マイクロ秒スリープ
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}
	//現在時間記録
	reference_ = std::chrono::steady_clock::now();
}