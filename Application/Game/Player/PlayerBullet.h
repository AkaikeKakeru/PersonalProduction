/*プレイヤー弾*/

#pragma once
//#include "Model.h"
//#include "Object3d.h"
//#include "Sprite.h"
//#include "SpriteBasis.h"
//
//class CollisionManager;
//class GamePlayScene;

#include "Bullet.h"

//プレイヤー弾
class PlayerBullet :
	public Bullet{
public: //静的メンバ関数
	//オブジェクト生成
	static PlayerBullet* Create(Model* model = nullptr);

public://定数

public://メンバ関数
	bool Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;

	//衝突時コールバック関数
	void OnCollision(const CollisionInfo & info) override;

public: //アクセッサ
	
private: //静的メンバ変数

public://メンバ定数

private: //メンバ変数

public:
	PlayerBullet() = default;
	~PlayerBullet() = default;
};
