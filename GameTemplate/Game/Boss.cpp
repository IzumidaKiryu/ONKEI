#include "stdafx.h"
#include "Boss.h"
#include "Player.h"
#include "EnemyManager.h"

bool Boss::Start()
{
    // 1. モデルとアニメーションのロード
    // ボスなので少し豪華に、あるいは既存モデルを大きく表示
    m_animationClips[enState_Idle].Load("Assets/Karimodel/SkeletonAnim/SkeletonIdle.tka");
    m_animationClips[enState_Idle].SetLoopFlag(true);
    m_animationClips[enState_Walk].Load("Assets/Karimodel/SkeletonAnim/SkeletonWalk.tka");
    m_animationClips[enState_Walk].SetLoopFlag(true);
    m_animationClips[enState_AttackMelee].Load("Assets/Karimodel/SkeletonAnim/SkeletonAttack.tka");
    m_animationClips[enState_AttackMelee].SetLoopFlag(false);
	m_animationClips[enState_AttackRange].Load("Assets/Karimodel/SkeletonAnim/SkeletonAttack.tka");//変える
	m_animationClips[enState_AttackRange].SetLoopFlag(false);
    m_animationClips[enState_Death].Load("Assets/Karimodel/SkeletonAnim/SkeletonDeath.tka");
    m_animationClips[enState_Death].SetLoopFlag(false);

    m_modelRender.Init("Assets/KariModel/Skeleton/Skeleton.tkm", m_animationClips, enState_Death + 1);
    m_modelRender.SetScale({ 40.0f, 40.0f, 40.0f }); // ザコより大きく設定

    // 2. HPバー（UI）の初期化
    // 画面下部に固定するため、テクスチャは白い画像を用意して色を付ける
    m_hpBarBossBack.Init("Assets/sprite/white.dds", 1000.0f, 40.0f);
    m_hpBarBoss.Init("Assets/UI/bar.dds", 1000.0f, 40.0f);

    m_hpBarBossBack.SetMulColor({ 0.2f, 0.2f, 0.2f, 0.8f }); // 暗いグレー（背景）
    m_hpBarBoss.SetMulColor({ 0.8f, 0.0f, 0.0f, 1.0f });     // 鮮やかな赤（体力）

    // UIなので中心ではなく左端から縮むようにピボットを設定
    m_hpBarBoss.SetPivot({ 0.0f, 0.5f });
    m_hpBarBossBack.SetPivot({ 0.0f, 0.5f });

    // 画面下部に配置（座標系に合わせて調整してください）
    // CalcScreenPositionを使わず、直接スクリーン座標を指定
    m_hpBarBossBack.SetPosition({ -500.0f, 400.0f, 0.0f });
    m_hpBarBoss.SetPosition({ -500.0f, 400.0f, 0.0f });

    // 3. 物理・プレイヤー取得
    m_player = FindGO<Player>("player");
    m_charaCon.Init(80.0f, 200.0f, m_position); // 当たり判定も大きく

    //  当たり判定用コリジョンの作成
    m_collisionObject = NewGO<CollisionObject>(0);
    // ボックス形状を作成 (サイズはモデルに合わせて調整してください)
    m_collisionObject->CreateBox(m_position, Quaternion::Identity, { 50.0f, 100.0f, 50.0f });
    // 攻撃側が探せるように名前を付ける
    m_collisionObject->SetName("boss");
    // 自動消滅しないように設定
    m_collisionObject->SetIsEnableAutoDelete(false);

	m_enemyManager = FindGO<EnemyManager>("EnemyManager");
    return true;
}

void Boss::Update()
{
    if (!m_player) return;
    if (m_enemyManager->IsActive() == false)return;

    ManageState();
    AnimState();
    UpdateHPBar();

    // 座標の反映
    m_modelRender.SetPosition(m_position);
    m_modelRender.SetRotation(m_rot);
    m_modelRender.Update();
    Vector3 colPos = m_position;
    colPos.y += 50.0f;
    m_collisionObject->SetPosition(colPos);
}

void Boss::ManageState()
{
    // 死亡時
    if (m_hp <= 0) {
        m_bossState = enState_Death;
        if (!m_modelRender.IsPlayingAnimation()) {
			m_enemyManager->SetBossDeathFlag(true); // ボスが死んだことを EnemyManager に伝える
            DeleteGO(this);
        }
        return;
    }

    // 攻撃中の硬直
    if (m_bossState == enState_AttackMelee) {
        if (!m_modelRender.IsPlayingAnimation()) {
            m_bossState = enState_Idle;
            m_hasDealtDamage = false;
        }
        else {
            AttackMelee(); // 攻撃判定チェック
        }
        return;
    }

    // プレイヤーとの距離計算
    Vector3 diff = m_player->m_position - m_position;
    diff.y = 0.0f;
    float dist = diff.Length();

    if (dist < 250.0f) {
        // 近ければ攻撃
        m_bossState = enState_AttackMelee;
    }
    else if (dist < 2000.0f) {
        // 追跡
        m_bossState = enState_Walk;
        diff.Normalize();

        // 重要: 一時オブジェクトを非 const 参照に渡してはいけないため、名前付きの lvalue に格納する
        Vector3 move = diff * 200.0f; // move は lvalue
        m_position = m_charaCon.Execute(move, g_gameTime->GetFrameDeltaTime());

        m_rot.SetRotationYFromDirectionXZ(diff);
    }
    else {
        m_bossState = enState_Idle;
    }
}

void Boss::UpdateHPBar()
{
    // HP割合の計算
    float hpRate = (float)m_hp / (float)m_hpMax;
    if (hpRate < 0.0f) hpRate = 0.0f;

    // バーの長さを更新
    m_hpBarBoss.SetScale({ hpRate, 1.0f, 1.0f });

    // SpriteRenderのUpdateを忘れずに
    m_hpBarBoss.Update();
    m_hpBarBossBack.Update();
}

void Boss::AttackMelee()
{
    if (m_hasDealtDamage) return;

    // アニメーションが再生中なら判定を行う
    if (m_modelRender.IsPlayingAnimation()) {
        Vector3 diff = m_player->m_position - m_position;
        // 距離が近ければダメージ
        if (diff.Length() < 300.0f) {
            m_player->OnDamege(30);
            m_hasDealtDamage = true; // 1回当たったらこの攻撃中はもう当たらない
        }
    }
}

void Boss::AttackRange()
{
}

void Boss::OnDamage(int damage)
{
    if (m_hp <= 0) return;
    m_hp -= damage;
}

void Boss::Render(RenderContext& rc)
{
    m_modelRender.Draw(rc);

    // ボスHPバーは常に表示（または戦闘開始後から表示）
    m_hpBarBossBack.Draw(rc);
    m_hpBarBoss.Draw(rc);
}

void Boss::AnimState()
{
    // 現在のボスの状態（m_bossState）に合わせてアニメーションを切り替える
    switch (m_bossState)
    {
    case enState_Idle:
        m_modelRender.PlayAnimation(enState_Idle);
        break;
    case enState_Walk:
        m_modelRender.PlayAnimation(enState_Walk);
        break;
    case enState_AttackMelee:
        m_modelRender.PlayAnimation(enState_AttackMelee);
        break;
    case enState_AttackRange:
        m_modelRender.PlayAnimation(enState_AttackRange);
        break;
    case enState_Death:
        m_modelRender.PlayAnimation(enState_Death);
        break;
    }
}