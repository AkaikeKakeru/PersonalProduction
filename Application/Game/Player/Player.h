#pragma once
#include "Model.h"
#include "Object3d.h"
#include "Camera.h"

class CollisionManager;

//�v���C���[
class Player 
	: public Object3d {
public: //�ÓI�����o�֐�
	//�I�u�W�F�N�g����
	static Player* Create(Model* model = nullptr);

public://�����o�֐�
	bool Initialize() override;
	void Update() override;
	void Draw();
	void Finalize();

	//�Փˎ��R�[���o�b�N�֐�
	void OnCollision(const CollisionInfo & info) override;

public: //�A�N�Z�b�T
	const Vector3& GetPosition() const { return worldTransform_.position_; }
	float GetRadius() const { return radius_; }

private: //�ÓI�����o�ϐ�
	//�Փ˃}�l�[�W���[
	static CollisionManager* collisionManager_;

private: //�����o�ϐ�
	//���a
	float radius_ = 1.0f;

public:
	Player() = default;
	~Player() = default;
};