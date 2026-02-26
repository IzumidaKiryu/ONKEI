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
    m_hpBarFront.Init("Assets/sprite/white.DDS", m_hpBarWidth, m_hpBarHeight);
    m_hpBarFront.SetMulColor({ 0.0f, 1.0f, 0.0f, 1.0f }); // 緑色
    m_hpBarFront.SetPivot({ 0.0f, 0.5f }); // 左端を起点にする

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
    Vector3 uiPos = { -600.0f, 400.0f, 0.0f };
    m_hpBarBack.SetPosition(uiPos);
    m_hpBarFront.SetPosition(uiPos);

    m_hpBarBack.Update();
    m_hpBarFront.Update();
}

void PlayerUI::Render(RenderContext& rc)
{
    // 背景 -> 前景の順に描画
    m_hpBarBack.Draw(rc);
    m_hpBarFront.Draw(rc);
}