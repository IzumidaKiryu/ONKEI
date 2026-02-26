#pragma once

class Player;

enum EnAnimationClip {
    m_enAnimClip_Idle,   // ‘Ò‹@
    m_enAnimClip_Walk,   // •à‚«
    m_enAnimClip_Attack, // UŒ‚
	m_enAnimClip_Death,  // €–S
    m_enAnimClip_Num,
};
class Enemy : public IGameObject {
public:
    bool Start() override;
    void Update() override;
    void Render(RenderContext& rc) override;

    void OnDamage(int damage);

private:
    ModelRender m_modelRender;
    Vector3 m_position = Vector3::Zero;
    Player* m_player = nullptr; // ’Ç‚¢‚©‚¯‚é‘ÎÛ
    CharacterController m_charaCon;
	CollisionObject* m_collisionObject = nullptr;
    AnimationClip m_animationClips[m_enAnimClip_Num];
    int m_enemyState = EnAnimationClip::m_enAnimClip_Idle; // 0:‘Ò‹@, 1:ˆÚ“®, 2:UŒ‚
	int m_hp = 10;

};