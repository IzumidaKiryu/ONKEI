#include "stdafx.h"
#include <SkillButton.h>
void SkillButton::Init(const char* skillIconPath, float coolTime)
{
    m_maxCoolTime = coolTime;

    // 1. 下地のアイコン（常に表示される暗いボタン）
    m_baseIcon.Init(skillIconPath, 200.0f, 200.0f);
    m_baseIcon.SetMulColor({ 0.2f, 0.2f, 0.2f, 1.0f }); // 暗くしておく

    // 2. メインのゲージ（時計回りに増える明るいボタン）
    m_gauge = new nsK2Engine::UIGaugeArc();
    // ここでさっき作った全円シェーダーを指定！
    m_gauge->Init(skillIconPath, 200.0f, 200.0f, "Assets/shader/SkillGauge.fx");
    m_gauge->SetPivot({ 0.5f, 0.5f }); // 中心基準
}

void SkillButton::Update()
{
    if (m_isCoolingDown) {
        m_timer += g_gameTime->GetFrameDeltaTime();
        m_hpRate = m_timer / m_maxCoolTime;

        if (m_hpRate >= 1.0f) {
            m_hpRate = 1.0f;
            m_isCoolingDown = false;
            // ★ここで「ピキーン！」と光る演出を入れると最高
        }
    }

    // シェーダーに現在の割合を渡す
    // inner=0.0, outer=0.5 で「円全体」になる
    m_gauge->Update(m_hpRate, 0.0f, -3.14f, 3.14f, 0.0f, 0.5f);

    m_baseIcon.SetPosition(m_position);
    m_gauge->SetPosition(m_position);

    m_baseIcon.Update();
    // m_gaugeのUpdateは内部のSpriteを更新する
}

void SkillButton::Draw(RenderContext& rc)
{
    m_baseIcon.Draw(rc); // 下地を描画
    if (m_gauge) {
        m_gauge->Draw(rc); // ゲージ（明るいアイコン）を描画
    }
}

void SkillButton::UseSkill()
{
    if (!m_isCoolingDown) {
        m_timer = 0.0f;
        m_hpRate = 0.0f;
        m_isCoolingDown = true;
    }
}