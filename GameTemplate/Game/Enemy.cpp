#include "stdafx.h"
#include "Enemy.h"
#include "Player.h"
#include "EnemyManager.h"
#include "EnemyAttack.h"

enum EnMoveState {
    m_enMoveState_Idle,   // 待機
    m_enMoveState_Walk,   // 歩き
    m_enMoveState_Attack, // 攻撃
    m_enMoveState_Num,
};

namespace {
    // エフェクト番号。0はmagic_fire（PlayerAttack/EnemyAttackの弾）が使っているので1から振る。
    const int EFFECT_NO_DEATH = 1;  // 撃破時の爆散
    const int EFFECT_NO_HIT = 2;    // 被弾時のヒット

    const Vector3 DEATH_EFFECT_SCALE = { 40.0f, 40.0f, 40.0f };
    const Vector3 HIT_EFFECT_SCALE = { 15.0f, 15.0f, 15.0f };
    // エフェクトを敵の胴体あたりに出すための高さ。
    const float EFFECT_HEIGHT = 80.0f;
}

//コリジョンオブジェクトを消すためにデストラクタが必要
Enemy::~Enemy()
{
    if (m_collisionObject) {
        DeleteGO(m_collisionObject);
    }

}

bool Enemy::Start() {

    m_modelRender.Init("Assets/ModelData/Enemy/enemy.tkm"); // 敵のモデル
	m_modelRender.SetScale({ 1.0f, 1.0f, 1.0f });
	m_modelRender.SetPosition(m_position);


    // 2. 当たり判定用コリジョンの作成
    m_collisionObject = NewGO<CollisionObject>(0);
    // ボックス形状を作成 (サイズはモデルに合わせて調整してください)
    m_collisionObject->CreateBox(m_position, Quaternion::Identity, { 50.0f, 100.0f, 50.0f });
    // 攻撃側が探せるように名前を付ける
    m_collisionObject->SetName(this->m_name.c_str());
    // 自動消滅しないように設定
    m_collisionObject->SetIsEnableAutoDelete(false);

    // プレイヤーを探しておく
    m_player = FindGO<Player>("player");

    m_charaCon.Init(25.0f, 75.0f, m_position);

    m_enemyManager = FindGO<EnemyManager>("EnemyManager");

    // エフェクトの登録。中でマップ登録済みかを見ているので、敵ごとに呼んでも二重読み込みにはならない。
    EffectEngine::GetInstance()->ResistEffect(EFFECT_NO_DEATH, u"Assets/Karieffect/efk/magic_explo04.efk");
    EffectEngine::GetInstance()->ResistEffect(EFFECT_NO_HIT, u"Assets/Karieffect/efk/enemy_slash_01.efk");

    // --- HPバーの初期化 ---
    // 背景（Assets/Image/hp_back.dds などを用意してください）
    m_hpBarBackSprite.Init("Assets/UI/bar.DDS", 60.0f, 7.0f);
	m_hpBarBackSprite.SetMulColor({ 0.0f, 0.0f, 0.0f, 1.0f }); // 黒色にする
    // バー本体（Assets/Image/hp_bar.dds など）
    m_hpBarSprite.Init("Assets/UI/bar.DDS", 60.0f, 7.0f);
	m_hpBarSprite.SetMulColor({ 1.0f, 0.0f, 0.0f, 1.0f }); // 赤色にする

    // 重要：ピボットを左端に設定（HPが減った時に左に縮むようにするため）
    m_hpBarSprite.SetPivot({ 0.0f, 0.5f });
    m_hpBarBackSprite.SetPivot({ 0.0f,0.5f });

    return true;
}

void Enemy::Update() {

    if (!m_player) return;
    if (m_enemyManager->IsActive() == false)return;

	//関数の呼び出し
	Move();
	ManageState();
	MoveState();
    UpdateHPBar();
    
}

