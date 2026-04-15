#include "stdafx.h"
#include "EnemyManager.h"
#include "Enemy.h"
#include "Boss.h"

void EnemyManager::Init() {
    // 1. 座標データテーブルを作成
    // ここに座標を追加するだけで敵が増やせます
    m_spawnList = {
        { { 7000.0f, 0.0f, -12000.0f } },
        { { 7100.0f, 0.0f, -3500.0f } },
        { { 7000.0f, 0.0f, 100.0f } },
        { { 7000.0f, 0.0f, -300.0f } },
        { { 4200.0f, 0.0f, -7100.0f } },
        { { 4400.0f, 0.0f, -7100.0f } },
        { { -7000.0f, 0.0f, -7000.0f } },
        { { -4000.0f, 0.0f, -7000.0f } },
        { { -10000.0f, 0.0f, -7000.0f } },
        { { 7000.0f, 0.0f, -3500.0f } },
        { { 7200.0f, 0.0f, -3500.0f } },
        { { 7300.0f, 0.0f, -3500.0f } },
        { { 7400.0f, 0.0f, -3500.0f } },
    };

    // 2. 敵の生成を実行
    SpawnEnemies();
}

void EnemyManager::SpawnEnemies() {
    // ループ回数はデータの数（m_spawnList.size()）だけ回す
    for (const auto& data : m_spawnList) {
        Enemy* enemy = NewGO<Enemy>(0); // 優先度は一括で0などでOK
        enemy->m_position = data.pos;
        enemy->m_firstPos = data.pos;

        // リストに追加
        m_enemies.push_back(enemy);
    }
}

// 敵から「消えるよ」と通知が来たらリストから消す
void EnemyManager::OnEnemyDestroy(Enemy* enemy) {
    auto it = std::find(m_enemies.begin(), m_enemies.end(), enemy);
    if (it != m_enemies.end()) {
        m_enemies.erase(it);
    }
}

void EnemyManager::BossSpawn()
{
    m_boss = NewGO<Boss>(0, "boss");

    // 3. ボスの初期位置を設定（ステージの奥など）
    Vector3 bossPos = { 0.0f, 0.0f, 0.0f };
    m_boss->SetPosition(bossPos);
}

void EnemyManager::ClearAllEnemies() {
    // リストのコピーを作成してループを回す（削除中のリスト変更を防ぐため）
    auto currentEnemies = m_enemies;
    for (auto enemy : currentEnemies) {
        DeleteGO(enemy);
    }
    m_enemies.clear();
}

void EnemyManager::Update() {
    // 必要ならここに全滅判定や追加スポーン処理を書く
}