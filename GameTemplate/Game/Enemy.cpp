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


    // --- HPバーの初期化 ---
    // 背景（Assets/Image/hp_back.dds などを用意してください）
    m_hpBarBackSprite.Init("Assets/sprite/white.DDS", 60.0f, 7.0f);
    // バー本体（Assets/Image/hp_bar.dds など）
    m_hpBarSprite.Init("Assets/sprite/Select.DDS", 60.0f, 7.0f);

    // 重要：ピボットを左端に設定（HPが減った時に左に縮むようにするため）
    m_hpBarSprite.SetPivot({ 0.0f, 0.5f });
    m_hpBarBackSprite.SetPivot({ 0.0f,0.5f });

    return true;
}

void Enemy::Update() {

    if (!m_player) return;
    if (m_enemyManager->IsActive() == false)return;

	//関数の呼び出し
	ManageState();
	AnimState();
    UpdateHPBar();
    
}

void Enemy::Move()
{

}

void Enemy::Rotation()
{
    

}

// 敵の状態を管理する関数
void Enemy::ManageState()
{
    Vector3 diff = m_player->m_position - m_position;
    diff.y = 0.0f;
    float distance = diff.Length();

    // --- 1. 死亡判定 (最優先) ---
    if (m_hp <= 0)
    {
        m_enemyState = 3; // 死亡
        if (m_modelRender.IsPlayingAnimation() == false) { // セミコロンを削除し、ブロックにする
            // ★重要：消える前にマネージャーの名簿から自分を消してもらう
            if (m_enemyManager) {
                m_enemyManager->OnEnemyDestroy(this);
                m_enemyManager->CountUpDeathCount();
            }
            DeleteGO(this);
            return; // 削除したのでこれ以降の処理はしない
        }
    }
    // --- 2. 攻撃中の継続判定 ---
    // 攻撃中の場合は、アニメーションが終わるまでステートを固定する
    else if (m_enemyState == 2)
    {
        // 攻撃アニメが終わったかチェック
        if (m_modelRender.IsPlayingAnimation() == false) {
            m_enemyState = 0;         // 待機に戻る
            m_hasDealtDamage = false; // フラグをリセット
        }
        else {
            // アニメが続いているなら攻撃関数を呼ぶ
            Attack();
        }
    }
    // --- 3. 通常のステート遷移 (待機・歩き・攻撃開始) ---
    else if (distance < 150.0f) {
        m_enemyState = 2;         // 攻撃開始
        m_hasDealtDamage = false; // 開始時にリセット
    }
    else if (distance < 1000.0f) {
        m_enemyState = 1;         // 追いかける

        diff.Normalize();
        Vector3 moveSpeed = diff * 150.0f;
        m_position = m_charaCon.Execute(moveSpeed, 1.0f / 60.0f);

        m_rot.SetRotationYFromDirectionXZ(diff);
        m_modelRender.SetRotation(m_rot);
		m_wanderPosSet = false; // プレイヤーを追いかけている間は徘徊地点をリセット
    }
    else {
        //m_enemyState = 0;         // 遠ければ待機
		m_attackTimer = 0.0f; // プレイヤーが遠ざかったら攻撃タイマーをリセット

        if (m_wanderPosSet == false) {
            //地点の設定
            m_wanderPos = m_position;
            m_wanderPosSet = true;
        }

        Wander();// 徘徊処理を呼ぶ
		
    }

    // --- 4. 共通の更新処理 ---
    Vector3 colPos = m_position;
    colPos.y += 50.0f;
    m_collisionObject->SetPosition(colPos);
    m_modelRender.SetPosition(m_position);
    m_modelRender.Update();
}

