#include "stdafx.h"
#include "PlayerAttack.h"
#include "Player.h"
#include "collision/CollisionObject.h"
#include "GameCamera.h"
#include "Enemy.h"

namespace
{
	//自動削除時間。
	const float DELETE_TIME = 0.28f;
	const Vector3 BOX_SCALE = { 100.0f,100.0f,100.0f };
	const Vector3 EFFECT_SCALE = { 55.0f,55.0f,55.0f };
}

bool PlayerAttack::Start()
{
	//カメラとプレイヤーのインスタンスを取得する。
	m_gameCamera = FindGO<GameCamera>("gamecamera");
	m_player = FindGO<Player>("player");

	//エフェクトの登録。
	EffectEngine::GetInstance()->ResistEffect(0, u"Assets/Karieffect/efk/magic_fire.efk");

	//カメラの前方向のベクトルを取得して、移動方向にする。
// --- 修正箇所：プレイヤーの回転から前方向を取得 ---
	// m_player->m_rot (Quaternion) から正面ベクトル(Forward)を取り出す
	// --- 修正箇所：Applyメソッドを使って正面方向を計算 ---
	// 1. Z方向（正面）の単位ベクトルを用意
	m_direction = Vector3::Front;

	// 2. プレイヤーの回転（m_rot）をそのベクトルに適用する
	// これで m_direction が「プレイヤーの正面を向いたベクトル」になります
	m_player->m_rot.Apply(m_direction);

	m_direction.Normalize();
	//プレイヤーの座標を持ってくる。
	m_position = m_player->GetPosition();

	//移動速度の設定。
	m_moveSpeed = m_direction * m_amuletSpeed;

	//コリジョンの作成。
	CreateCollision();

	//エフェクトの作成。
	CreateEffect();

	return true;
}

PlayerAttack::PlayerAttack()
{

}

PlayerAttack::~PlayerAttack()
{
	DeleteGO(m_collisionObj);
	DeleteGO(m_effectEmitter);
}

void PlayerAttack::Update()
{
	m_position += m_moveSpeed * g_gameTime->GetFrameDeltaTime() * 4.0f;

	if (m_effectEmitter->GetEffect() != nullptr)
	{
		m_effectEmitter->SetPosition(m_position);
	}

	m_collisionObj->SetPosition(m_position);



	//自動削除タイマー。
	m_deleteTimer += g_gameTime->GetFrameDeltaTime();
	//時間経過で削除する。
	if (m_deleteTimer >= DELETE_TIME)
	{
		//エフェクトの停止。
		m_effectEmitter->Stop();
		DeleteGO(this);
	}
}

void PlayerAttack::CreateCollision()
{
	//カメラ位置の取得。
	Vector3 cameraPosition = g_camera3D->GetPosition();
	//コリジョンオブジェクトの作成。
	m_collisionObj = NewGO<CollisionObject>(0);
	//ボックス上のコリジョンを作成。
	m_collisionObj->CreateBox(m_position, Quaternion::Identity, BOX_SCALE);
	//名前をつける。
	m_collisionObj->SetName("amulet");
	//自動削除されないようにする。
	m_collisionObj->SetIsEnableAutoDelete(false);
}

void PlayerAttack::CreateEffect()
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