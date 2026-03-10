#pragma once

class EnemyManager;
class Player;

class Enemy : public IGameObject {
public:
    bool Start() override;
    void Update() override;
	void Move();// “G‚ÌˆÚ“®‚ğŠÇ—‚·‚éŠÖ”
	void Rotation();// “G‚Ì‰ñ“]‚ğŠÇ—‚·‚éŠÖ”
	void ManageState();// “G‚Ìó‘Ô‚ğŠÇ—‚·‚éŠÖ”
	void AnimState();// ƒAƒjƒ[ƒVƒ‡ƒ“‚Ìó‘Ô‚ğŠÇ—‚·‚éŠÖ”
	void Wander();// “G‚Ìœpœj‚ğŠÇ—‚·‚éŠÖ”
	void Attack();// UŒ‚‚·‚éŠÖ”
	void OnDamage(int damage);// ƒ_ƒ[ƒW‚ğó‚¯‚éŠÖ”
    void Render(RenderContext& rc) override;
    
	enum EnAnimationClip {
		m_enAnimClip_Idle,   // ‘Ò‹@
		m_enAnimClip_Walk,   // •à‚«
		m_enAnimClip_Attack, // UŒ‚
		m_enAnimClip_Death,  // €–S
		m_enAnimClip_Num,
	};

	Vector3 m_position = Vector3::Zero;// “G‚ÌÀ•W
	Quaternion m_rot = Quaternion::Identity;// “G‚Ì‰ñ“]

	int m_hpMax = 10;// “G‚ÌÅ‘åHP
	int m_hp = m_hpMax;// “G‚ÌŒ»İ‚ÌHP

private:
    ModelRender m_modelRender;
    EnemyManager* m_enemyManager = nullptr; // ©•ª‚ğŠÇ—‚µ‚Ä‚¢‚é EnemyManager ‚Ö‚Ìƒ|ƒCƒ“ƒ^
    Player* m_player = nullptr; // ’Ç‚¢‚©‚¯‚é‘ÎÛ
    CharacterController m_charaCon;
	CollisionObject* m_collisionObject = nullptr;
    AnimationClip m_animationClips[m_enAnimClip_Num];

    int m_enemyState = EnAnimationClip::m_enAnimClip_Idle; // 0:‘Ò‹@, 1:ˆÚ“®, 2:UŒ‚
	float m_attackTimer = 0.0f; //UŒ‚”»’è—p‚Ìƒ^ƒCƒ}[

	bool m_hasDealtDamage = false;// UŒ‚‚ª“–‚½‚Á‚½‚©‚Ç‚¤‚©‚ğŠÇ—‚·‚éƒtƒ‰ƒO
	
};