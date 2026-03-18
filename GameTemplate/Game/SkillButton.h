#pragma once

#include "UIGaugeArc.h"
class SkillButton : public IGameObject
{
public:
    SkillButton()=default;
    ~SkillButton()=default;

    // skillIconPath: ボタンの中身の画像, coolTime: スキルが溜まる秒数
    void Init(const char* skillIconPath, float coolTime);
    void Update(float nowGauge);
    void Draw(RenderContext& rc);

    // スキルを使用する（リキャスト開始）
    void UseSkill();

    // 座標の設定
    void SetPosition(const Vector3& pos) { m_position = pos; }

    // スキルが使用可能か？
    bool IsReady() const { return !m_isCoolingDown; }

private:
    nsK2Engine::UIGaugeArc* m_gauge = nullptr; // さっき作った全円シェーダーを適用する板
    SpriteRender m_baseIcon;                      // 下地の暗いアイコン

	float m_nowSkillGauge = 0.0f;        // 現在のスキルゲージの値
    float m_maxCoolTime = 5.0f;  // リキャスト秒数
    float m_hpRate = 0.0f;       // 0.0 ～ 1.0
    bool m_isCoolingDown = true;

    Vector3 m_position = Vector3(350.0f,-50.0f,0.0f);
};