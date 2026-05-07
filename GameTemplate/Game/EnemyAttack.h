#pragma once

class Player;
class Enemy;

class EnemyAttack: public IGameObject
{
 public:
	EnemyAttack();
	~EnemyAttack();
	bool Start() override;
	void Update() override;
	//コリジョンの作成。
	void CreateCollision();
	//エフェクトの作成。
	void CreateEffect();
	void Render(RenderContext& rc) override;

	Player* m_player = nullptr;                     // 攻撃の対象
	Enemy* m_enemy = nullptr;                       // 攻撃オブジェクトの所有者
	CollisionObject* m_collisionObj;				//コリジョンオブジェクト。
	EffectEmitter* m_effectEmitter;			        //エフェクト
	const float			m_amuletSpeed = 500.0f;	    //攻撃の移動速度。
	float				m_deleteTimer = 0.0f;		//削除を管理するタイマー。
	Vector3				m_position;					//座標。
	Vector3             m_direction = Vector3::Front; // エネミーから渡される移動方向
	Vector3				m_moveSpeed;				//移動速度。
	Vector3				m_scale = Vector3::One;		//大きさ。
	Quaternion			m_rotation;					//回転。
};

