#pragma once
#include "GameObject/IGameObject.h"

// 音ゲーの結果次第で発動するバーンダメージ
class BarnDamage : public IGameObject
{
public:
    // 初期化: ダメージ量、攻撃範囲、中心座標を受け取る
    // ※EnemyManagerがグローバル、あるいはFindGOで見つけられる前提です
    void Init(float damage, float range, const Vector3& pos);

    void Update() override;

private:
    float m_damage = 0.0f;
    float m_range = 0.0f;
    Vector3 m_position = Vector3::Zero;
    bool m_isExecuted = false; // 1フレームだけ判定を行うためのフラグ
};