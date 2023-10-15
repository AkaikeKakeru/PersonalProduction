/*FPSを60に固定させる*/

#pragma once
#include <chrono>

//FPSを固定
class FixFPS {
public: //基本メンバ関数
	void Initialize();
	void Update();

private: //メンバ変数
	//記録時間(FPS固定用)
	std::chrono::steady_clock::time_point reference_;

private:
	FixFPS() = default;
	~FixFPS() = default;
	FixFPS(const FixFPS&) = delete;
	const FixFPS& operator=(const FixFPS&) = delete;

public://static
	static FixFPS* GetInstace();
};