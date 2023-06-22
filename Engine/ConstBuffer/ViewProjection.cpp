#include "ViewProjection.h"
#include "WinApp.h"
#include <cmath>
#include <cassert>
#include "DirectXBasis.h"

void ViewProjection::Initialize() {
	// ビュー行列
	matView_ = Matrix4Identity();
	// 射影行列
	matProjection_ = Matrix4Identity();

	eye_ = { 0, 10, -20.0f };
	target_ = { 0, 0, 0 };
	up_ = { 0, 1, 0 };

	angle_ = ConvertToRadian(45.0f);
	aspect_ = (float)WinApp::Win_Width/ WinApp::Win_Height;
	nearClip_ = 0.1f;
	farClip_ = 1000.0f;

	CreateConstBuffer();
	Maping();
	UpdateMatrix();
	TransferMatrix();
}

void ViewProjection::CreateConstBuffer() {
	HRESULT result;

	// ヒーププロパティ
	D3D12_HEAP_PROPERTIES heapProps{};
	heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

	// リソース設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = (sizeof(ConstBufferDataViewProjection) + 0xff) & ~0xff;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;


	// 定数バッファの生成
	result = DirectXBasis::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProps, // アップロード可能
		D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&constBuff_));
	assert(SUCCEEDED(result));
}

void ViewProjection::Maping() {
	// 定数バッファとのデータリンク
	HRESULT result = constBuff_->Map(0, nullptr, (void**)&constMap_);
	assert(SUCCEEDED(result));
}

void ViewProjection::UpdateMatrix() {
	//ビュー行列の計算
	UpdateViewMatrix();
	//プロジェクション行列の計算
	UpdateProjectionMatrix();

	matViewProjection_ = matView_ * matProjection_;
}

void ViewProjection::UpdateProjectionMatrix() {
	Vector4 pers = {
		1 / (static_cast<float>(tan(angle_ / 2))) / aspect_,
		1 / (static_cast<float>(tan(angle_ / 2))),
		1 / (farClip_ - nearClip_) * farClip_,
		-nearClip_ / (farClip_ - nearClip_) * farClip_,
	};

	matProjection_ = Matrix4Identity();
	matProjection_ = {
		pers.x,0,0,0,
		0,pers.y,0,0,
		0,0,pers.z,1,
		0,0,pers.w,0
	};
}

void ViewProjection::UpdateViewMatrix() {
	Vector3 axisZ = Vector3Normalize(target_ - eye_);
	Vector3 axisX = Vector3Normalize(Vector3Cross(up_, axisZ));
	Vector3 axisY = Vector3Cross(axisZ, axisX);

	//カメラ回転行列
	Matrix4 matCameraRot;
	//カメラ座標系→ワールド座標系の変換行列0
	matCameraRot = {
		axisX.x,axisX.y,axisX.z,0,
		axisY.x,axisY.y,axisY.z,0,
		axisZ.x,axisZ.y,axisZ.z,0,
		0,0,0,1
	};

	//転置により逆行列(逆回転)を計算
	matView_ = Matrix4Transposed(matCameraRot);

	//視点座標に-1を掛けた座標
	Vector3 reverseEyePosition = eye_ * -1;
	//カメラの位置からワールド原点へのベクトル(カメラ座標系)
	Vector3 cameraMoveVal_ = {
		Vector3Dot(reverseEyePosition,axisX),
		Vector3Dot(reverseEyePosition,axisY),
		Vector3Dot(reverseEyePosition,axisZ)
	};

	//ビュー行列に平行移動成分を設定
	matView_.m[3][0] = cameraMoveVal_.x;
	matView_.m[3][1] = cameraMoveVal_.y;
	matView_.m[3][2] = cameraMoveVal_.z;

	// ビルボード行列
	matBillboard_ = {
		axisX.x,axisX.y,axisX.z,0,
		axisY.x,axisY.y,axisY.z,0,
		axisZ.x,axisZ.y,axisZ.z,0,
		0,0,0,1
	};

	// カメラX軸、Y軸、Z軸
	Vector3 ybillCameraAxisX, ybillCameraAxisY, ybillCameraAxisZ;

	// X軸は共通
	ybillCameraAxisX = axisX;
	// Y軸はワールド座標系のY軸
	ybillCameraAxisY = Vector3Normalize(up_);
	// Z軸はX軸→Y軸の外積で求まる
	ybillCameraAxisZ = Vector3Cross(ybillCameraAxisX, ybillCameraAxisY);

	// Y軸回りビルボード行列
	matBillboard_ = {
		ybillCameraAxisX.x,ybillCameraAxisX.y,ybillCameraAxisX.z,0,
		ybillCameraAxisY.x,ybillCameraAxisY.y,ybillCameraAxisY.z,0,
		ybillCameraAxisZ.x,ybillCameraAxisZ.y,ybillCameraAxisZ.z,0,
		0,0,0,1
	};
}

void ViewProjection::TransferMatrix() {
	// 定数バッファに書き込み
	constMap_->view_ = matView_;
	constMap_->projection_ = matProjection_;
	constMap_->cameraPos_ = eye_;
}
