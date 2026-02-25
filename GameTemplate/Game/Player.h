#pragma once
#include "sound/SoundSource.h"
class PlayerAttack;
class ItemSokudo;
class Collision;
class Player :public IGameObject
{
public:
	Player();
	~Player();

	bool Start() override;
	void InitEffect();
	void Update();
	void Move();
	void Rotetion();
	void Anime();
	void State();
	void Attack();
	void Render(RenderContext& rc);

	enum EnAnimationClip {
		m_enAnimClip_Idle,
		m_enAnimClip_Walk,
		m_enAnimClip_Jump,
		m_enAnimClip_Run,
		m_enAnimClip_Num,
	};

	Vector3 m_position = Vector3::Zero;
	Vector3 m_moveSpeed = Vector3::Zero;
	Vector3 m_StartMoveSpeed = { 0.0f,0.0f,0.0f };
	Vector3 colpos = Vector3::Zero;
	ModelRender m_modelRender;
	Quaternion m_rot = Quaternion::Identity;
	AnimationClip m_animationClips[m_enAnimClip_Num];

	int m_playerState = 0;
	int m_jumpState = 0;
	int m_playerHP = 300;
	int m_maxHP = m_playerHP;
	int m_playerATK = 1;

	float m_BaisokuCnt = 1.0f;//äOÇ©ÇÁë¨ìxUPÇ≥ÇπÇÈÇΩÇﬂÇÃïœêîÅB

	bool m_ballView = false;

	CharacterController characterController;

	CollisionObject* m_collisionObject = nullptr;
	PlayerAttack* m_plAtk = nullptr;

};

