#pragma once
#include <vector> // vectorを使えるようにする
#include "Enemy.h"

class Enemy;
class Boss;

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
    void SpawnEnemies(); // 敵を生成する関数
	void BossSpawn(); // ボスをスポーンさせる関数
	void SetBossDeathFlag(bool isDead) { m_BossDeathFlag = isDead; }
	bool GetBossDeathFlag() { return m_BossDeathFlag; }

    // 管理している敵のリストを返すゲッターを追加
    const std::vector<Enemy*>& GetEnemyList() const {
        return m_enemies;
    }

    Boss* GetBoss() const { return m_boss; }
private:

    Boss* m_boss = nullptr; // ボスのポインタ
    // 配置データをリスト化して持つ
    std::vector<EnemySpawnData> m_spawnList;
    std::vector<Enemy*> m_enemies; // 生成したエネミーを覚えておくリスト
    float m_spawnTimer = 0.0f;
    int m_deathCount = 0;
	bool m_BossDeathFlag = false; // ボスが死んだかどうかのフラグ
	bool m_isSpawnActive = true; // 敵のスポーンが許可されているかどうかのフラグ
	
};