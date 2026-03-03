#include "stdafx.h"
#include "Enemy.h"
#include "Player.h"
#include "EnemyManager.h"

bool Enemy::Start() {
    // 1. 自分自身に名前を付ける (FindGO で見つけられるようにするため)
    SetName("enemy");
    // アニメーションのロード
    m_animationClips[m_enAnimClip_Idle].Load("Assets/Karimodel/SkeletonAnim/SkeletonIdle.tka");
    m_animationClips[m_enAnimClip_Idle].SetLoopFlag(true);
    m_animationClips[m_enAnimClip_Walk].Load("Assets/Karimodel/SkeletonAnim/SkeletonWalk.tka");
    m_animationClips[m_enAnimClip_Walk].SetLoopFlag(true);
    m_animationClips[m_enAnimClip_Attack].Load("Assets/Karimodel/SkeletonAnim/SkeletonAttack.tka");
    m_animationClips[m_enAnimClip_Attack].SetLoopFlag(false); // 攻撃は1回ずつ
    m_animationClips[m_enAnimClip_Death].Load("Assets/Karimodel/SkeletonAnim/SkeletonDeath.tka");
    m_animationClips[m_enAnimClip_Death].SetLoopFlag(false); 
    m_modelRender.Init("Assets/KariModel/Skeleton/Skeleton.tkm", m_animationClips, m_enAnimClip_Num); // 敵のモデル
	m_modelRender.SetScale({ 15.0f, 15.0f, 15.0f });
    m_position = { 500.0f, 0.0f, 500.0f }; // 初期位置

    // 2. 当たり判定用コリジョンの作成
    m_collisionObject = NewGO<CollisionObject>(0);
    // ボックス形状を作成 (サイズはモデルに合わせて調整してください)
    m_collisionObject->CreateBox(m_position, Quaternion::Identity, { 50.0f, 100.0f, 50.0f });
    // 攻撃側が探せるように名前を付ける
    m_collisionObject->SetName("enemy");
    // 自動消滅しないように設定
    m_collisionObject->SetIsEnableAutoDelete(false);

    // プレイヤーを探しておく
    m_player = FindGO<Player>("player");

    m_charaCon.Init(25.0f, 75.0f, m_position);

    m_enemyManager = FindGO<EnemyManager>("EnemyManager");
    return true;
}

void Enemy::Update() {
    if (!m_player) return;

    Vector3 diff = m_player->m_position - m_position;
    diff.y = 0.0f;
    float distance = diff.Length();

    // --- 挙動とステートの決定 ---
	if (m_hp <= 0)
    {
        m_enemyState = 3; // 死亡
        if (m_modelRender.IsPlayingAnimation() == false);
		m_enemyManager->CountUpDeathCount();
		DeleteGO(this);
    }

    else if (distance < 150.0f) {
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
	case 3: m_modelRender.PlayAnimation(m_enAnimClip_Death); break;
    }
    // 3. コリジョンの位置をモデルに合わせる
    Vector3 colPos = m_position;
    colPos.y += 50.0f; // 腰の高さくらいに調整
    m_collisionObject->SetPosition(colPos);
    m_modelRender.SetPosition(m_position);
    m_modelRender.Update();
}

void Enemy::Render(RenderContext& rc) {
    m_modelRender.Draw(rc);
}

void Enemy::OnDamage(int damage)
{
	if (m_hp <= 0) return; // すでに死んでいる場合は無視
	m_hp -= damage;
}
