#include "stdafx.h"
#include "Enemy.h"
#include "Player.h"

bool Enemy::Start() {
    // アニメーションのロード
    m_animationClips[m_enAnimClip_Idle].Load("Assets/Karimodel/SkeletonAnim/SkeletonIdle.tka");
    m_animationClips[m_enAnimClip_Idle].SetLoopFlag(true);
    m_animationClips[m_enAnimClip_Walk].Load("Assets/Karimodel/SkeletonAnim/SkeletonWalk.tka");
    m_animationClips[m_enAnimClip_Walk].SetLoopFlag(true);
    m_animationClips[m_enAnimClip_Attack].Load("Assets/Karimodel/SkeletonAnim/SkeletonAttack.tka");
    m_animationClips[m_enAnimClip_Attack].SetLoopFlag(false); // 攻撃は1回ずつ
    m_modelRender.Init("Assets/KariModel/Skeleton/Skeleton.tkm", m_animationClips, m_enAnimClip_Num); // 敵のモデル
	m_modelRender.SetScale({ 15.0f, 15.0f, 15.0f });
    m_position = { 500.0f, 0.0f, 500.0f }; // 初期位置

    // プレイヤーを探しておく
    m_player = FindGO<Player>("player");

    m_charaCon.Init(25.0f, 75.0f, m_position);
    return true;
}

void Enemy::Update() {
    if (!m_player) return;

    Vector3 diff = m_player->m_position - m_position;
    diff.y = 0.0f;
    float distance = diff.Length();

    // --- 挙動とステートの決定 ---
    if (distance < 150.0f) {
        // 1. 至近距離なら攻撃
        m_enemyState = 2;
    }
    else if (distance < 1000.0f) {
        // 2. ある程度近ければ追いかける（歩き）
        m_enemyState = 1;

        diff.Normalize();
        Vector3 moveSpeed = diff * 150.0f;
        m_position = m_charaCon.Execute(moveSpeed, 1.0f / 60.0f);

        Quaternion rot;
        rot.SetRotationYFromDirectionXZ(diff);
        m_modelRender.SetRotation(rot);
    }
    else {
        // 3. 遠ければ待機
        m_enemyState = 0;
    }

    // 攻撃中かつ、アニメーションが終了していたら待機に戻る
    if (m_enemyState == 2 && m_modelRender.IsPlayingAnimation() == false) {
        m_enemyState = 0;
    }

    // --- アニメーションの再生 ---
    switch (m_enemyState) {
    case 0: m_modelRender.PlayAnimation(m_enAnimClip_Idle); break;
    case 1: m_modelRender.PlayAnimation(m_enAnimClip_Walk); break;
    case 2: m_modelRender.PlayAnimation(m_enAnimClip_Attack); break;
    }

    m_modelRender.SetPosition(m_position);
    m_modelRender.Update();
}

void Enemy::Render(RenderContext& rc) {
    m_modelRender.Draw(rc);
}