#pragma once

class Player;

enum EnAnimationClip {
    m_enAnimClip_Idle,   // 待機
    m_enAnimClip_Walk,   // 歩き
    m_enAnimClip_Attack, // 攻撃
	m_enAnimClip_Death,  // 死亡
    m_enAnimClip_Num,
};
class EnemyManager;
class Enemy : public IGameObject {
public:
    bool Start() override;
    void Update() override;
    void Render(RenderContext& rc) override;
    Vector3 m_position = Vector3::Zero;
    void OnDamage(int damage);

private:
    ModelRender m_modelRender;
    Player* m_player = nullptr; // 追いかける対象
    CharacterController m_charaCon;
	CollisionObject* m_collisionObject = nullptr;
    AnimationClip m_animationClips[m_enAnimClip_Num];
    int m_enemyState = EnAnimationClip::m_enAnimClip_Idle; // 0:待機, 1:移動, 2:攻撃
	int m_hp = 10;
	EnemyManager* m_enemyManager = nullptr; // 自分を管理している EnemyManager へのポインタ
};