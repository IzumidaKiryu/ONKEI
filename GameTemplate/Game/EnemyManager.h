#pragma once
#include <vector> // vectorを使えるようにする
#include "Enemy.h"

class Enemy;

// 敵の初期配置用データ構造体
struct EnemySpawnData {
    Vector3 pos;
};

class EnemyManager : public IGameObject {
public:
    void Init();
    void Update() override;

    void CountUpDeathCount() { m_deathCount++; }
    int GetDeathCount() const { return m_deathCount; }
    void ClearAllEnemies(); // 全削除用関数
    // 敵が死んだときに呼び出してもらう関数
    void OnEnemyDestroy(Enemy* enemy);
private:
    void SpawnEnemies(); // 敵を一括生成する関数

    float m_spawnTimer = 0.0f;
    int m_deathCount = 0;

    // 配置データをリスト化して持つ
    std::vector<EnemySpawnData> m_spawnList;
    std::vector<Enemy*> m_enemies; // 生成したエネミーを覚えておくリスト
};