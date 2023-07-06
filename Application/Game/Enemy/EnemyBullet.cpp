#include "EnemyBullet.h"
#include <Input.h>
#include "CollisionManager.h"
#include "CollisionAttribute.h"
#include "SphereCollider.h"
#include <cassert>

CollisionManager* EnemyBullet::collisionManager_ = CollisionManager::GetInstance();

EnemyBullet* EnemyBullet::Create(Model* model) {
	//�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	EnemyBullet* instance = new EnemyBullet();
	if (instance == nullptr) {
		return nullptr;
	}

	//������
	if (!instance->Initialize()) {
		delete instance;
		assert(0);
	}

	//���f���̃Z�b�g
	if (model) {
		instance->SetModel(model);
	}

	return instance;
}

bool EnemyBullet::Initialize() {
	if (!Object3d::Initialize()) {
		return false;
	}

	//�R���C�_�|�ǉ�

	//���a�������������畂�������W�����̒��S�ɂ���
	SetCollider(new SphereCollider(
		Vector3{ 0.0f,radius_,0.0f },
		radius_)
	);

	collider_->SetAttribute(COLLISION_ATTR_PLAYER);

	return true;
}

void EnemyBullet::Update() {
	// ���݂̍��W���擾
	Vector3 position = Object3d::GetPosition();
	// ���݂̉�]���擾
	Vector3 rot = Object3d::GetRotation();

	//���t���[���A�x���V�e�B���O�i
	position += velocity_;

	// ���W�̉�]�𔽉f
	Object3d::SetRotation(rot);
	// ���W�̕ύX�𔽉f
	Object3d::SetPosition(position);

	Object3d::Update();

	//���Ԍo�߂Ŏ��󂳂���
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}
}

void EnemyBullet::Draw() {
	Object3d::Draw(worldTransform_);
}

void EnemyBullet::Finalize() {
}

void EnemyBullet::OnCollision(const CollisionInfo& info) {
	CollisionInfo colInfo = info;
}