void Enemy::Move()
{
	//攻撃をするならばプレイヤーの方を向く
    if(m_enemyState == 2)
    {
        Vector3 diff = m_player->m_position - m_position;
        diff.y = 0.0f; // 水平方向のみにする
        diff.Normalize();
        m_rot.SetRotationYFromDirectionXZ(diff);
        m_modelRender.SetRotation(m_rot);
	}
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

    // 1. 死亡判定
    if (m_hp <= 0) {
        m_enemyState = 3;
        // 撃破の手応えを出すために爆散させる。
        PlayEffect(EFFECT_NO_DEATH, DEATH_EFFECT_SCALE, m_position + Vector3(0.0f, EFFECT_HEIGHT, 0.0f));
        if (m_enemyManager) {
            m_enemyManager->OnEnemyDestroy(this);
            m_enemyManager->CountUpDeathCount();
        }
        DeleteGO(this);
        return;
    }

    // 2. 攻撃ステートの管理
    if (m_enemyState == 2) {
        Attack(); // 内部でタイマーが進む

        // 2.0秒経過したらリセット
        if (m_attackTimer >= 2.0f) {
            m_enemyState = 0;         // これで次のUpdateで距離判定が行われる
            m_attackTimer = 0.0f;
            m_hasDealtDamage = false;
            m_attackRot = false;
        }
        // 攻撃ステートの間はここで処理を終了して、移動させない
        goto UpdatePosition;
    }

    // 3. 次の行動判定（攻撃ステート以外の時に来る）
    if (distance < 350.0f) {
        m_enemyState = 2; // 攻撃開始
    }
    else if (distance < 1000.0f) { // 追従範囲（1000だと遠すぎる場合があるので調整）
        m_enemyState = 1;
        diff.Normalize();
        Vector3 moveSpeed = diff * 150.0f;
        m_position = m_charaCon.Execute(moveSpeed, g_gameTime->GetFrameDeltaTime());

        m_rot.SetRotationYFromDirectionXZ(diff);
        m_modelRender.SetRotation(m_rot);
        m_wanderPosSet = false;
    }
    else {
        // 遠いので徘徊
        if (!m_wanderPosSet) {
            m_wanderPos = m_position;
            m_wanderPosSet = true;
        }
        Wander();
    }

UpdatePosition:
    // 共通の更新処理
    Vector3 colPos = m_position;
    colPos.y += 50.0f;
    m_collisionObject->SetPosition(colPos);
    m_modelRender.SetPosition(m_position);
    m_modelRender.Update();
}

void Enemy::MoveState()
{

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
    m_attackTimer += g_gameTime->GetFrameDeltaTime();

    if (m_hasDealtDamage) return;

    // --- STEP 1: 予兆 (0.0s ～ 1.0s) ---
    if (m_attackTimer < 1.0f) {
        if (!m_attackRot) {
            // プレイヤーへの方向を計算
            Vector3 diff = m_player->m_position - m_position;
            diff.y = 0.0f;
            diff.Normalize();

            // ★重要：この瞬間の方向を「狙い」として保存！
            m_attackDir = diff;

            m_rot.SetRotationYFromDirectionXZ(m_attackDir);
            m_modelRender.SetRotation(m_rot);
            m_attackRot = true;
        }
    }
    // --- STEP 2: 発射 (1.0s 経過時) ---
    else if (m_attackTimer >= 1.0f) {
        m_enemyAttack = NewGO<EnemyAttack>(0);

        // 出現位置：エネミーの座標 + (狙った方向に少し進める) + (高さ調整)
        // これで「固定値-100」ではなく、エネミーの正面から弾が出ます
        m_enemyAttack->m_position = m_position + (m_attackDir * 100.0f) + Vector3(0.0f, 100.0f, 0.0f);

        // ★重要：弾に「狙った方向」を渡す
        // EnemyAttack.h に public で Vector3 m_direction; を作っておくか、
        // SetDirection() のような関数を作って渡してください
        m_enemyAttack->m_direction = m_attackDir;

        m_enemyAttack->m_rotation = m_rot;

        m_hasDealtDamage = true;
    }
}

// ダメージを受ける関数
void Enemy::OnDamage(int damage)
{
	if (m_hp <= 0) return; // すでに死んでいる場合は無視
	m_hp -= damage;

	// 当たった感触を出す。死亡時は爆散エフェクトの方を出すので、ここでは出さない。
	if (m_hp > 0) {
		PlayEffect(EFFECT_NO_HIT, HIT_EFFECT_SCALE, m_position + Vector3(0.0f, EFFECT_HEIGHT, 0.0f));
	}
}

// 指定のエフェクトをその場所で再生する。
// EffectEmitterは再生が終わると自分でDeleteGOするので、ポインタを持たずに投げっぱなしにする。
void Enemy::PlayEffect(int effectNo, const Vector3& scale, const Vector3& pos)
{
	auto* effect = NewGO<EffectEmitter>(0);
	effect->Init(effectNo);
	effect->SetScale(scale);
	effect->SetPosition(pos);
	effect->Play();
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

	//HPバーの色をHP割合に応じて変化させる（例: 黄色 → 赤）
	Vector4 barColor;
    if (hpRate > 0.5f) {
        // HPが50%以上なら黄色
		barColor = { 1.0f, 1.0f, 0.0f, 1.0f };
    }
    else {
		// HPが50%以下なら赤
        barColor = { 1.0f, 0.0f, 0.0f, 1.0f };
	}

	m_hpBarSprite.SetMulColor(barColor);

    // 5. 反映
    m_hpBarBackSprite.Update();
    m_hpBarSprite.Update();
}

void Enemy::Render(RenderContext& rc) {
    m_modelRender.Draw(rc);
    m_hpBarBackSprite.Draw(rc);
	m_hpBarSprite.Draw(rc);
}

