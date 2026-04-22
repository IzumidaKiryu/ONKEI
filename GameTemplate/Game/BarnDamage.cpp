#include "stdafx.h"
#include "BarnDamage.h"
#include "EnemyManager.h" // 敵を管理しているクラス
#include "Enemy.h"        // 敵の基底クラス

void BarnDamage::Init(float damage, float range, const Vector3& pos)
{
    m_damage = damage;
    m_range = range;
    m_position = pos;
    m_isExecuted = false;
}

void BarnDamage::Update()
{
    if (m_isExecuted) return;

    // 1. EnemyManagerを探す (シングルトンやFindGOなど、プロジェクトの作法に合わせてください)
    auto enemyManager = FindGO<EnemyManager>("EnemyManager");
    if (enemyManager) {
        // 2. 管理下の敵リストを取得 (std::vector<Enemy*> 等を想定)
        const auto& enemyList = enemyManager->GetEnemyList();

        for (auto enemy : enemyList) {
            if (!enemy) continue;

            // 3. 距離判定 (中心点からの距離)
            Vector3 diff = enemy->GetPosition() - m_position;
            if (diff.Length() <= m_range) {
                // 4. ダメージ適用 (Enemy側にダメージを受ける関数がある想定)
                enemy->OnDamage(m_damage);
            }
        }
    }

    // エフェクトを出したり、SEを鳴らしたりする場合はここに追加
    // 例: NewGO<Effect>(...);

    m_isExecuted = true;
    DeleteGO(this); // ダメージを与えたら自身を削除
}