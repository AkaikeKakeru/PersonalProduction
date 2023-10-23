/*タイルを並べるようなシーン遷移オブジェクト*/

#include "ArrangeTile.h"
#include <SafeDelete.h>

#include "Vector3.h"

void ArrangeTile::Initialize(
	int32_t texIndex,
	const Vector2 pos,
	const float rotation,
	const Vector2 size,
	const int value) {

	for (size_t i = 0; i < value; i++) {
		AddSprite(texIndex, pos, rotation, size);
	}

	timerMax_ = 60 / 3;

	posStart_ = pos;
	sizeStart_ = {};
	sizeEnd_ = size;

	is_ = true;
	isOpen_ = true;
	isEnd_ = false;

	judgeSize_ = size;
}

void ArrangeTile::Update() {
	//移動
	Vector3 move{};
	//ワイド
	Vector3 wide{};

	//イージングの開始点
	Vector2 easeS{};
	//イージングの終了地点
	Vector2 easeE{};

	//横に並べる枚数
	float width = (WinApp::Win_Width / judgeSize_.x) + 1;
	//縦に並べる枚数
	float height = (WinApp::Win_Height / judgeSize_.y);
	//タイム
	float time = (float)timerNow_ / timerMax_;

	if (is_) {
		for (size_t h = 0; h < height; h++) {
			for (size_t w = 0; w < width; w++) {
				//今確認中のスプライト番号
				int nowSpriteNum = (int)((h * width) + w);

				//通過点を決める
				posPass_ = {
					w * judgeSize_.x ,
					h * judgeSize_.y
				};

				sizePass_ = sizeEnd_;

				//幕を開くときは画面内から画面外に
				//幕を閉じるときは、画面外から画面内に
				easeS = isOpen_ ? posPass_ : posStart_;
				easeE = isOpen_ ?
					Vector2{
					posPass_.x ,
					posPass_.y + 900.0f
				} : posPass_;

				move = EaseOut(
					ConvertVector2ToVector3(easeS),
					//終了位置
					ConvertVector2ToVector3(easeE),
					time
				);

				//幕を閉じるときは、スプライトを拡大していく
				easeS = isOpen_ ? sizePass_ : sizeStart_;
				easeE = sizePass_;

				wide = EaseOut(
					//開始サイズ
					ConvertVector2ToVector3(easeS),
					//終了位置
					ConvertVector2ToVector3(easeE),
					time
				);

				sprites_[nowSpriteNum]->SetPosition(
					ConvertVector3ToVector2(move)
				);
				sprites_[nowSpriteNum]->SetSize(
					ConvertVector3ToVector2(wide)
				);

				sprites_[nowSpriteNum]->Update();
			}
		}

		//タイマーを進める
		if (timerNow_ < timerMax_) {
			timerNow_++;
		}
		//最大値を超えたら、現在値を最大値に
		else {
			timerNow_ = timerMax_;
			is_ = false;

			if (!isOpen_) {
				isEnd_ = true;
			}
		}
	}
}

void ArrangeTile::Draw() {
	for (std::unique_ptr<Sprite>& sprite : sprites_) {
		sprite->Draw();
	}
}

void ArrangeTile::Finalize() {
	//SafeDelete(sprite_);
}

void ArrangeTile::AddSprite(
	int32_t texIndex,
	const Vector2 pos,
	const float rotation,
	const Vector2 size) {
	std::unique_ptr<Sprite> newSprite =
		std::make_unique<Sprite>();

	newSprite->Initialize(texIndex);
	newSprite->SetSize(size);
	newSprite->SetRotation(rotation);
	newSprite->SetPosition(pos);
	newSprite->SetAnchorPoint({ 0.5f,0.5f });
	newSprite->Update();
	//リストに登録
	sprites_.push_back(std::move(newSprite));
}

void ArrangeTile::Reset(
	bool is,
	bool isOpen) {
	timerNow_ = 0;

	is_ = is;
	isOpen_ = isOpen;
	isEnd_ = false;
}

Vector2 ArrangeTile::ConvertVector3ToVector2(const Vector3 v) {
	return Vector2(v.x, v.y);
}

Vector3 ArrangeTile::ConvertVector2ToVector3(const Vector2 v) {
	return Vector3(v.x, v.y, 0.0f);
}
