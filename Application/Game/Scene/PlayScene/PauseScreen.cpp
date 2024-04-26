#include "PauseScreen.h"
#include "Framework.h"
void PauseScreen::Initialize() {
	float size = 64.0f;
	float texSize = 64.0f;
	float kCenterAnchorPoint_ = 0.5f;

	windowSprite_ = std::make_unique<Sprite>();
	windowSprite_->Initialize(Framework::kPauseTextureIndex_);
	windowSprite_->SetSize({
		WinApp::Win_Width, WinApp::Win_Height
		});
	windowSprite_->SetPosition({
		-WinApp::Win_Width / 2, -WinApp::Win_Height / 2
		});
	windowSprite_->SetAnchorPoint({
		kCenterAnchorPoint_,kCenterAnchorPoint_
		});
	windowSprite_->Update();
	windowSprite_->SetPosition({
		WinApp::Win_Width / 2, WinApp::Win_Height / 2
		});

	pauseLogoSprite_ = std::make_unique<Sprite>();
	pauseLogoSprite_->Initialize(Framework::kMouseTextureIndex_);
	pauseLogoSprite_->SetPosition({
		-WinApp::Win_Width / 2, -WinApp::Win_Height / 2
		});
	pauseLogoSprite_->SetSize({ size * 5.0f, size * 2.5f });
	pauseLogoSprite_->SetAnchorPoint({
		kCenterAnchorPoint_,
		kCenterAnchorPoint_ });
	pauseLogoSprite_->SetTextureSize({
		texSize * 1.5f,
		texSize / 2 });
	pauseLogoSprite_->SetTextureLeftTop({
		texSize,
		(texSize * 2) + (texSize / 2) + (texSize / 2)  });

	pauseLogoSprite_->Update();

	pauseLogoSprite_->SetPosition({
		WinApp::Win_Width/2, WinApp::Win_Height / 4
		});
}

void PauseScreen::Update() {
	Vector3 updateSize{};

	if (sizeEase_.IsEnd()) {
		if (isRemove_) {
			isRun_ = !isRun_;
			isRemove_ = !isRemove_;
		}
	}
	else {
		sizeEase_.Update();
	}

	updateSize = sizeEase_.GetReturn();

	windowSprite_->SetSize({
		updateSize.x,updateSize.y
		});

	pauseLogoSprite_->SetSize({
		updateSize.x / 5.0f,updateSize.y / 5.0f
		});

	windowSprite_->Update();
	pauseLogoSprite_->Update();
}

void PauseScreen::Draw() {
	windowSprite_->Draw();
	pauseLogoSprite_->Draw();
}

void PauseScreen::ResetEase_Run() {
	isRun_ = true;

	sizeEase_.Reset(
		Ease::Out_,
		PauseScreen::cMaxTimeEase_,
		sizeEase_.GetReturn(),
		{
			WinApp::Win_Width,
			WinApp::Win_Height,
			0.0f
		}
	);
}

void PauseScreen::ResetEase_Remove() {
	sizeEase_.Reset(
		Ease::Out_,
		PauseScreen::cMaxTimeEase_,
		sizeEase_.GetReturn(),
		{
			0.0f,
			0.0f,
			0.0f
		}
	);
}
