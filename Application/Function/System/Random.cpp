/*ƒ‰ƒ“ƒ_ƒ€‚È’l‚ğo—Í‚·‚éŠÖ”‚Ì“Z‚ß*/

#include "Random.h"

float RandomOutput(float min, float max) {
	std::random_device seed_gen;
	std::mt19937_64 engine(seed_gen());
	std::uniform_real_distribution<float> dist(min, max);

	return dist(engine);
}