#pragma once
#include "sound/SoundSource.h"
class PlayerAttack;
class ItemSokudo;
class Collision;
class InGameNomalState;
class InGameBossState; 
class Player :public IGameObject
{
public:
	Player();
	~Player();

	bool Start() override;
	void Update();
	void Move();
	void Rotetion();
	void Anime();
	void State();
	void Attack();
	void OnDamege(int damege);
	void Render(RenderContext& rc);
	// 親をセットする関数
	void SetParentState(InGameNomalState* parent) { m_parentState = parent; }
	void SetParentState(InGameBossState* state) { m_bossState = state; } // ★追加
	enum EnAnimationClip {
		m_enAnimClip_Idle,
		m_enAnimClip_Walk,
		m_enAnimClip_Jump,
		m_enAnimClip_Run,
		m_enAnimClip_Num,
	};

	Vector3 m_position = { 0.0f, 0.0f, -300.0f };
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
	int m_playerATK = 10;
	int m_playerSkillATK = 20;//プレイヤーのスキルダメージ
	int m_playerSkillGauge = 0;//プレイヤーのスキルゲージ

	bool m_ballView = false;

	CharacterController characterController;

	CollisionObject* m_collisionObject = nullptr;
	PlayerAttack* m_plAtk = nullptr;

	// プレイヤーの位置を取得する。PlayerAttack などから呼ばれている GetPosition と整合させるために追加。
	// 既存のメンバ変数名に合わせて内部参照を調整してください。
	const Vector3& GetPosition() const
	{
		return m_position;
	}

private:
	InGameNomalState* m_parentState = nullptr;
	InGameBossState* m_bossState = nullptr; // ★追加
};

