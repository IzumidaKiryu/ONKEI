#include "stdafx.h"
#include "EnemyAttack.h"
#include "Player.h"
#include "Enemy.h"

namespace
{
    //自動削除時間。
    const float DELETE_TIME = 0.5f;
    const Vector3 BOX_SCALE = { 50.0f, 50.0f, 50.0f };
    const Vector3 EFFECT_SCALE = { 10.0f,10.0f,10.0f };
}

EnemyAttack::EnemyAttack()
{

}

EnemyAttack::~EnemyAttack()
{
    if (m_collisionObj) {
        DeleteGO(m_collisionObj);
    }
    if (m_effectEmitter) {
        DeleteGO(m_effectEmitter);
    }
}

bool EnemyAttack::Start()
{
	m_player = FindGO<Player>("player"); // 攻撃の対象であるプレイヤーを探す
	//m_enemy = FindGO<Enemy>("enemy"); // 攻撃の所有者である敵を探す

    //エフェクトの登録。
    EffectEngine::GetInstance()->ResistEffect(0, u"Assets/Karieffect/efk/magic_fire.efk");

    // 1. Z方向（正面）の単位ベクトルを用意
    //m_direction = Vector3::Front;

    //// 2. エネミーの回転（m_rot）をそのベクトルに適用する
    //// これで m_direction が「エネミーの正面を向いたベクトル」になります
    //m_enemy->m_rot.Apply(m_direction);

    //m_direction.Normalize();
    // 2. ★重要：御札自体の回転もエネミーと同じにする
    //m_rotation = m_enemy->m_rot;

    //m_position = m_enemy->m_position + Vector3(0.0f, 0.0f, 50.0f);

    // 3. 移動速度を「方向×スピード」で固定
    //m_moveSpeed = m_direction * m_amuletSpeed;

    //m_position.y += 50.0f; // 高さの微調整だけここでやる
    //m_moveSpeed = m_direction * m_amuletSpeed;

    //コリジョンの作成。
    CreateCollision();

    //エフェクトの作成。
    CreateEffect();

    // 例: コリジョンオブジェクトの作成、エフェクトの準備など
    return true; // 初期化が完了したらtrueを返す
}

void EnemyAttack::Update()
{
    // エネミーから受け取った方向に向かって進み続ける（追従しない）
    m_position += m_direction * m_amuletSpeed * g_gameTime->GetFrameDeltaTime();

    // 移動：単純に「現在の座標 + (速度 * 時間)」にする
    m_position += m_moveSpeed * g_gameTime->GetFrameDeltaTime() * 4.0f;

    if (m_effectEmitter->GetEffect() != nullptr)
    {
        m_effectEmitter->SetPosition(m_position);
        // ★エフェクトの向きもプレイヤーと同じ方向に向ける
        m_effectEmitter->SetRotation(m_rotation);
    }

    m_collisionObj->SetPosition(m_position);
    // コリジョンも回転を合わせる（必要であれば）
    m_collisionObj->SetRotation(m_rotation);

	//プレイヤーとの当たり判定を行う
	const auto& playerPos = g_collisionObjectManager->FindCollisionObjects("player_col");
    for (auto playerCol : playerPos) {
        if (m_collisionObj->IsHit(playerCol)) {
            // ダメージを与える
            m_player->OnDamege(20.0f);

            // 御札を消す
            m_effectEmitter->Stop();
            DeleteGO(this);
            return; // ★自分を消したので、この後の処理（タイマー等）は行わずに終了
        }
    }

    // 自動削除タイマー。
    m_deleteTimer += g_gameTime->GetFrameDeltaTime();
    // 時間経過で削除する。
    if (m_deleteTimer >= DELETE_TIME)
    {
        // エフェクトの停止。
        m_effectEmitter->Stop();
        DeleteGO(this);
        return; // 安全のため return
    }
}

// コリジョンの作成
void EnemyAttack::CreateCollision()
{
    //コリジョンオブジェクトの作成。
    m_collisionObj = NewGO<CollisionObject>(0);
    //ボックス上のコリジョンを作成。
    m_collisionObj->CreateBox(m_position, Quaternion::Identity, BOX_SCALE);
    //名前をつける。
    m_collisionObj->SetName("enemy_attack");
    //自動削除されないようにする。
    m_collisionObj->SetIsEnableAutoDelete(false);
}

// エフェクトの作成
void EnemyAttack::CreateEffect()
{
    //エフェクトエミッターのインスタンスを作成。
    m_effectEmitter = NewGO<EffectEmitter>(0);
    m_effectEmitter->Init(0);
    //エフェクトのサイズを設定する。
    m_effectEmitter->SetScale(EFFECT_SCALE);
    //初期座標を設定する。
    m_effectEmitter->SetPosition(m_position);
    //エフェクトを再生。
    m_effectEmitter->Play();
}

void EnemyAttack::Render(RenderContext& rc)
{
    // 攻撃の描画処理をここに書く
    
    // 例: エフェクトの描画など

}