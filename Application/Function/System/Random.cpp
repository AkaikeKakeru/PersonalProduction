/*ランダムな値を出力する関数の纏め*/

#include "Random.h"

float RandomOutputFloat(float min, float max) {
	std::random_device seed_gen;
	std::mt19937_64 engine(seed_gen());
	std::uniform_real_distribution<float> dist(min, max);

	return dist(engine);
}
