#include "stdafx.h"
#include "PlayerUI.h"
#include "Player.h"

PlayerUI::PlayerUI()
{

}

PlayerUI::~PlayerUI()
{

}

bool PlayerUI::Start()
{
    // --- 1. 背景バー（ダメージを受けた後の残りの枠）の初期化 ---
    m_hpBarBack.Init("Assets/sprite/white.DDS", m_hpBarWidth, m_hpBarHeight);
    m_hpBarBack.SetMulColor({ 0.2f, 0.2f, 0.2f, 1.0f }); // 暗いグレー
    m_hpBarBack.SetPivot({ 0.0f, 0.5f }); // 左端を起点にする

    // --- 2. 前景バー（現在のHP）の初期化 ---
    m_hpBarFront.Init("Assets/UI/bar.DDS", m_hpBarWidth, m_hpBarHeight);
    m_hpBarFront.SetMulColor({ 0.0f, 1.0f, 0.0f, 1.0f }); // 緑色
    m_hpBarFront.SetPivot({ 0.0f, 0.5f }); // 左端を起点にする

	// --- 3. キャラアイコンの初期化 ---
	m_charaIcon.Init("Assets/UI/charaIcon.DDS", 1920, 1080);
	m_charaIcon.SetPosition({ -700.0f, -300.0f, 0.0f }); // HPバーの左端あたりに配置
	m_charaIcon.SetScale({ 0.25f, 0.25f, 1.0f }); // アイコンを小さくする

    //ボタンUIの初期化
    m_buttonA.Init("Assets/UI/buttonA.DDS", 192, 108);
    m_buttonA.SetPosition({ 670.0f,-380.0f,0.0f });
    m_buttonA.Update();

    m_buttonB.Init("Assets/UI/buttonB.DDS", 192, 108);
    m_buttonB.SetPosition({ 440.0f,-380.0f,0.0f });
    m_buttonB.Update();

	// --- 4. 攻撃ゲージの初期化 ---
	m_AttackGauge = new nsK2Engine::UIGaugeArc();
	m_AttackGauge->Init("Assets/UI/action.DDS", 600, 300, "Assets/shader/SkillGauge.fx");
    m_AttackGauge->SetPosition(Vector3(600.0f, -150.0f, 0.0f));

    // プレイヤーの参照を取得
    m_player = FindGO<Player>("player");

    return true;
}

void PlayerUI::Update()
{
    if (m_player) {
        // プレイヤーから最新のHPを取得（PlayerクラスにHP変数がある前提）
        m_currentHP = static_cast<float>(m_player->m_playerHP);
        m_maxHP = static_cast<float>(m_player->m_maxHP);
    }

    // --- 3. HPの割合に応じてゲージの長さを変える ---
    float hpRate = m_currentHP / m_maxHP;
    if (hpRate < 0.0f) hpRate = 0.0f;

    // 前景バーのスケールをX方向（横幅）だけ変更する
    m_hpBarFront.SetScale({ hpRate, 1.0f, 1.0f });

    // 画面左上に配置
    Vector3 uiPos = { -650.0f, -350.0f, 0.0f };
    m_hpBarBack.SetPosition(uiPos);
    m_hpBarFront.SetPosition(uiPos);

	//ゲージの色をHPの割合に応じて変える（緑→オレンジ→赤）
	if (hpRate > 0.5f) {
		m_hpBarFront.SetMulColor({ 0.0f, 1.0f, 0.0f, 1.0f }); // 緑色
	}
	else if (hpRate > 0.25f) {
		m_hpBarFront.SetMulColor({ 1.0f, 0.5f, 0.0f, 1.0f }); // オレンジ色
	}
	else {
		m_hpBarFront.SetMulColor({ 1.0f, 0.0f, 0.0f, 1.0f }); // 赤色
	}


    m_hpBarBack.Update();
    m_hpBarFront.Update();

	m_charaIcon.Update();

	m_AttackGauge->Update(m_attackGaugeRate, 0.0f, 0.0f, 3.14f, 0.0f, 0.5f);
}

void PlayerUI::Render(RenderContext& rc)
{
    // 背景 -> 前景の順に描画
    m_hpBarBack.Draw(rc);
    m_hpBarFront.Draw(rc);
	m_charaIcon.Draw(rc);
    m_buttonA.Draw(rc);
	m_buttonB.Draw(rc);
	m_AttackGauge->Draw(rc);
}