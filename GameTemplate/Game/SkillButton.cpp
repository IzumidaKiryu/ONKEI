#include "stdafx.h"
#include <SkillButton.h>
void SkillButton::Init(const char* skillIconPath, float coolTime)
{
    m_maxCoolTime = coolTime;
   
    // ゲージ（シェーダーで「暗い部分」と「明るい部分」を塗り分ける）
    m_gauge = new nsK2Engine::UIGaugeArc();
    m_gauge->Init(skillIconPath, 750.0f, 400.0f, "Assets/shader/SkillGauge.fx");
    m_gauge->SetPivot({ 0.5f, 0.5f }); // 中心基準
}

void SkillButton::Update(float nowGauge)
{
    if (m_isCoolingDown) {
        m_nowSkillGauge = nowGauge;
        m_hpRate = m_nowSkillGauge / m_maxCoolTime;

        if (m_hpRate >= 1.0f) {
            m_hpRate = 1.0f;
            m_isCoolingDown = false;
            // ここで「ピキーン！」と光る演出（オーラなど）を出すならフラグを立てる
        }
    }

    // 座標の更新
    m_gauge->SetPosition(m_position);

    // シェーダーに現在の割合を渡す
    // outer=0.5で円の端まで。inner=0.0で塗りつぶし
    m_gauge->Update(m_hpRate, 0.0f, 0.0f, 3.14f, 0.0f, 0.5f);
}

void SkillButton::Draw(RenderContext& rc)
{
    if (m_gauge) {
        m_gauge->Draw(rc);
    }
}

void SkillButton::UseSkill()
{
    if (!m_isCoolingDown) {
        m_nowSkillGauge = 0.0f;
        m_hpRate = 0.0f;
        m_isCoolingDown = true;
    }
}