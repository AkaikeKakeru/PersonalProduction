/*シェイク*/

#include "Shake.h"
#include "Random.h"

void Shake::Update() {
	if (is_) {
		ShakePosition(widthSwing_);
		widthSwing_--;
	}

	if (widthSwing_ <= 0) {
		is_ = false;
		output_ = {0,0,0};
	}
}

void Shake::ShakePosition(
	float widthSwing) {
	//位置座標をランダムに分布
	Vector3 pos{};
	pos.x = RandomOutput(-widthSwing / 2, widthSwing / 2);
	pos.y = RandomOutput(-widthSwing / 2, widthSwing / 2);
	pos.z = RandomOutput(-widthSwing / 2, widthSwing / 2);

	output_ = pos;
}
