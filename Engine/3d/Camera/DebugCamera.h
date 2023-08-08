#pragma once
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Input.h"

//デバッグカメラ
class DebugCamera {
public://静的メンバ変数
	static Input* input_;

public:
	void Initialize(const Vector3 pos,
		const Vector3 rotaRadianAngle);
	void Update();

private:
	//ワールド変換
	WorldTransform worldTransform_;
	//ビュープロジェクション
	ViewProjection viewProjection_;

};