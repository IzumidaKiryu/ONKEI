#include "stdafx.h"
#include "Player.h"


#include "graphics/effect/EffectEmitter.h"
#include "collision/CollisionObject.h"
#include "sound/SoundEngine.h"


Player::Player() {
}

Player::~Player() {

}

bool Player::Start()
{
	//アニメーションクリップをロードする。
	m_animationClips[m_enAnimClip_Idle].Load("Assets/animData/idle.tka");
	m_animationClips[m_enAnimClip_Idle].SetLoopFlag(true);
	m_animationClips[m_enAnimClip_Walk].Load("Assets/animData/walk.tka");
	m_animationClips[m_enAnimClip_Walk].SetLoopFlag(true);
	m_animationClips[m_enAnimClip_Jump].Load("Assets/animData/jump.tka");
	m_animationClips[m_enAnimClip_Jump].SetLoopFlag(false);
	m_animationClips[m_enAnimClip_Run].Load("Assets/animData/run.tka");
	m_animationClips[m_enAnimClip_Run].SetLoopFlag(true);
	m_modelRender.Init("Assets/modelData/unityChan.tkm", m_animationClips, m_enAnimClip_Num, enModelUpAxisY);
	m_modelRender.SetPosition(Vector3(0.0f, 0.0f, 0.0f));

	characterController.Init(25.0f, 75.0f, m_position);

	
	m_modelRender.Update();
	return true;
}

void Player::Update() {
	Move();
	Rotetion();
	State();
	Anime();
	Attack();


	m_modelRender.Update();
}

void Player::Move() {
	m_moveSpeed.x = m_StartMoveSpeed.x;
	m_moveSpeed.z = m_StartMoveSpeed.z;
	//左スティックの入力量を取得。
	Vector3 stickL;
	stickL.x = g_pad[0]->GetLStickXF();
	stickL.y = g_pad[0]->GetLStickYF();

	//カメラの前方向と右方向のベクトルを持ってくる。
	Vector3 forward = g_camera3D->GetForward();
	Vector3 right = g_camera3D->GetRight();
	//y方向には移動させない。
	forward.y = 0.0f;
	right.y = 0.0f;

	//左スティックの入力量と300.0fを乗算。
	right *= stickL.x * 300.0f;
	forward *= stickL.y * 300.0f;

	//移動速度にスティックの入力量を加算する&加速処理。
	m_moveSpeed += (right + forward) *= m_BaisokuCnt;
	if (g_pad[0]->IsPress(enButtonB)) {
		m_moveSpeed.x *= 3.0f;
		m_moveSpeed.z *= 3.0f;
	}
	//地面に付いていたら。
	if (characterController.IsOnGround())
	{
		//重力を無くす。
		m_moveSpeed.y = 0.0f;
		m_jumpState = 0;
		//Aボタンが押されたら。
		if (g_pad[0]->IsTrigger(enButtonA))
		{
			//ジャンプさせる。
			m_moveSpeed.y += 200.0f;
		}
	}
	//地面に付いていなかったら。
	else
	{
		//Aボタンが押されたら。
		if (g_pad[0]->IsTrigger(enButtonA) && m_jumpState == 0)
		{
			//ジャンプさせる。
			m_moveSpeed.y += 200.0f;
			m_jumpState = 1;
		}
		//重力を発生させる。
		m_moveSpeed.y -= 5.0f;
	}
	//キャラクターコントローラーを使って座標を移動させる。
	m_position = characterController.Execute(m_moveSpeed, 1.0f / 60.0f);
	//絵描きさんに座標を教える。
	m_modelRender.SetPosition(m_position);

}

void Player::Rotetion() {
	//xかzの移動速度があったら(スティックの入力があったら)。
	if (fabsf(m_moveSpeed.x) >= 0.001f || fabsf(m_moveSpeed.z) >= 0.001f)
	{
		//キャラクターの方向を変える。
		m_rot.SetRotationYFromDirectionXZ(m_moveSpeed);
		//絵描きさんに回転を教える。
		m_modelRender.SetRotation(m_rot);
	}

}

void Player::State() {
	if (characterController.IsOnGround() == false) {
		m_playerState = 1;
		return;
	}
	if (fabsf(m_moveSpeed.x) >= 0.001f || fabsf(m_moveSpeed.z) >= 0.001f) {
		if (g_pad[0]->IsPress(enButtonB)) {
			m_playerState = 3;
		}
		else {
			m_playerState = 2;
		}
	}

	else {
		m_playerState = 0;
	}
}

void Player::Anime() {

	//switch文。
	switch (m_playerState) {
		//プレイヤーステートが0(待機)だったら。
	case 0:
		//待機アニメーションを再生する。
		m_modelRender.PlayAnimation(m_enAnimClip_Idle);
		break;
		//プレイヤーステートが1(ジャンプ中)だったら。
	case 1:
		//ジャンプアニメーションを再生する。
		m_modelRender.PlayAnimation(m_enAnimClip_Jump);
		break;
		//プレイヤーステートが2(歩き)だったら。
	case 2:
		//歩きアニメーションを再生する。
		m_modelRender.PlayAnimation(m_enAnimClip_Walk);
		break;
	case 3:
		m_modelRender.PlayAnimation(m_enAnimClip_Run);
		break;
	}

}

void Player::Attack() {
	if (g_pad[0]->IsTrigger(enButtonSelect) && m_ballView == false) {
	
	}
}

void Player::Render(RenderContext& rc) {
	m_modelRender.Draw(rc);
}