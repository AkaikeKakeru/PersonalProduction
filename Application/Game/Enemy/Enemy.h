#pragma once
#include "Model.h"
#include "Object3d.h"
#include "Sprite.h"
#include "DrawBasis.h"

#include <list>
#include <memory>

class CollisionManager;

//�G�l�~�[
class Enemy
	: public Object3d {
public: //�ÓI�����o�֐�
	//�I�u�W�F�N�g����
	static Enemy* Create(Model* model = nullptr);

public://�����o�֐�
	bool Initialize() override;
	void Update() override;
	void Draw();
	void DrawUI();
	void DrawImgui();
	void Finalize();

	//�Փˎ��R�[���o�b�N�֐�
	void OnCollision(const CollisionInfo& info) override;

public: //�A�N�Z�b�T
	const Vector3& GetPosition() const {
		return worldTransform_.position_;
	}
	float GetRadius() const {
		return radius_;
	}

private: //�ÓI�����o�ϐ�
	//�Փ˃}�l�[�W���[
	static CollisionManager* collisionManager_;

private: //�����o�ϐ�
	//���a
	float radius_ = 1.0f;

	//3d���e�B�N���̃��[���h�ϊ�
	WorldTransform worldTransform3dReticle_;

	//���e�B�N���p�X�v���C�g
	Sprite* spriteReticle_ = nullptr;

	//�e
	//std::list<std::unique_ptr<PlayerBullet>> bullets_;

public:
	Enemy() = default;
	~Enemy() = default;
};