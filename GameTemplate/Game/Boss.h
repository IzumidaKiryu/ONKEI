#pragma once
#include "Enemy.h" // 共通の構造があるなら継承もアリですが、今回は独立して作ります

class Player;
class EnemyManager;

class Boss : public IGameObject {
public:
    bool Start() override;
    void Update() override;
    void Render(RenderContext& rc) override;

    // ダメージ処理（ザコより多めにノックバックさせるなど工夫できる）
    void OnDamage(int damage);
    void SetPosition(Vector3 pos) { m_position = pos; }

private:
    void ManageState();  // 思考ルーチン
    void AnimState();    // アニメーション制御
    void UpdateHPBar();  // ボス専用HPバー（画面下部など）

    // 攻撃バリエーション
    void AttackMelee();  // 近接攻撃
    void AttackRange();  // 遠距離攻撃（火炎放射や衝撃波など）

private:
    ModelRender         m_modelRender;
    CharacterController m_charaCon;
    CollisionObject* m_collisionObject = nullptr;

    Player* m_player = nullptr;
    Quaternion          m_rot = Quaternion::Identity;
    Vector3             m_position;

    // ステータス
    int m_hpMax = 500;  // ボスなので多め
    int m_hp = m_hpMax;

    // ボス専用UI（画面下部の長いHPバーなど）
    SpriteRender m_hpBarBoss;
    SpriteRender m_hpBarBossBack;

    enum EnAnimationClip {
        m_enAnimClip_Idle,   // 待機
        m_enAnimClip_Walk,   // 歩き
        m_enAnimClip_Attack, // 攻撃
		m_enAnimClip_AttackRange, // ダメージ
        m_enAnimClip_Death,  // 死亡
        m_enAnimClip_Num,
    };

    // ボスの状態定義
    enum EnBossState {
        enState_Idle,
        enState_Walk,
        enState_AttackMelee, // 近接
        enState_AttackRange, // 遠距離
        enState_Death
    };
    EnBossState m_bossState = enState_Idle;

    float m_attackTimer = 0.0f;
    bool  m_hasDealtDamage = false;
    AnimationClip m_animationClips[m_enAnimClip_Num];
    EnemyManager* m_enemyManager = nullptr; // 自分を管理している EnemyManager へのポインタ

};