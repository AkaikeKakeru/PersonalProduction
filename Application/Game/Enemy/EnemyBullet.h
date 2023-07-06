#pragma once
#include "Model.h"
#include "Object3d.h"
#include "Sprite.h"
#include "DrawBasis.h"

class CollisionManager;

class EnemyBullet :
	public Object3d{
public: //�ÓI�����o�֐�
	//�I�u�W�F�N�g����
	static EnemyBullet* Create(Model* model = nullptr);

public://�����o�֐�
	bool Initialize() override;
	void Update() override;
	void Draw();
	void Finalize();

	//�Փˎ��R�[���o�b�N�֐�
	void OnCollision(const CollisionInfo & info) override;

public: //�A�N�Z�b�T
	const Vector3& GetPosition() const {
		return worldTransform_.position_; }
	float GetRadius() const {
		return radius_; }

	//���x�̃Z�b�g
	void SetVelocity(Vector3 velocity) {
		velocity_ = velocity;
	}

	//���󂵂������擾
	bool IsDead() const {
		return isDead_;
	}

private: //�ÓI�����o�ϐ�
	//�Փ˃}�l�[�W���[
	static CollisionManager* collisionManager_;

public://�����o�萔
	//����<�t���[���P��>
	static const int32_t kLifeTime_ = 60 * 5;

private: //�����o�ϐ�
	//���a
	float radius_ = 1.0f;

	//���x
	Vector3 velocity_ = {};

	//����^�C�}�[
	int32_t deathTimer_ = kLifeTime_;

	//����t���O
	bool isDead_ = false;

public:
	EnemyBullet() = default;
	~EnemyBullet() = default;
};