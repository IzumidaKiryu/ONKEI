#pragma once
#include "sound/SoundSource.h"
class Player;

class PlayerAttack :public IGameObject
{
public:
	PlayerAttack();
	~PlayerAttack();

	bool Start();

	void Update();

	Player* m_Player = nullptr;

	void SetPosition(const Vector3& position)
	{
		m_position = position;
	}

	void SetRotation(const Quaternion& rotation)
	{
		m_rotation = rotation;
	}

	void SetScale(const Vector3& scale)
	{
		m_scale = scale;
	}


	SoundSource* m_bgm = nullptr;

	Player* m_player = nullptr;


private:
	Vector3					m_position = Vector3::Zero;			//座標。
	Quaternion				m_rotation = Quaternion::Identity;	//回転。
	Vector3					m_scale = Vector3::One;				//大きさ。
	Vector3					m_moveSpeed = Vector3::Zero;		//移動速度。
	EffectEmitter* m_effectEmitter = nullptr;			        //エフェクト。
	float					m_timer = 0.0f;						//タイマー。
	CollisionObject* m_coll = nullptr;
};