// アニメーションの再生
void Enemy::AnimState()
{
    switch (m_enemyState) {
    case 0: m_modelRender.PlayAnimation(m_enAnimClip_Idle); 
        break;
    case 1: m_modelRender.PlayAnimation(m_enAnimClip_Walk); 
        break;
    case 2: m_modelRender.PlayAnimation(m_enAnimClip_Attack); 
        break;
    case 3: m_modelRender.PlayAnimation(m_enAnimClip_Death); 
        break;
    }
}

// 敵の徘徊を管理する関数
void Enemy::Wander()
{

    if (!m_wanderPosSet) return; // 安全策

    m_enemyState = 1; // Walkアニメーション
    Vector3 moveSpeed = Vector3::Zero;

    // 向きに合わせて移動方向（速度）を決める
    if (m_enemymukiState == 0) {
        moveSpeed.z = 100.0f; // 前進
        m_rot.SetRotationDegY(0.0f);
    }
    else {
        moveSpeed.z = -100.0f; // 後退
        m_rot.SetRotationDegY(180.0f);
    }

    // 基準点から一定距離（500.0f）離れたら反転
    if (m_position.z >= m_wanderPos.z + 500.0f) {
        m_enemymukiState = 1;
    }
    else if (m_position.z <= m_wanderPos.z - 500.0f) {
        m_enemymukiState = 0;
    }

    // CharacterController を使って移動を適用（めり込み防止）
    m_position = m_charaCon.Execute(moveSpeed, g_gameTime->GetFrameDeltaTime());

    m_modelRender.SetRotation(m_rot);
}

// 攻撃判定用関数
void Enemy::Attack()
{
	//既に攻撃が当たっている場合は無視
	if (m_hasDealtDamage) return;

	//攻撃判定用のタイマー更新
	m_attackTimer += g_gameTime->GetFrameDeltaTime();

    //タイマーが一秒経過したら
    if (m_attackTimer >= 1.0f) {
        //プレイヤーとの距離を計算
        Vector3 diff = m_player->m_position - m_position;
        diff.y = 0.0f; // 水平方向の距離だけを考慮
        float distance = diff.Length();

        // 攻撃判定距離（150.0f〜170.0fくらいで調整）
        if (distance < 160.0f) {
            m_player->OnDamege(10); // プレイヤーにダメージを与える
            m_hasDealtDamage = true; // ダメージを与えたのでフラグを立てる
        }

        m_attackTimer = 0.0f; // タイマーをリセット
    }

}

// ダメージを受ける関数
void Enemy::OnDamage(int damage)
{
	if (m_hp <= 0) return; // すでに死んでいる場合は無視
	m_hp -= damage;
}

void Enemy::Render(RenderContext& rc) {
    m_modelRender.Draw(rc);
    m_hpBarBackSprite.Draw(rc);
	m_hpBarSprite.Draw(rc);
}

void Enemy::UpdateHPBar() {
    // --- HPバーの座標更新 ---

      // 1. 3D空間の頭上の位置を決める
    Vector3 worldPos = m_position;
    worldPos.y += 180.0f; // 敵のモデルに合わせて調整（15倍スケールならもう少し高くてもいいかも）

    // 2. カメラの関数を使ってスクリーン座標に変換
    Vector2 screenPos2D;
    g_camera3D->CalcScreenPositionFromWorldPosition(screenPos2D, worldPos);

    // 3. スプライトはVector3を想定しているので詰め替える (Zは0)
    Vector3 screenPos3D;
    screenPos3D.x = screenPos2D.x;
    screenPos3D.y = screenPos2D.y;
    screenPos3D.z = 0.0f;

    m_hpBarBackSprite.SetPosition(screenPos3D);
    m_hpBarSprite.SetPosition(screenPos3D);

    // 4. HP割合でスケール変更
    float hpRate = (float)m_hp / (float)m_hpMax;
    if (hpRate < 0.0f) hpRate = 0.0f;
    m_hpBarSprite.SetScale({ hpRate, 1.0f, 1.0f });

    // 5. 反映
    m_hpBarBackSprite.Update();
    m_hpBarSprite.Update();
}