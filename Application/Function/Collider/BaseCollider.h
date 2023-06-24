#pragma once

#include "CollisionTypes.h"
#include "CollisionInfo.h"
#include "Object3d.h"

class BaseCollider {
public: //フレンドクラス
	friend class CollisionManager;

public: //メンバ関数
	virtual void Update() = 0;

public: //アクセッサ
	//オブジェクトセット
	inline void SetObject(Object3d* object) {
		object_ = object; }
	//オブジェクト取得
	inline Object3d* GetObject3d() {
		return object_; }

	//形状タイプ取得
	inline CollisionShapeType GetShapeType() {
		return shapeType_; }

	//衝突時コールバック関数
	inline void OnCollision(const CollisionInfo& info) {
		object_->OnCollision(info);
	}

	//属性セット
	inline void SetAttribute(unsigned short attribute) {
		this->attribute_ = attribute;
	}
	//属性追加
	inline void AddAtribute(unsigned short attribute) {
		this->attribute_ |= attribute;
	}
	//属性取り除き
	inline void RemoveAttribute(unsigned short attribute) {
		this->attribute_ &= !attribute;
	}

protected: //メンバ変数
	Object3d* object_ = nullptr;
	//形状タイプ
	CollisionShapeType shapeType_ = SHAPE_UNKNOWN;

	//衝突属性
	unsigned short attribute_ = 0b1111111111111111;

public: //コンストデスト
	BaseCollider() = default;
	virtual ~BaseCollider() = default;

};