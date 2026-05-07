#pragma once
#include"graphics/effect/EffectEmitter.h"

class Player;
class GameCamera;
class PlayerAttack :public IGameObject
{
public:
	//メンバ関数。
	PlayerAttack();
	~PlayerAttack();
	void Update();
	bool Start();
	//コリジョンの作成。
	void CreateCollision();
	//エフェクトの作成。
	void CreateEffect();

	//座標を取得する。
	const Vector3& GetPosition()const
	{
		return m_position;
	}

	//座標をセットする。
	void SetPosition(const Vector3& position)
	{
		m_position = position;
	}

	//回転をセットする。
	void SetRotation(const Quaternion& rotation)
	{
		m_rotation = rotation;
	}


private:
	//メンバ変数。
	Player* m_player;					//プレイヤー。
	GameCamera* m_gameCamera;				//カメラ。
	CollisionObject* m_collisionObj;				//コリジョンオブジェクト。
	EffectEmitter* m_effectEmitter;			//エフェクト
	const float			m_amuletSpeed = 600.0f;	//御札の移動速度。
	float				m_deleteTimer = 0.0f;		//削除を管理するタイマー。
	Vector3				m_position;					//座標。
	Vector3				m_direction;				//移動方向。
	Vector3				m_moveSpeed;				//移動速度。
	Vector3				m_scale = Vector3::One;		//大きさ。
	Quaternion			m_rotation;					//回転。
};

